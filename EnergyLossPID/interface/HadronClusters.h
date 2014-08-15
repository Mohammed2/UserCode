#ifndef _HadronClusters_h_
#define _HadronClusters_h_

#include <fstream>
#include <vector>

namespace edm { class EventSetup; class ParameterSet; }

class SiPixelRecHit;
class SiStripRecHit2D;

class TrackingRecHit;

//class PixelThresholdClusterizer;

#include "DataFormats/GeometryVector/interface/LocalVector.h"

#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHitCollection.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripRecHit2DCollection.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripMatchedRecHit2DCollection.h"
#include "DataFormats/TrackerRecHit2D/interface/ProjectedSiStripRecHit2D.h"

#include "RecoLocalTracker/SiPixelClusterizer/interface/PixelClusterizerBase.h"

#include "CalibTracker/SiPixelESProducers/interface/SiPixelGainCalibrationServiceBase.h"

class TrackerGeometry;
class ClusterShapeHitFilter;
class Trajectory;

class TTrack;

class HadronClusters : public PixelClusterizerBase
{
 public:
   HadronClusters(const edm::EventSetup& es, const edm::ParameterSet& pset);
   ~HadronClusters();

  void analyzeRecTrajectory(const Trajectory & trajectory, TTrack & r);

  // dummy
  void clusterizeDetUnit( const edm::DetSet<PixelDigi> & input, 
                          const PixelGeomDetUnit * pixDet,
                          const std::vector<short>& badChannels,
                          edmNew::DetSetVector<SiPixelCluster>::FastFiller&
output) {}


 private:
  void processRec(const SiPixelRecHit &   recHit,
                  LocalVector ldir, TTrack & r, float, float);
  void processRec(const SiStripRecHit2D & recHit,
                  LocalVector ldir, TTrack & r, float, float);

  const TrackerGeometry * theTracker;
  const ClusterShapeHitFilter * theClusterShape;

  SiPixelGainCalibrationServiceBase * theSiPixelGainCalibration_;
};

#endif
