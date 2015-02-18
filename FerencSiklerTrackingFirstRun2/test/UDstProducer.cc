#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Common/interface/TriggerNames.h"

//
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertexContainer.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"

//
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

//
#include "SimTracker/Records/interface/TrackAssociatorRecord.h"
#include "SimTracker/TrackAssociation/interface/TrackAssociatorByHits.h"

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
class UDstProducer : public edm::EDAnalyzer
{
 public:
  explicit UDstProducer(const edm::ParameterSet& pset);
  ~UDstProducer();
  virtual void beginJob();
  virtual void endJob();
  virtual void beginRun(edm::Run const & r, edm::EventSetup const & es);
  virtual void analyze(const edm::Event& ev, const edm::EventSetup& es);

 private:
  bool isRelevantVertex(const TrackingVertex & vertex);
  bool isPrimary(const TrackingParticleRef & simTrack);
  bool isPrimary(const Track & recTrack, const Vertex & vertex);

  bool isAmongGoodTracks(const edm::RefToBase<Track> & recTrack);

  //
  map<size_t, int> recTrackMap; 

  // Root
  TFile * file;
  TTree * tree;
  TBunchCrossing * bunchCrossing;

  const BeamSpot * theBeamSpot;

  // Processes
  void processTrigger      (const edm::Event& ev);
  void processHadronForward(const edm::Event& ev);
  void processEventRelated (const edm::Event& ev);

  void processSimTracks(const edm::Event& ev);
  void processRecTracks(const edm::Event& ev);
  void processVertices (const edm::Event& ev);

  // Parameters
  bool hasSimInfo;
  string allVertices;
  string allTracks;
};

typedef vector<pair<edm::RefToBase<Track>, double> > assocTracks_t;

/*****************************************************************************/
UDstProducer::UDstProducer(const edm::ParameterSet & pset)
{
  hasSimInfo  = pset.getParameter<bool>("hasSimInfo");

  allVertices = pset.getParameter<string>("allVertices");
  allTracks   = pset.getParameter<string>("allTracks");
}

/*****************************************************************************/
UDstProducer::~UDstProducer()
{
}

/*****************************************************************************/
void UDstProducer::beginJob()
{
  // Root
  file = new TFile("hadronTree.root","recreate");
  tree = new TTree("hadronTree","HadronTree");
  bunchCrossing = new TBunchCrossing();
  tree->Branch("bunchCrossing", "TBunchCrossing", &bunchCrossing, 16000, 2);
}

/*****************************************************************************/
void UDstProducer::endJob()
{
//  tree->Print();
  file->Write();
  file->Close();
}

/*****************************************************************************/
void UDstProducer::beginRun(edm::Run const & r, edm::EventSetup const & es)
{
}

/*****************************************************************************/
bool UDstProducer::isRelevantVertex(const TrackingVertex & vertex)
{
  return (vertex.position().rho() < 5 &&
     fabs(vertex.position().z())  < 50);
}


/*****************************************************************************/
bool UDstProducer::isPrimary(const TrackingParticleRef & simTrack)
{
//  return (simTrack->status() >= 0); // FIXME
  bool isStable =
    (abs(simTrack->pdgId()) ==   11 || // electron
     abs(simTrack->pdgId()) ==   13 || // muon
     abs(simTrack->pdgId()) ==  211 || // pion
     abs(simTrack->pdgId()) ==  321 || // kaon
     abs(simTrack->pdgId()) == 2212);  // proton

  return (isStable && simTrack->parentVertex()->nSourceTracks() == 0);
}

/*****************************************************************************/
bool UDstProducer::isPrimary(const Track & recTrack, const Vertex & vertex)
{
  // Relative transverse impact parameter
  double dt = fabs(recTrack.dxy(theBeamSpot->position()) /
                   recTrack.dxyError());

  // Relative longitudinal impact parameter (5*sigma)
  double dz = fabs( (recTrack.dz(theBeamSpot->position())
                               + theBeamSpot->position().z()
                                     - vertex.position().z())) /
                  sqrt(recTrack.dzError()*recTrack.dzError() +
                          vertex.zError()*   vertex.zError());

cerr << "   dt=" << dt << " dz=" << dz << endl;

  return (dt < 3 && dz < 3);
}

/*****************************************************************************/
bool UDstProducer::isAmongGoodTracks(const edm::RefToBase<Track> & recTrack)
{
  if(recTrackMap.count(recTrack.key()) > 0)
  {
//    int iv = recTrackMap[recTrack.key()].first;
//    int it = recTrackMap[recTrack.key()].second;
    return true;
  }
  else return false;
}

/*****************************************************************************/
void UDstProducer::processSimTracks(const edm::Event& ev)
{
  // We have only a single signal process
  edm::Handle<edm::HepMCProduct> hepEv;
  ev.getByLabel("generator", hepEv);
  int proc = hepEv->GetEvent()->signal_process_id();

  bunchCrossing->processId = proc;
  LogTrace("UDstProducer") << " [UDstProducer] processId = " << proc;

  edm::Handle<TrackingVertexCollection>  simVertexCollection;
//  ev.getByLabel("mix","MergedTrackTruth",simVertexCollection);
  ev.getByLabel("mix","",simVertexCollection);
  const TrackingVertexCollection * vertices =
                                         simVertexCollection.product();

  // Get association
  edm::Handle<SimToRecoCollection> simToRecCollectionH;
  ev.getByLabel("tpRecoAssocGeneralTracks",simToRecCollectionH);
  SimToRecoCollection simToRec = *(simToRecCollectionH.product());

  // Pick all vertices, vertex without sources
  for(TrackingVertexCollection::const_iterator vertex = vertices->begin();
                                               vertex!= vertices->end();
                                               vertex++)
  if(isRelevantVertex(*vertex))
  {
    TVertex simVertex;
    simVertex.processId = proc;
    simVertex.z = vertex->position().z();

    // Go through simtracks
    for(TrackingVertex::tp_iterator simTrack = vertex->daughterTracks_begin();
                                    simTrack!= vertex->daughterTracks_end();
                                    simTrack++)
    {
      TTrack s;

      s.isPrimary = isPrimary(*simTrack);

      // sim
      s.pdgId    = (*simTrack)->pdgId();

      if((*simTrack)->parentVertex()->nSourceTracks() == 0)
        s.parentId = 0;
      else
        s.parentId =
          (*((*simTrack)->parentVertex()->sourceTracks_begin()))->pdgId();

      s.charge = (*simTrack)->charge();
      s.eta    = (*simTrack)->eta();
      s.pt     = (*simTrack)->pt();
      s.phi    = (*simTrack)->phi();

      if((*simTrack)->charge() != 0)
        s.nTrackerHits = (*simTrack)->numberOfTrackerLayers();

      // Associate to recTracks
      s.nAssoc = 0;

      if(simToRec.find(*simTrack) != simToRec.end())
      {
        assocTracks_t recTracks_ = (assocTracks_t) simToRec[*simTrack];

        for(assocTracks_t::const_iterator recTrack_ = recTracks_.begin();
                                          recTrack_!= recTracks_.end();
                                          recTrack_++)
          if(isAmongGoodTracks(recTrack_->first)) s.nAssoc += 1;
      }


      if((*simTrack)->charge() != 0 && fabs((*simTrack)->eta()) < 2.8)
      if(s.isPrimary)
      if(s.nAssoc == 0)
      {
      cerr << "\033[22;33m" << " SimTrack "
           << int(simTrack - vertex->daughterTracks_begin()) << " no reco |"
           << " q=" << (*simTrack)->charge()
           << " eta=" << (*simTrack)->eta() << " pt=" << (*simTrack)->pt()
           << " vtx=(" << (*simTrack)->vertex().x()
                << "," << (*simTrack)->vertex().y()
               << ","  << (*simTrack)->vertex().z() << ")"
           << " " << (*simTrack)->numberOfTrackerHits()
           << " " << (*simTrack)->numberOfTrackerLayers()
           << " " << (*simTrack)->longLived()
           << " " << (*simTrack)->status()
           << " " << (*simTrack)->pdgId()
           << "\033[22;0m" << endl;
      } 

      if(s.nAssoc >= 2)
      if(s.isPrimary)
      {
      cerr << "\033[22;34m" << " SimTrack "
           << int(simTrack - vertex->daughterTracks_begin()) << " multipl |"
           << " q=" << (*simTrack)->charge()
           << " eta=" << (*simTrack)->eta() << " pt=" << (*simTrack)->pt()
           << "\033[22;0m" << endl;
      }


      // Store track 
      simVertex.tracks.push_back(s);
    }

    // Store vertex
    bunchCrossing->simVertices.push_back(simVertex);
  }
}

/*****************************************************************************/
void UDstProducer::processRecTracks(const edm::Event& ev)
{
  recTrackMap.clear();

  // Get association
  edm::Handle<RecoToSimCollection> recToSimCollectionH;
  ev.getByLabel("tpRecoAssocGeneralTracks",recToSimCollectionH);
  RecoToSimCollection recToSim = *(recToSimCollectionH.product());

  // For printing fakes
  // rec
  {
  edm::Handle<reco::TrackCollection>       recTrackHandle;
  ev.getByLabel(allTracks,                 recTrackHandle);
  const reco::TrackCollection* recTracks = recTrackHandle.product();

  for(reco::TrackCollection::const_iterator recTrack = recTracks->begin();
                                            recTrack!= recTracks->end();
                                            recTrack++)
  {
      cerr << "\033[22;35m" << " RecTrack "
           << int(recTrack - recTracks->begin())
           << " q=" << (*recTrack).charge()
           << " eta=" << (*recTrack).eta() << " pt=" << (*recTrack).pt()
           << "\033[22;0m" << endl;
  } 
 
  edm::Handle<edm::View<Track> > recCollection;
  ev.getByLabel(allTracks,       recCollection);

  for(edm::View<Track>::size_type j=0;
             j < recCollection.product()->size(); ++j)
  {
    edm::RefToBase<Track> recTrack(recCollection, j);

    if(recToSim.find(recTrack) == recToSim.end())
      cerr << "\033[22;32m" << " RecTrack "
           << j << " fake"
           << " q=" << (*recTrack).charge()
           << " eta=" << (*recTrack).eta() << " pt=" << (*recTrack).pt()
           << "\033[22;0m" << endl;
  } 
  }
  //


  edm::Handle<VertexCollection> recVertexCollection;
  ev.getByLabel(allVertices,    recVertexCollection);
  const VertexCollection * recVertices =
                                recVertexCollection.product();

  LogTrace("UDstProducer")
      << " [UDstProducer] recVertices = " << recVertices->size();

  // Find reconstructed vertex with highest multiplicity
  if(recVertices->size() <= 2) // FIXME
  for(VertexCollection::const_iterator vertex = recVertices->begin();
                                       vertex!= recVertices->end();
                                       vertex++)
  {
    TVertex recVertex;
    recVertex.z = vertex->position().z();

cerr << "  vertex = " << int(vertex - recVertices->begin()) + 1 << endl;

    // Go through rectracks
    for(Vertex::trackRef_iterator recTrack = vertex->tracks_begin();
                                  recTrack!= vertex->tracks_end();
                                  recTrack++)
    {
      // track
      TTrack r;

      // rec
cerr << " eta=" << (*recTrack)->eta() << " pt=" << (*recTrack)->pt();

      r.isPrimary  = isPrimary(**recTrack, *vertex);

      switch((*recTrack)->algo())
      {
        case TrackBase::lowPtTripletStep    : r.algo = 1; break;
        case TrackBase::detachedTripletStep : r.algo = 2; break;
        case TrackBase::pixelPairStep       : r.algo = 3; break;
        default: r.algo = 0;
      }

      r.charge = (*recTrack)->charge();
      r.eta    = (*recTrack)->eta();
      r.pt     = (*recTrack)->pt();
      r.phi    = (*recTrack)->phi();

      r.z      = (*recTrack)->dz (theBeamSpot->position());
      r.d0     = (*recTrack)->dxy(theBeamSpot->position());

      r.chi2   = (*recTrack)->chi2();
      r.ndf    = (*recTrack)->ndof();

      r.nTrackerHits = (*recTrack)->numberOfValidHits();

      if(recToSim.find(*recTrack) != recToSim.end())
      {
        r.nAssoc = recToSim[*recTrack].size();
      }
      else
      {
        cerr << "\033[22;31m" << " RecTrack "
             << int(recTrack - vertex->tracks_begin()) << " fake"
             << " q=" << r.charge
             << " eta=" << r.eta << " pt=" << r.pt
             << "\033[22;0m" << endl;
        r.nAssoc = 0;
      }

//      if(r.algo != 3 || r.nTrackerHits >= 4) // FIXME
      {
        recTrackMap[(*recTrack).key()] = 1;

        // Store track 
        recVertex.tracks.push_back(r);
      }
    }

    // Store vertex
    bunchCrossing->recVertices.push_back(recVertex);
  }
}

/*****************************************************************************/
void UDstProducer::processVertices(const edm::Event& ev)
{
  recTrackMap.clear();

  // Process recvertices, recTracks 
//  if(bunchCrossing->hfTowers.first  > 0 &&
//     bunchCrossing->hfTowers.second > 0) 
    processRecTracks(ev);

  // Process simvertices, simtracks
  if(hasSimInfo)
    processSimTracks(ev);
}

/*****************************************************************************/
void UDstProducer::processTrigger(const edm::Event& ev)
{
  // HLT
  edm::Handle<edm::TriggerResults> triggerResults;
  ev.getByLabel("TriggerResults",  triggerResults);
  
  edm::TriggerNames triggerNames = ev.triggerNames(*triggerResults);
  
  if(triggerResults.product()->wasrun())
  if(triggerResults.product()->accept())
  for(unsigned int itrig = 0; itrig < triggerResults.product()->size(); ++itrig)
    if(triggerResults.product()->accept(itrig))
    {
      LogTrace("UDstProducer")
        << " [UDstProducer] trigger "
        << triggerNames.triggerName(itrig).c_str();

      bunchCrossing->triggerNames.push_back(triggerNames.triggerName(itrig));
    }
}

/*****************************************************************************/
void UDstProducer::processHadronForward(const edm::Event& ev)
{
  edm::Handle<CaloTowerCollection> towers;
  ev.getByLabel("towerMaker",      towers);

  short int posTowers = 0;
  short int negTowers = 0;

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

  LogTrace("UDstProducer")
      << " [UDstProducer] hfTowers :"
      << " -" << bunchCrossing->hfTowers.first
      << " +" << bunchCrossing->hfTowers.second;
}

/*****************************************************************************/
void UDstProducer::processEventRelated(const edm::Event& ev)
{
  bunchCrossing->runNumber   = ev.run();
  bunchCrossing->lumiSection = ev.luminosityBlock();
  bunchCrossing->bxNumber    = ev.bunchCrossing();

  // Beam spot
  edm::Handle<BeamSpot>            beamSpotHandle;
  ev.getByLabel("offlineBeamSpot", beamSpotHandle);
  theBeamSpot = beamSpotHandle.product();
}

/*****************************************************************************/
void UDstProducer::analyze
  (const edm::Event& ev, const edm::EventSetup& es)
{
  // Analyze
//  processTrigger(ev);
//  processHadronForward(ev); // FIXME
  processEventRelated(ev);
  processVertices(ev);

   // Fill tree
  tree->Fill();
  bunchCrossing->Clear();
}

DEFINE_FWK_MODULE(UDstProducer);
