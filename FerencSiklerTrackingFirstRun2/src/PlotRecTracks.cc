#include "../interface/PlotRecTracks.h"
#include "../interface/PlotUtils.h"
#include "../interface/PlotRecHits.h"
#include "../interface/HitInfo.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"

#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"

#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHit.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripRecHit2D.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripMatchedRecHit2D.h"
#include "DataFormats/TrackerRecHit2D/interface/ProjectedSiStripRecHit2D.h"

#include "DataFormats/SiPixelDetId/interface/PXBDetId.h"
#include "DataFormats/SiPixelDetId/interface/PXFDetId.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "TrackingTools/PatternTools/interface/Trajectory.h"

#include "SimTracker/TrackerHitAssociation/interface/TrackerHitAssociator.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"

#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"

#include "TrackingTools/TrajectoryState/interface/FreeTrajectoryState.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"

#include "TrackingTools/MaterialEffects/interface/PropagatorWithMaterial.h"

#include "DataFormats/GeometrySurface/interface/Cylinder.h"

using namespace std;

/*****************************************************************************/
PlotRecTracks::PlotRecTracks
  (const edm::EventSetup& es_, string trackProducer_, bool hasSimInfo_,
   ofstream& file_) : es(es_), 
   trackProducer(trackProducer_), hasSimInfo(hasSimInfo_), file(file_)
{
  // Get tracker geometry
  edm::ESHandle<TrackerGeometry> trackerHandle;
  es.get<TrackerDigiGeometryRecord>().get(trackerHandle);
  theTracker = trackerHandle.product();

  // Get magnetic field
  edm::ESHandle<MagneticField> magField;
  es.get<IdealMagneticFieldRecord>().get(magField);
  theMagField = magField.product();

  // Get propagator
  edm::ESHandle<Propagator> thePropagatorHandle;
  es.get<TrackingComponentsRecord>().get("PropagatorWithMaterial",
                                          thePropagatorHandle);
  thePropagator = thePropagatorHandle.product();
}

/*****************************************************************************/
PlotRecTracks::~PlotRecTracks()
{
}

/*****************************************************************************/
string PlotRecTracks::getPixelInfo
  (const TrackingRecHit* recHit, const ostringstream& o, const ostringstream& d)
{
  const SiPixelRecHit* pixelRecHit =
    dynamic_cast<const SiPixelRecHit *>(recHit);

  DetId id = recHit->geographicalId();
  LocalPoint lpos = recHit->localPosition();
  GlobalPoint p = theTracker->idToDet(id)->toGlobal(lpos);

  SiPixelRecHit::ClusterRef const& cluster = pixelRecHit->cluster();
  vector<SiPixelCluster::Pixel> pixels = cluster->pixels();

  string info = ", Text[StyleForm[\"" + o.str() + "\", URL->\"Track " + o.str() + d.str();

  if(0 && hasSimInfo)
  {
    vector<PSimHit> simHits = theHitAssociator->associateHit(*recHit);

    ostringstream o;
    o << "simTrack (nAssoc="  << simHits.size()
               << " trackId=" << simHits[0].trackId()
                   << " pid=" << simHits[0].particleType()
                  << " proc=" << simHits[0].processType() << ")";

    info += " | " + o.str();
  }

  {
    ostringstream o;
    o << theTracker->idToDet(id)->subDetector();

    info += " | " + o.str();
  }

  info += HitInfo::getInfo(*recHit);

  info += "\"]";

  {
    ostringstream o;
    o << ", {" << p.x() << "," << p.y() << ",(" << p.z() << "-zs)*mz},"
      << " {" << 0 << "," << -1 << "}]";

    info += o.str();
  }

  return info;
}

/*****************************************************************************/
string PlotRecTracks::getStripInfo
  (const TrackingRecHit* recHit, const ostringstream& o, const ostringstream& d)
{
  DetId id = recHit->geographicalId();
  LocalPoint lpos = recHit->localPosition();
  GlobalPoint p = theTracker->idToDet(id)->toGlobal(lpos);

  string info = ", Text[StyleForm[\"" + o.str() + "\", URL->\"Track " + o.str() + d.str(); 

  if(0 && hasSimInfo)
  {
    vector<PSimHit> simHits = theHitAssociator->associateHit(*recHit);

    if(simHits.size() > 0)
    {
    ostringstream o;
    o << "simTrackId=" << simHits[0].trackId() ;

    info += " | " + o.str();
    }
  }

  {
    ostringstream o;
    o << theTracker->idToDet(id)->subDetector();

    info += " | " + o.str();
  }

  info += HitInfo::getInfo(*recHit);

  const SiStripMatchedRecHit2D* stripMatchedRecHit =
    dynamic_cast<const SiStripMatchedRecHit2D *>(recHit);
  const ProjectedSiStripRecHit2D* stripProjectedRecHit =
    dynamic_cast<const ProjectedSiStripRecHit2D *>(recHit);
  const SiStripRecHit2D* stripRecHit =
    dynamic_cast<const SiStripRecHit2D *>(recHit);

  if(stripMatchedRecHit != 0)   info += " matched";
  if(stripProjectedRecHit != 0) info += " projected";
  if(stripRecHit != 0)          info += " single";
  
  info += "\"]";

  {
    ostringstream o;
    o << ", {" << p.x() << "," << p.y() << ",(" << p.z() << "-zs)*mz},"
      << " {" << 0 << "," << -1 << "}]";

    info += o.str();
  }

  return info;
}

/*****************************************************************************/
FreeTrajectoryState PlotRecTracks::getTrajectoryAtOuterPoint
  (const reco::Track& track)
{
  GlobalPoint position(track.outerX(),
                       track.outerY(),
                       track.outerZ());

  GlobalVector momentum(track.outerPx(),
                        track.outerPy(),
                        track.outerPz());

  GlobalTrajectoryParameters gtp(position,momentum,
                                 track.charge(),theMagField);

  FreeTrajectoryState fts(gtp,track.outerStateCovariance());

  return fts;
}

/*****************************************************************************/
void PlotRecTracks::printRecTracks(const edm::Event& ev)
{
  if(0 && hasSimInfo)
//    theHitAssociator = new TrackerHitAssociator::TrackerHitAssociator(ev);
    theHitAssociator = new TrackerHitAssociator(ev);

  file << ", If[rt, {AbsolutePointSize[6]";

  edm::Handle<reco::TrackCollection> recTrackHandle;
  ev.getByLabel(trackProducer, recTrackHandle);

  edm::Handle<vector<Trajectory> > trajectoryHandle;
  ev.getByLabel(trackProducer, trajectoryHandle);

  const reco::TrackCollection* recTracks    =   recTrackHandle.product();
  const vector<Trajectory>*    trajectories = trajectoryHandle.product(); 

  edm::LogVerbatim("MinBiasTracking") 
       << " [HadronPlotter] recTracks (" << trackProducer << ") "
       << recTracks->size();
  PlotRecHits theRecHits(es,file);

  file << ", RGBColor[0,0,0.4]";
  reco::TrackCollection::const_iterator recTrack = recTracks->begin();

  int i = 0;
  for(vector<Trajectory>::const_iterator it = trajectories->begin();
                                         it!= trajectories->end();
                                         it++, i++, recTrack++)
  {
    ostringstream o; o << i;

    ostringstream d; d << fixed << setprecision(2)
                       << " | d0=" << recTrack->d0() << " cm"
                       << " | z0=" << recTrack->dz() << " cm"
                       << " | eta=" << recTrack->eta()
                       << " | pt=" << recTrack->pt() << " GeV/c";

    const Trajectory* trajectory = &(*it);

    for(vector<TrajectoryMeasurement>::const_reverse_iterator
        meas = trajectory->measurements().rbegin();
        meas!= trajectory->measurements().rend(); meas++)
    {
    const TrackingRecHit* recHit = meas->recHit()->hit();

    if(recHit->isValid())
    {
      DetId id = recHit->geographicalId();
//      LocalPoint lpos = recHit->localPosition();
//      GlobalPoint p = theTracker->idToDet(id)->toGlobal(lpos);

      if(theTracker->idToDet(id)->subDetector() ==
           GeomDetEnumerators::PixelBarrel ||
         theTracker->idToDet(id)->subDetector() ==
           GeomDetEnumerators::PixelEndcap)
      {
        // Pixel
        const SiPixelRecHit* pixelRecHit =
          dynamic_cast<const SiPixelRecHit *>(recHit);

        if(pixelRecHit != 0)
        {
          theRecHits.printPixelRecHit(*pixelRecHit);
          file << getPixelInfo(recHit, o,d);
        }
      }
      else
      {
        // Strip
        const SiStripMatchedRecHit2D* stripMatchedRecHit =
          dynamic_cast<const SiStripMatchedRecHit2D *>(recHit);
        const ProjectedSiStripRecHit2D* stripProjectedRecHit =
          dynamic_cast<const ProjectedSiStripRecHit2D *>(recHit);
        const SiStripRecHit2D* stripRecHit =
          dynamic_cast<const SiStripRecHit2D *>(recHit);

        if(stripMatchedRecHit != 0)
        {
          theRecHits.printStripRecHit(stripMatchedRecHit->monoHit());
          theRecHits.printStripRecHit(stripMatchedRecHit->stereoHit());

          DetId id = stripMatchedRecHit->geographicalId();
          LocalPoint lpos = stripMatchedRecHit->localPosition();
          GlobalPoint p = theTracker->idToDet(id)->toGlobal(lpos);

          file << ", Point[{"<< p.x()<<","<<p.y()<<",("<<p.z()<<"-zs)*mz}]" << endl;
        }

        if(stripProjectedRecHit != 0)
          theRecHits.printStripRecHit((stripProjectedRecHit->originalHit()));

        if(stripRecHit != 0)
          theRecHits.printStripRecHit(*stripRecHit);

        if(stripMatchedRecHit != 0 ||
           stripProjectedRecHit != 0 ||
           stripRecHit != 0)
          file << getStripInfo(recHit, o,d);
      }
      }
    }
  }

  PlotUtils plotUtils;

  // Trajectory
  recTrack = recTracks->begin();

  for(vector<Trajectory>::const_iterator it = trajectories->begin();
                                         it!= trajectories->end();
                                         it++, recTrack++)
  {
    int algo;
    switch(recTrack->algo())
    {
      case reco::TrackBase::lowPtTripletStep    : algo = 1; break;
      case reco::TrackBase::detachedTripletStep : algo = 2; break;
      case reco::TrackBase::pixelPairStep       : algo = 3; break;
      default: algo = 0;
    }

    if(algo == 1) file << ", RGBColor[1,0,0]";
    if(algo == 2) file << ", RGBColor[0.2,0.6,0.2]";
    if(algo == 3) file << ", RGBColor[0.2,0.2,0.6]";

    vector<TrajectoryMeasurement> meas = it->measurements();

    if(it->direction() == oppositeToMomentum)
    for(vector<TrajectoryMeasurement>::reverse_iterator im = meas.rbegin();
                                                        im!= meas.rend(); im++)
    {
      if(im == meas.rbegin())
      {
        GlobalPoint  p2 = (*(im  )).updatedState().globalPosition();
        GlobalVector v2 = (*(im  )).updatedState().globalDirection();
        GlobalPoint  p1 = GlobalPoint(recTrack->vertex().x(),
                                      recTrack->vertex().y(),
                                      recTrack->vertex().z());

        plotUtils.printHelix(p1,p2,v2, file, recTrack->charge());
      }

      if(im+1 != meas.rend())
      {
        GlobalPoint  p1 = (*(im  )).updatedState().globalPosition();
        GlobalPoint  p2 = (*(im+1)).updatedState().globalPosition();
        GlobalVector v2 = (*(im+1)).updatedState().globalDirection();

        plotUtils.printHelix(p1,p2,v2, file, recTrack->charge());
      }
    }
    else
    for(vector<TrajectoryMeasurement>::iterator im = meas.begin();
                                                im!= meas.end(); im++)
    {
      if(im == meas.begin())
      {
        GlobalPoint  p2 = (*(im  )).updatedState().globalPosition();
        GlobalVector v2 = (*(im  )).updatedState().globalDirection();
        GlobalPoint  p1 = GlobalPoint(recTrack->vertex().x(),
                                      recTrack->vertex().y(),
                                      recTrack->vertex().z());

        plotUtils.printHelix(p1,p2,v2, file, recTrack->charge());
      }

      if(im+1 != meas.end())
      {
        GlobalPoint  p1 = (*(im  )).updatedState().globalPosition();
        GlobalPoint  p2 = (*(im+1)).updatedState().globalPosition();
        GlobalVector v2 = (*(im+1)).updatedState().globalDirection();

        plotUtils.printHelix(p1,p2,v2, file, recTrack->charge());
      }
    }
  }

  file << "}]";

  if(0 && hasSimInfo)
    delete theHitAssociator;
}
