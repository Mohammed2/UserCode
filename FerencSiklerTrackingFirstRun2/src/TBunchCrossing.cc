#include "../interface/TBunchCrossing.h"
#include "../interface/TCaloTower.h"
#include "../interface/TVertex.h"

ClassImp(TBunchCrossing)

TBunchCrossing::TBunchCrossing()
{
}

TBunchCrossing::~TBunchCrossing()
{
}

void TBunchCrossing::Clear()
{
  triggerNames.clear();

  hfTowers.clear();

  simVertices.clear();
  recVertices.clear();
}
