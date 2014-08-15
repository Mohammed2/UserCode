#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iomanip>

#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"

#include "TFile.h"
#include "TH2S.h"

using namespace std;

#include "../interface/Enumerators.h"

class Value
{
 public:
  int x;
  int y;
  int z;
};

vector<Value> values;
int allhits;
double thr;

vector<double> thresholds(nDets);

/*****************************************************************************/
void readThresholds()
{
  char fileName[256];
  sprintf(fileName,"../data/stripThresholds.par");

  ifstream file(fileName);
  cerr << " reading " << fileName << endl;
   
  while(!file.eof())
  {
    char detName[256];
    double t;

    file >> detName;
    file >> t;
   
    for(int i = 0; i < nDets; i++)
    if(strcmp(detName,detNames[i].c_str()) == 0)
      thresholds[i] = t; 
  }

  file.close();
}

/*****************************************************************************/
void readData(TFile * fileRoot, int det)
{
  char name[256];
  sprintf(name,"coupling_%s", detNames[det].c_str());

  TH2S * histo = (TH2S *) fileRoot->Get(name);

  thr = thresholds[det];

  values.clear();

  allhits = 0;

  int lowerx, uppery;

  if(det == TOB || det == TEC5) { lowerx = 50; uppery = 50; }
                           else { lowerx = 50; uppery = 30; }

  for(int x = lowerx; x < 250   ; x++) // CHECK
  for(int y =      0; y < uppery; y++) // CHECK
  {
    int z = histo->GetBinContent(x,y);

    if(z > 0 && y > thr + 2)
    {
      Value val;

      val.x = x;
      val.y = y;
      val.z = z;

      allhits += z;

      values.push_back(val);
    }
  }

  delete histo;
}

/*****************************************************************************/
double LogLikelihood(const double *par)
{
  int i = 0;
  double alpha  = par[i++];
  double sigma  = par[i++];
  
  double r = alpha / (1 - 2*alpha);

  vector<pair<double,double> > sum(254, pair<double,double>(0.,0.));

  for(vector<Value>::const_iterator val = values.begin();
                                    val!= values.end(); val++)
  {
    double meas = val->z;
    
    double mean = r * val->x;
    double q = (val->y - mean)/sigma;
    double theo = exp(-0.5 * q*q);

    sum[val->x].first  += meas * theo / (meas); // sigma2 = meas
    sum[val->x].second += theo * theo / (meas);
  }

  vector<double> amp(254, 0.);

  for(int x = 0; x < 254; x++)
    if(sum[x].second > 0)
      amp[x] = sum[x].first / sum[x].second;

  double chi2 = 0.;

  for(vector<Value>::const_iterator val = values.begin();
                                    val!= values.end(); val++)
  {
    double meas = val->z;

    double mean = r * val->x;
    double q = (val->y - mean)/sigma;
    double theo = amp[val->x] * exp(-0.5 * q*q);

    q = meas - theo;
    chi2 += q*q / (meas);
  }

  return chi2;
} 

/*****************************************************************************/
void fitWithMinuit(ofstream & file)
{
  // Number of parameters
  const int npar = 2;

  ROOT::Math::Functor f(&LogLikelihood,npar);

  ROOT::Math::Minimizer * min =
  ROOT::Math::Factory::CreateMinimizer("Minuit", "Migrad");

  min->SetMaxFunctionCalls(1000000);
  min->SetMaxIterations   (1000000);
  min->SetTolerance(1e-3);
  min->SetFunction(f);

  min->SetPrintLevel(0);

  unsigned int i = 0;
  //                           name      val   step  min  max
  min->SetLimitedVariable(i++, "alpha" , 0.1, 1e-3, 0., 0.3);
  min->SetLimitedVariable(i++, "sigma" , 5.0, 1e-2, 0., 30.);

  min->Minimize();

  cerr << fixed << setprecision(4);

  for(int i = 0; i < npar; i++)
    cerr << " \t" <<  min->X()[i]
         << " \t" << min->Errors()[i];

  cerr << " \tstatus=" << min->Status() << " hits=" << allhits << endl;

  file << " \t" << fixed << setprecision(1) << thr         // threshold [adc]
       << " \t" << setprecision(3) << min->X()[0] // alpha
       << " \t" << setprecision(1) << min->X()[1] // sigma [adc]
       << endl;
}

/*****************************************************************************/
void options(int arg, char **arc)
{
  int i = 1;

  do
  {

    i++;
  }
  while(i < arg);
}

/*****************************************************************************/
int main(int arg, char **arc)
{
  options(arg,arc);

  cerr << "\033[22;31m"
       << "Evaluating strip properties..."
       << "\033[22;0m"
       << endl;

  char fileName[256];
  sprintf(fileName,"../out/data_-1.root");
  TFile * fileRoot = new TFile(fileName);

  sprintf(fileName,"../data/stripProps.par");
  ofstream file(fileName);

  readThresholds();
  
  // Truncated linear regression
  cerr << " ----------------------------------------------------------" << endl;
  cerr << " det    alpha           sigma" << endl;
  cerr << " ----------------------------------------------------------" << endl;

  for(int det = TIB; det < nDets; det++)
  {
    cerr << " " << detNames[det];
    file << " " << detNames[det];

    readData(fileRoot, det);
    fitWithMinuit(file);
  }

  file.close();

  fileRoot->Close();

  return 0;
}
