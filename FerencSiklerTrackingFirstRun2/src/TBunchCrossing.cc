#include "../interface/TBunchCrossing.h"
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

  simVertices.clear();
  recVertices.clear();
}
