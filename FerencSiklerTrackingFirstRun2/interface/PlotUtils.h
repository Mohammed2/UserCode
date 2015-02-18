#ifndef _HadronSpectra_PlotUtils_h_
#define _HadronSpectra_PlotUtils_h_

#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"

#include <fstream>

class PlotUtils
{
  public:
    void printHelix(const GlobalPoint& p1, const GlobalPoint& p2,
                    const GlobalVector& n2, std::ofstream& outFile, int charge);
};

#endif
