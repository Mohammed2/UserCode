#include <cstdlib>
#include <fstream>
#include <iostream>

#include <cmath>

#include "TFile.h"
#include "TH2S.h"
#include "TH2F.h"

#include "../interface/Enumerators.h"

//
using namespace std;

int pass;
char fileName[256];

/*****************************************************************************/
void printToFile(const TH2S & h2, const char* name)
{
  sprintf(fileName,"../out/%s.dat",name);
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
void printToFile(const TH2F & h2, const char* name, int pass)
{
  sprintf(fileName,"../out/%s_%d.dat",name,pass);
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
void options(int arg, char **arc)
{
  int i = 1;
  do
  {
    if(strcmp(arc[i],"-pass")   == 0) pass = atoi(arc[++i]);

    i++;
  }
  while(i < arg);
}

/*****************************************************************************/
void writeRecCoupling(TFile & file)
{
  cerr << " recCoupling:" << endl;

  char name[256];

  for(int det = TIB; det < nDets; det++)
  {
    sprintf(name,"coupling_%s", detNames[det].c_str());
    cerr << "  " << name << endl;

    TH2S * histo = (TH2S *) file.Get(name);
    printToFile(*histo, name);

    delete histo;
  }
} 

/*****************************************************************************/
void writeRecEloss(TFile & file)
{
  cerr << " recEloss:" << endl;

  char name[256];

  for(int cha = pos; cha < nCharges; cha++)
  for(int ver = 0; ver < nVers; ver++)
  {
    sprintf(name,"histos_%s_%s", verNames[ver].c_str(),
                              chargeNames[cha].c_str());
    cerr << "  " << name << endl;

    TH2F * histo = (TH2F *) file.Get(name);
    printToFile(*histo, name, pass);

    delete histo;
  }
}

/*****************************************************************************/
int main(int arg, char **arc)
{
  options(arg,arc);

  sprintf(fileName,"../out/data_%d.root", pass);
  TFile file(fileName);

  if(pass ==-1) writeRecCoupling(file);
  if(pass >= 0) writeRecEloss   (file);

  file.Close();

  return 0;
}
