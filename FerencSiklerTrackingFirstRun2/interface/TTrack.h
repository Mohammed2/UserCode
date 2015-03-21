#ifndef _TTrack_h_
#define _TTrack_h_

#include <utility>
#include "TObject.h"

//#include "TPixelHit.h"
//#include "TStripHit.h"

class TTrack : public TObject
{
 public:
  TTrack();
  virtual ~TTrack();

  short int charge;
  float eta;
  float pt;
  float phi;

  float z;
  float d0;

  float chi2;
  short int ndf;

//  std::pair<float,float> epsilon;
//  std::vector<TPixelHit> pixelHits;
//  std::vector<TStripHit> stripHits;
  short int nTrackerHits;

  bool isPrimary;

  // associated tracks
  typedef std::pair<short int,short int> aTrack_t;
  std::vector<aTrack_t> aTracks;

  short int pdgId;
  short int parentId;

  short int algo;

  ClassDef(TTrack,1)
};

#endif
