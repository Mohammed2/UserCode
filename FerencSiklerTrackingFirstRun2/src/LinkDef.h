#include "../interface/TBunchCrossing.h"
#include "../interface/TVertex.h"
#include "../interface/TTrack.h"

#include "../interface/TPixelHit.h"
#include "../interface/TStripHit.h"

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class TBunchCrossing+;
#pragma link C++ class TVertex+;
#pragma link C++ class TTrack+;

#pragma link C++ class TPixelHit+;
#pragma link C++ class TStripHit+;

#pragma link C++ class pair<bool,bool>+;

#pragma link C++ class pair<short,short>+;

#pragma link C++ class pair<short int,short int>+;
#pragma link C++ class               pair<uint8_t, uint8_t>+;
#pragma link C++ class pair<uint8_t, pair<uint8_t, uint8_t> >+;

#endif
