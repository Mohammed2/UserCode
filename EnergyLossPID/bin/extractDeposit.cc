#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

using namespace std;

#define Sqr(x) ((x) * (x))

#include "TFile.h"
#include "TH3I.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TF2.h"

#include "TMath.h"

#include "../interface/Enumerators.h"

TH1D * histo;
TH1D * sigma1;

double bg,path,y;
int ilbg,ipath,iy;

/*****************************************************************************/
void printToFile(const TH1D & h1, const char* name)
{
  char fileName[128];
  sprintf(fileName,"../out/depositCurves/%s.dat",name);

  ofstream file(fileName);

  for(int i = 1; i <= h1.GetNbinsX(); i++)
    file << " " << h1.GetBinCenter(i)
         << " " << h1.GetBinContent(i)
         << " " << h1.GetBinError(i)
         << " " << h1.GetBinWidth(i)
         << " " << h1.GetBinLowEdge(i)
         << " " << h1.GetBinLowEdge(i+1)
         << endl;
  file.close();
}

/*****************************************************************************/
void printToFile(const TH2D & h2, const char* name)
{
  char fileName[128];
  sprintf(fileName,"../out/depositCurves/%s.dat",name);

  ofstream file(fileName);

  for(int i = 1; i <= h2.GetNbinsX(); i++)
  {
    for(int j = 1; j <= h2.GetNbinsY(); j++)
      file << " " << h2.GetXaxis()->GetBinLowEdge(i)
           << " " << h2.GetYaxis()->GetBinLowEdge(j)
           << " " << h2.GetBinContent(i,j)
           << " " << h2.GetBinError(i,j)
           << " " << h2.GetXaxis()->GetBinCenter(i)
           << " " << h2.GetYaxis()->GetBinCenter(j)
           << " " << h2.GetXaxis()->GetBinWidth(i)
           << " " << h2.GetYaxis()->GetBinWidth(j)
           << endl;

      file << " " << h2.GetXaxis()->GetBinLowEdge(i)
           << " " << h2.GetYaxis()->GetXmax()
           << " 0 0 0. 0. 0. 0." << endl;
    file << endl;
  }

  for(int j = 1; j <= h2.GetNbinsY(); j++)
    file << " " << h2.GetXaxis()->GetXmax()
         << " " << h2.GetYaxis()->GetBinLowEdge(j)
         << " 0 0 0. 0. 0. 0." << endl;

    file << " " << h2.GetXaxis()->GetXmax()
         << " " << h2.GetYaxis()->GetXmax()
         << " 0 0 0. 0. 0. 0." << endl;

  file.close();
}

/*****************************************************************************/
double GetMedian(TH1D * histo)
{
   int n = histo->GetXaxis()->GetNbins();  
   vector<double> x(n);
   histo->GetXaxis()->GetCenter( &x[0] );
   const double * y = histo->GetArray(); 

   // exclude underflow/overflows from bin content array y
   return TMath::Median(n, &x[0], &y[1]); 
}

/*****************************************************************************/
void options(int arg, char **arc)
{
  cerr << "\033[22;31m"
       << "Extracting deposit curves.."
       << "\033[22;0m"
       << endl;
}

/*****************************************************************************/
int main(int arg, char **arc)
{
  options(arg,arc);

  char fileName[256];
  sprintf(fileName,"../out/data_2.root");
  TFile file(fileName);

  // Look at all detectors
  for(int det = 0; det < nDets; det++)
  for(int cha = pos; cha < nCharges; cha++)
  for(int part = 0; part < nParts; part++)
  if(part == unknown)
  {
    char name[256];

    sprintf(name,"%s_%s",chargeNames[cha].c_str(),
                            detNames[det].c_str());
    cerr << " " << name << " : ";

    sprintf(name,"deposit_%s_%s_%s",chargeNames[cha].c_str(),
                                     partNames[part].c_str(),
                                       detNames[det].c_str());
    TH3I * deposits = (TH3I *) file.Get(name);

    sprintf(name,"esigma_%s_%s_%s",chargeNames[cha].c_str(),
                                    partNames[part].c_str(),
                                      detNames[det].c_str());
    TH3I * esigmas  = (TH3I *) file.Get(name);

    // Take all bg
    for(ilbg  = 1; ilbg  <= deposits->GetXaxis()->GetNbins(); ilbg++ )
    {
    bg = exp(deposits->GetXaxis()->GetBinCenter(ilbg));

    bool plot = (
        (bg > 0.50 && bg < 0.52) ||
        (bg > 0.69 && bg < 0.71) ||
        (bg > 0.83 && bg < 0.87) ||
        (bg > 0.93 && bg < 0.97) ||
        (bg > 1.18 && bg < 1.20) ||
        (bg > 1.38 && bg < 1.40) ||
        (bg > 1.68 && bg < 1.70) ||
        (bg > 2.07 && bg < 2.09) ||
        (bg > 2.52 && bg < 2.54) ||
        (bg > 3.09 && bg < 3.11) ||
        (bg > 3.45 && bg < 3.55) ||
        (bg > 3.98 && bg < 4.12) ||
        (bg > 4.42 && bg < 4.58) ||
        (bg > 4.90 && bg < 5.10) || //
        (bg > 5.40 && bg < 10.) );

    if(plot)
    {
      // Get deposit with bg
      deposits->GetXaxis()->SetRange(1,0);
      deposits->GetXaxis()->SetRange(ilbg,ilbg);
  
      sprintf(name,"zy");
      TH2D * deposit = (TH2D *) deposits->Project3D(name);

      // Get sigmas with bg
      esigmas->GetXaxis()->SetRange(1,0);
      esigmas->GetXaxis()->SetRange(ilbg,ilbg);

      sprintf(name,"zy");
      TH2D * esigma = (TH2D *) esigmas->Project3D(name);

      // <sigmaEnergy> vs path, dummy
      sigma1 = esigma->ProjectionX("zy", 1, 1);

      // Normalize
      int nlines = 0;
      for(ipath = 1; ipath <= deposit->GetXaxis()->GetNbins(); ipath++)
      {
        TH1D * sigma = esigma->ProjectionY("sig", ipath, ipath);

        // why mean
        sigma1->SetBinContent(ipath, sigma->GetMean());

        double s = 0.;
        for(iy = 1; iy <= deposit->GetYaxis()->GetNbins(); iy++)
          s += deposit->GetBinContent(ipath,iy);
  
        if(s > 100.)
        {
          for(iy = 1; iy <= deposit->GetYaxis()->GetNbins(); iy++)
          {
            double value = deposit->GetBinContent(ipath,iy);
            double error = (value >= 0 ? sqrt(value) : 1); // CHECK

            deposit->SetBinContent(ipath,iy, value / s);
            deposit->SetBinError  (ipath,iy, error / s);
          }

          nlines++;
        }
        else
          for(iy = 1; iy <= deposit->GetYaxis()->GetNbins(); iy++)
            deposit->SetBinContent(ipath,iy,0.);

        gDirectory->Delete("sig"); 
      }

      // If enough lines, fit
      if(nlines >= 0)
      {
        char name[256];

        sprintf(name,"his_%s_%s_%s_%.2f",detNames[det].c_str(),
                                      chargeNames[cha].c_str(),
                                       partNames[part].c_str(),
                                      bg);
        printToFile(*deposit, name);

        sprintf(name,"sig_%s_%s_%s_%.2f",detNames[det].c_str(),
                                      chargeNames[cha].c_str(),
                                       partNames[part].c_str(),
                                      bg);
        printToFile(*sigma1, name);

        cerr << "+";
      }
      else
        cerr << "x";

      gDirectory->Delete("*zy"); 
    }
    }

    cerr << endl; 
  }

  return 0;
}
