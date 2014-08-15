#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

//
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

//
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "TrackingTools/PatternTools/interface/Trajectory.h"

//
#include "../interface/HadronClusters.h"

// HF
#include "DataFormats/CaloTowers/interface/CaloTowerCollection.h"
#include "DataFormats/CaloTowers/interface/CaloTower.h"
#include "DataFormats/HcalDetId/interface/HcalSubdetector.h"

// My classes
#include "../interface/TBunchCrossing.h"
#include "../interface/TVertex.h"
#include "../interface/TTrack.h"

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"

#include <fstream>
using namespace std;
using namespace reco;

/*****************************************************************************/
class MicroDstProducer : public edm::EDAnalyzer
{
 public:
   explicit MicroDstProducer(const edm::ParameterSet& pset);
   ~MicroDstProducer();
   virtual void beginJob();
   virtual void endJob();
   virtual void beginRun(edm::Run const & r, edm::EventSetup const & es);
   virtual void analyze(const edm::Event& ev, const edm::EventSetup& es);

 private:
   bool isPrimary(const Track & recTrack, const Vertex & vertex);

   // Root
   TFile * file;
   TTree * tree;
   TBunchCrossing * bunchCrossing;

   const TrackerGeometry * theTracker;
   const BeamSpot        * theBeamSpot;
   HadronClusters * theClusters;

   // Processes
   void processHadronForward (const edm::Event& ev);
   void processEventRelated  (const edm::Event& ev);
   void processVerticesTracks(const edm::EventSetup& es, const edm::Event& ev);

   // Parameters
   string allVertices;
   string allTracks;
   string allTrajectories;

   const edm::ParameterSet& pset;
};

/*****************************************************************************/
MicroDstProducer::MicroDstProducer(const edm::ParameterSet& pset_) : pset(pset_)
{
  allVertices     = pset.getParameter<string>("allVertices");
  allTracks       = pset.getParameter<string>("allTracks");
  allTrajectories = pset.getParameter<string>("allTrajectories");
}

/*****************************************************************************/
MicroDstProducer::~MicroDstProducer()
{
}

/*****************************************************************************/
void MicroDstProducer::beginJob()
{
  // Root
  file = new TFile("hadronTree.root","recreate");
  tree = new TTree("hadronTree","HadronTree");
  bunchCrossing = new TBunchCrossing();
  tree->Branch("bunchCrossing", "TBunchCrossing", &bunchCrossing, 16000, 2);
}

/*****************************************************************************/
void MicroDstProducer::endJob()
{
  file->Write();
  file->Close();
}

/*****************************************************************************/
void MicroDstProducer::beginRun(edm::Run const & r, edm::EventSetup const & es)
{
  // Get tracker geometry
  edm::ESHandle<TrackerGeometry> tracker;
  es.get<TrackerDigiGeometryRecord>().get(tracker);
  theTracker = tracker.product();

  // Clusters
  theClusters = new HadronClusters(es, pset);
}

/*****************************************************************************/
bool MicroDstProducer::isPrimary(const Track & recTrack, const Vertex & vertex)
{
  // Transverse impact paramter (0.2 cm or 5*sigma)
  double dt = fabs(recTrack.dxy(theBeamSpot->position()));

  if(dt > 0.2) return false;

  // Longitudinal impact parameter (5*sigma)
  double dz = fabs(recTrack.dz(theBeamSpot->position())
                             + theBeamSpot->position().z()
                   - vertex.position().z());

  if(dz > 0.2 * cosh(recTrack.eta())) return false;

  return true;
}

/*****************************************************************************/
void MicroDstProducer::processVerticesTracks
  (const edm::EventSetup& es, const edm::Event& ev)
{
  // rectrajs
  edm::Handle<vector<Trajectory> > trajeHandle;
  ev.getByLabel(allTrajectories,   trajeHandle);
  const vector<Trajectory> & trajeCollection = *(trajeHandle.product());

  reco::TrackBase::TrackQuality trackQualityHighPurity =
   reco::TrackBase::qualityByName("highPurity");

  LogDebug("microDstProducer")
    << " [MicroDstProducer] rectrajs = " << trajeCollection.size();

  // Process recvertices, recTracks 
  {
    edm::Handle<VertexCollection> recVertexCollection;
    ev.getByLabel(allVertices,    recVertexCollection);
    const VertexCollection * recVertices =
                             recVertexCollection.product();

    LogDebug("microDstProducer")
      << " [MicroDstProducer] recvertices = " << recVertices->size();

    // Take all vertices
    for(VertexCollection::const_iterator vertex = recVertices->begin();
                                         vertex!= recVertices->end();
                                         vertex++)
    {
      TVertex recVertex;
      recVertex.z = vertex->position().z();

      // Go through rectracks
      for(Vertex::trackRef_iterator recTrack = vertex->tracks_begin();
                                    recTrack!= vertex->tracks_end();
                                    recTrack++)
      {
        // track
        TTrack r;

        // rec
        r.isPrimary    = isPrimary(**recTrack, *vertex);
        r.isHighPurity = (*recTrack)->quality(trackQualityHighPurity); 

        r.charge = (*recTrack)->charge();

        r.eta    = (*recTrack)->eta();
        r.pt     = (*recTrack)->pt();
        r.phi    = (*recTrack)->phi();

        r.chi2   = (*recTrack)->chi2();
        r.ndf    = (*recTrack)->ndof();

        r.z      = (*recTrack)->dz (theBeamSpot->position());
        r.d0     = (*recTrack)->dxy(theBeamSpot->position());

        if((*recTrack).key() < trajeCollection.size()) 
        {
          const Trajectory & trajectory = trajeCollection[(*recTrack).key()];
          theClusters->analyzeRecTrajectory(trajectory, r);
        }

        switch((*recTrack)->algo())
        {
          case reco::TrackBase::iter1: r.algo = 1; break; // prim3
          case reco::TrackBase::iter2: r.algo = 2; break; // seco3
          case reco::TrackBase::iter3: r.algo = 3; break; // prim2
          default: r.algo = 0;
        } 

        // Store track
        recVertex.tracks.push_back(r);
      }

      // Store vertex 
      bunchCrossing->recVertices.push_back(recVertex);
    }
  }

  if(bunchCrossing->recVertices.size() > 0)
  LogDebug("microDstProducer")
    << " [MicroDstProducer] only "
    << bunchCrossing->recVertices.size() << " vertex with "
    << bunchCrossing->recVertices.front().tracks.size() << " tracks";
}

/*****************************************************************************/
void MicroDstProducer::processHadronForward(const edm::Event& ev)
{
  edm::Handle<CaloTowerCollection> towers;
  ev.getByLabel("towerMaker",      towers);

  short int negTowers = 0;
  short int posTowers = 0;

  for(CaloTowerCollection::const_iterator cal = towers->begin();
                                          cal!= towers->end(); ++cal)
  {
    for(unsigned int i = 0; i < cal->constituentsSize(); i++)
    {
      const DetId id = cal->constituent(i);
      if(id.det() == DetId::Hcal)
      {
        if((HcalSubdetector) id.subdetId() == HcalForward)
        if(cal->energy() > 3) // energy threshold of 3 GeV
        {
          if(cal->eta() <  0) negTowers++;
          if(cal->eta() >  0) posTowers++;
        }
      }
    }
  }

  bunchCrossing->hfTowers.first  = negTowers;
  bunchCrossing->hfTowers.second = posTowers;

  LogDebug("microDstProducer")
    << " [MicroDstProducer] hfTowers :"
       << " -" << bunchCrossing->hfTowers.first 
       << " +" << bunchCrossing->hfTowers.second;
}

/*****************************************************************************/
void MicroDstProducer::processEventRelated(const edm::Event& ev)
{
  bunchCrossing->runNumber   = ev.run();
  bunchCrossing->lumiSection = ev.luminosityBlock();
  bunchCrossing->bxNumber    = ev.bunchCrossing();

  // Beam spot
  edm::Handle<BeamSpot>      beamSpotHandle;

  ev.getByLabel("offlineBeamSpot", beamSpotHandle);
  theBeamSpot = beamSpotHandle.product();

  bunchCrossing->beamSpot_pos[0] =  theBeamSpot->position().x();
  bunchCrossing->beamSpot_pos[1] =  theBeamSpot->position().y();
  bunchCrossing->beamSpot_pos[2] =  theBeamSpot->position().z();

  bunchCrossing->beamSpot_sig[0] =  theBeamSpot->BeamWidthX();
  bunchCrossing->beamSpot_sig[1] =  theBeamSpot->BeamWidthY();
  bunchCrossing->beamSpot_sig[2] =  theBeamSpot->sigmaZ();

  LogDebug("microDstProducer")
    << fixed << setprecision(4)
    << " [MicroDstProducer] beamSpot at " << theBeamSpot->position();

  LogDebug("microDstProducer")
    << fixed << setprecision(4)
    << " [MicroDstProducer] beamSpot sigmaZ = " << theBeamSpot->sigmaZ()
                                    << " +- " << theBeamSpot->sigmaZ0Error()
                          << ", BeamWidth = " << theBeamSpot->BeamWidthX()
                                    << " +- " << theBeamSpot->BeamWidthXError();
  LogDebug("microDstProducer")
    << " [MicroDstProducer] beamSpot dxdz = " << theBeamSpot->dxdz()
                                << " dydz = " << theBeamSpot->dydz() << endl;
}

/*****************************************************************************/
void MicroDstProducer::analyze
  (const edm::Event& ev, const edm::EventSetup& es)
{
  LogDebug("microDstProducer") << "[MicroDstProducer]";

  LogDebug("microDstProducer")
    << " [MicroDstProducer] bunchCrossing number = " << ev.bunchCrossing();

  // Analyze
  processHadronForward (ev);
  processEventRelated  (ev);
  processVerticesTracks(es,ev);

  // Fill tree
  tree->Fill();
  bunchCrossing->Clear();
}

DEFINE_FWK_MODULE(MicroDstProducer);

