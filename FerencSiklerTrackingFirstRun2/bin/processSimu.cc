#include <cstdlib>
#include <iostream>

using namespace std;

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

#include "../interface/TBunchCrossing.h"
#include "../interface/TVertex.h"

/*****************************************************************************/
void processEvent(const TBunchCrossing * bunx)
{
  cerr << "  s" << bunx->simVertices.size()
       << "  r" << bunx->recVertices.size()
       << endl;

  // Simulated
  for(vector<TVertex>::const_iterator
              vertex = bunx->simVertices.begin();
              vertex!= bunx->simVertices.end(); vertex++)
  {
    for(vector<TTrack>::const_iterator track = vertex->tracks.begin();
                                       track!= vertex->tracks.end();
                                       track++)
    {
    }
  }

  // Reconstructed
  for(vector<TVertex>::const_iterator
            vertex = bunx->recVertices.begin();
            vertex!= bunx->recVertices.end(); vertex++)
  {
    for(vector<TTrack>::const_iterator track = vertex->tracks.begin();
                                       track!= vertex->tracks.end();
                                       track++)
    {
    }
  }
}

/*****************************************************************************/
int main(int arg, char **arc)
{
  TBunchCrossing * bunx = new TBunchCrossing();

  TFile * fileData = TFile::Open("hadronTree.root","read");
  TTree * tree = (TTree *) fileData->Get("hadronTree");
  int nBunchCrossings = tree->GetEntries();

  TBranch * branch = tree->GetBranch("bunchCrossing");
  branch->SetAddress(&bunx);

  for(int i = 0; i < nBunchCrossings; i++)
  {
    branch->GetEntry(i); // read
    processEvent(bunx);  // process
    bunx->Clear();       // clear
  }

  delete bunx;
}
