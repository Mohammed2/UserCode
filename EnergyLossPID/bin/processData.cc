#include "../interface/DataHandler.h"

#include <cstdlib>
#include <iostream>

using namespace std;

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

#include "../interface/TBunchCrossing.h"

int filesFrom, filesTo, chunk;
int nFiles;
char eospath[256];

/*****************************************************************************/
void options(int arg, char **arc)
{
  nFiles = 0;
  int nChunks = 0;
  chunk = 0;

  int i = 1;

  do
  {
    if(strcmp(arc[i],"-nfiles" ) == 0) nFiles  = atoi(arc[++i]);
    if(strcmp(arc[i],"-nchunks") == 0) nChunks = atoi(arc[++i]);
    if(strcmp(arc[i],"-chunk"  ) == 0) chunk   = atoi(arc[++i]);

    if(strcmp(arc[i],"-eospath" ) == 0) sprintf(eospath,"%s",arc[++i]);

    i++;
  }
  while(i < arg);

  if(nChunks == 0)
  { 
    filesFrom = 1;
    filesTo   = nFiles;
  }
  else
  { 
    filesFrom = int( (float(nFiles)/nChunks) * (chunk - 1) + 1.5 );
    filesTo   = int( (float(nFiles)/nChunks) *  chunk      + 0.5 );

    if(chunk == 1)       filesFrom = 1;
    if(chunk == nChunks) filesTo   = nFiles;
  }

  cerr << "   (files " << filesFrom << " -> " << filesTo << ")" << endl;
}

/*****************************************************************************/
int main(int arg, char **arc)
{
  options(arg,arc);

  DataHandler theDataHandler;

  theDataHandler.options(arg,arc,chunk);
  theDataHandler.beginJob();

  TBunchCrossing * bunx = new TBunchCrossing();

  ifstream fileList("list.txt");

  for(int j = 1; j <= nFiles; j++)
  {
    char eosname[256];
    fileList >> eosname;

    char fileName[256];
    sprintf(fileName, "root://eoscms//%s/%s", eospath, eosname);

    if(j >= filesFrom && j <= filesTo)
    {
      // Open file
      TFile * fileData = TFile::Open(fileName,"read");
//      cerr << "  opening " << eosname << endl;
  
      // Get tree
      TTree * tree = (TTree *) fileData->Get("hadronTree");
  
      // Read in
      int nBunchCrossings = tree->GetEntries();
  
      cerr << "  file " << j << " with " << nBunchCrossings << " bx ";
  
      int times = 1;
      if(nBunchCrossings >= 30) times = nBunchCrossings / 30;
  
      TBranch * branch = tree->GetBranch("bunchCrossing");
  
      if(branch != 0)
      {
        branch->SetAddress(&bunx);
    
        for(int i = 0; i < nBunchCrossings; i++) //
        {
          if(i % times == 0) cerr << ".";
    
          // Read
          branch->GetEntry(i);
  
          // Process
          theDataHandler.processEvent(bunx);
  
            // Clear
          bunx->Clear();
        }
        cerr << " [done]" << endl;
      }
    }
  }

  fileList.close();

  delete bunx;

  theDataHandler.endJob(chunk);
}
