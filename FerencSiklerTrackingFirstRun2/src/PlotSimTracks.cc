#include "../interface/PlotSimTracks.h"
#include "../interface/PlotUtils.h"
#include "../interface/HitInfo.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"

#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"

#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"

#include "SimDataFormats/CrossingFrame/interface/CrossingFrame.h"
#include "SimDataFormats/CrossingFrame/interface/MixCollection.h"

using namespace std;

/*****************************************************************************/
PlotSimTracks::PlotSimTracks
  (const edm::EventSetup& es, ofstream& file_) : file(file_)
{
  // Get tracker geometry
  edm::ESHandle<TrackerGeometry> trackerHandle;
  es.get<TrackerDigiGeometryRecord>().get(trackerHandle);
  theTracker = trackerHandle.product();
}

/*****************************************************************************/
PlotSimTracks::~PlotSimTracks()
{
}

/*****************************************************************************/
void PlotSimTracks::printSimTracks(const edm::Event& ev)
{
  // Tracker
  edm::Handle<TrackingParticleCollection> simTrackHandle;
//  ev.getByLabel("mix", "MergedTrackTruth", simTrackHandle);
  ev.getByLabel("mix", "", simTrackHandle);
  const TrackingParticleCollection* simTracks = simTrackHandle.product();

  // Utilities
//  PlotUtils plotUtils;

  file << ", If[st, {RGBColor[0.5,0.5,0.5]";

  int ii = 1;

  for(TrackingParticleCollection::const_iterator simTrack = simTracks->begin();
                                                 simTrack!= simTracks->end();
                                                 simTrack++)
  {
  if(simTrack->charge() != 0)
  if(fabs(simTrack->eta()) < 2.8)
//  if(simTrack->parentVertex()->nSourceTracks() == 0) // isPrimary
  {
cerr << " SimTrack " << ii
       << " q=" << simTrack->charge()
     << " eta=" << simTrack->eta()
      << " pt=" << simTrack->pt()
         << " " << simTrack->parentVertex()->nSourceTracks()
          << endl;

    GlobalPoint p0(simTrack->vertex().x(),
                   simTrack->vertex().y(),
                   simTrack->vertex().z());

    double p = sqrt( simTrack->momentum().x() * simTrack->momentum().x()
                   + simTrack->momentum().y() * simTrack->momentum().y()
                   + simTrack->momentum().z() * simTrack->momentum().z() );

    GlobalVector v1(simTrack->momentum().x() / p,
                    simTrack->momentum().y() / p,
                    simTrack->momentum().z() / p);

    GlobalPoint p1(simTrack->vertex().x() + v1.x() * 5,
                   simTrack->vertex().y() + v1.y() * 5,
                   simTrack->vertex().z() + v1.z() * 5);

//    plotUtils.printHelix(p0,p1,v1, file, simTrack->charge());
    file << ", Line[{{"<< p0.x() <<","<< p0.y() <<",("<< p0.z() <<"-zs)*mz}, {"<< p1.x() <<","<< p1.y() <<",("<< p1.z() <<"-zs)*mz}}]" << endl;

    file << ", Point[{" << p1.x() << "," << p1.y() << ",(" << p1.z() << "-zs)*mz}]"
         << endl;

    file << ", Text[StyleForm[\"" << ii << "\", URL->\" SimTrack #" << ii
         << " q=" << simTrack->charge()
     << " eta=" << simTrack->eta()
      << " pt=" << simTrack->pt()
         << " " << simTrack->parentVertex()->nSourceTracks()
         << " | pdg=" << simTrack->pdgId()
         << "\"], {"
         << p1.x() << "," << p1.y() << ",(" << p1.z() << "-zs)*mz}, {1,1}]"
         << endl;
    
  }
    ii++; 
  }

  file << "}]";
}

