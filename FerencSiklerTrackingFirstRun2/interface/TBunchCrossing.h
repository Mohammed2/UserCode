#ifndef _TBunchCrossing_h_
#define _TBunchCrossing_h_

#include "TObject.h"
#include <vector>
#include <string>

class TVertex;

class TBunchCrossing : public TObject
{
 public:
  TBunchCrossing();
  virtual ~TBunchCrossing();

  int processId;

  int runNumber;
  int lumiSection;
  int bxNumber;

  // triggers
  std::vector<std::string> triggerNames;

  // HF
  std::pair<short int,short int> hfTowers;

  // vertices
  std::vector<TVertex> simVertices;
  std::vector<TVertex> recVertices;

  void Clear();

  ClassDef(TBunchCrossing,1)
};

#endif
