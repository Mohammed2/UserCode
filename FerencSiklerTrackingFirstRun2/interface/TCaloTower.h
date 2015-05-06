#ifndef _TCaloTower_h_
#define _TCaloTower_h_

#include "TObject.h"
#include <vector>

class TCaloTower : public TObject
{
 public:
  TCaloTower();
  virtual ~TCaloTower(); 

  float eta;
  float energy;

  ClassDef(TCaloTower,1)
};

#endif
