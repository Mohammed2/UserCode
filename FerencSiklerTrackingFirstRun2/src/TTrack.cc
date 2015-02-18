#include "../interface/TTrack.h"

ClassImp(TTrack)

TTrack::TTrack()
{
  pdgId    = 0;
  parentId = 0;

  nAssoc = 0;
//  aTrack = aTrack_t(-1, -1);

//  pixelHits.clear();
//  stripHits.clear();
}

TTrack::~TTrack()
{
}
