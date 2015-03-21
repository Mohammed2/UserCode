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

// Track refit
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"

#include "RecoVertex/KalmanVertexFit/interface/SingleTrackVertexConstraint.h"

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

#undef Debug

#define Sqr(x) ((x) * (x))

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
  bool isPrimary(const edm::RefToBase<Track> & recTrack);

  //
  float refitPtWithVertex(const Track & recTrack, const Vertex & vertex);
  const TransientTrackBuilder * theTTBuilder;

  //
  map<size_t, TTrack::aTrack_t> recTrackMap; 
  map<TrackingParticleRef::key_type, TTrack::aTrack_t> simTrackMap;

  // Root
  TFile * file;
  TTree * tree;
  TBunchCrossing * bunchCrossing;

  const BeamSpot * theBeamSpot;

  // Processes
  void processTrigger      (const edm::Event& ev);
  void processHadronForward(const edm::Event& ev);
  void processEventRelated (const edm::Event& ev);

  void processSimTracks(const edm::Event& ev, bool associate);
  void processRecTracks(const edm::Event& ev, bool associate);
  void processVertices (const edm::Event& ev);

  // Parameters
  bool hasSimInfo;
  string allVertices;
  string allTracks;
};

typedef vector<pair<edm::RefToBase<Track>, double> > assocRecTracks_t;
typedef vector<pair<TrackingParticleRef,   double> > assocSimTracks_t;

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
  file->Write();
  file->Close();
}

/*****************************************************************************/
void UDstProducer::beginRun(edm::Run const & r, edm::EventSetup const & es)
{
  // Get transient track builder
  edm::ESHandle<TransientTrackBuilder> builder;
  es.get<TransientTrackRecord>().get("TransientTrackBuilder", builder);
  theTTBuilder = builder.product();
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
  double dt = fabs(recTrack.dxy(theBeamSpot->position()));

  double st = recTrack.dxyError();

  // Relative longitudinal impact parameter
  double dz = fabs( (recTrack.dz(theBeamSpot->position())
                               + theBeamSpot->position().z()
                                     - vertex.position().z()) );

  double sz = sqrt(Sqr(recTrack.dzError()) + Sqr(vertex.zError()));

  return (dt < 0.3 && dz < 0.6 &&   // cm
          dt/st < 3 && dz/sz < 3);  // in sigma
}

/*****************************************************************************/
bool UDstProducer::isPrimary(const edm::RefToBase<Track> & recTrack)
{
  if(recTrackMap.count(recTrack.key()))
  {
    int iv = recTrackMap[recTrack.key()].first;
    int it = recTrackMap[recTrack.key()].second;

    TTrack & r = bunchCrossing->recVertices[iv].tracks[it];

    return r.isPrimary;
  }
  else return false;
}

/*****************************************************************************/
float UDstProducer::refitPtWithVertex
  (const Track & recTrack, const Vertex & vertex)
{
  TransientTrack theTransientTrack = theTTBuilder->build(recTrack);

  // Get vertex position and error matrix
  GlobalPoint vertexPosition(vertex.position().x(),
                             vertex.position().y(),
                             vertex.position().z());

  GlobalError vertexError(Sqr(theBeamSpot->BeamWidthX()), 0,
                          Sqr(theBeamSpot->BeamWidthY()), 0,
                          0,vertex.covariance(2,2));

  // Refit track with vertex constraint
  SingleTrackVertexConstraint stvc;
  SingleTrackVertexConstraint::BTFtuple result =
    stvc.constrain(theTransientTrack, vertexPosition, vertexError);

  return result.get<1>().impactPointTSCP().pt();
}

/*****************************************************************************/
void UDstProducer::processSimTracks(const edm::Event& ev, bool associate)
{
  int proc = 0;
  SimToRecoCollection simToRec;

  if(!associate)
  {
    // We have only a single signal process
    edm::Handle<edm::HepMCProduct> hepEv;
    ev.getByLabel("generator", hepEv);
    proc = hepEv->GetEvent()->signal_process_id();

    bunchCrossing->processId = proc;
    LogTrace("UDstProducer") << " [UDstProducer] processId = " << proc;
  }
  else
  {
    // Get association
    edm::Handle<SimToRecoCollection> simToRecCollectionH;
    ev.getByLabel("tpRecoAssocGeneralTracks",simToRecCollectionH);
    simToRec = *(simToRecCollectionH.product());
  }

  edm::Handle<TrackingVertexCollection>  simVertexCollection;
  ev.getByLabel("mix","",simVertexCollection);
  const TrackingVertexCollection * vertices =
                                         simVertexCollection.product();

  // Pick all vertices, vertex without sources
  for(TrackingVertexCollection::const_iterator vertex = vertices->begin();
                                               vertex!= vertices->end();
                                               vertex++)
  if(isRelevantVertex(*vertex))
  {
    TVertex simVertex;

    if(!associate)
    {
      if(vertex == vertices->begin()) simVertex.processId = proc;
                                 else simVertex.processId = 0;

      simVertex.z = vertex->position().z();
    }

    // Go through simtracks
    for(TrackingVertex::tp_iterator simTrack = vertex->daughterTracks_begin();
                                    simTrack!= vertex->daughterTracks_end();
                                    simTrack++)
    if(!associate)
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

      // Remember index
      simTrackMap[simTrack->key()] =
        TTrack::aTrack_t(bunchCrossing->simVertices.size(),       // last
                                        simVertex.tracks.size()); // last

      // Store track 
      simVertex.tracks.push_back(s);
    }
    else
    {
      // Associate to recTracks
      if(simTrackMap.count(simTrack->key()) > 0)
      {
        int iv = simTrackMap[simTrack->key()].first;
        int it = simTrackMap[simTrack->key()].second;
        TTrack & s = bunchCrossing->simVertices[iv].tracks[it];

        if(simToRec.find(*simTrack) != simToRec.end())
        {
          assocRecTracks_t recTracks_ = (assocRecTracks_t) simToRec[*simTrack];
  
          for(assocRecTracks_t::const_iterator recTrack_ = recTracks_.begin();
                                               recTrack_!= recTracks_.end();
                                               recTrack_++)
            if(isPrimary(recTrack_->first))
              s.aTracks.push_back(recTrackMap[recTrack_->first.key()]);
        }
      }

#ifdef Debug
      if(s.isPrimary && s.charge() != 0 && fabs(s.eta) < 2.8)
      {
        if(s.aTracks.size() == 0)
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

        if(s.aTracks.size() >= 2)
          cerr << "\033[22;34m" << " SimTrack "
               << int(simTrack - vertex->daughterTracks_begin()) << " multipl |"
               << " q=" << (*simTrack)->charge()
               << " eta=" << (*simTrack)->eta() << " pt=" << (*simTrack)->pt()
               << "\033[22;0m" << endl;
      }
#endif
    }

    if(!associate)
    {
      // Store vertex
      bunchCrossing->simVertices.push_back(simVertex);
    }
  }
}

/*****************************************************************************/
void UDstProducer::processRecTracks(const edm::Event& ev, bool associate)
{
  RecoToSimCollection recToSim;

  if(associate)
  {
    // Get association
    edm::Handle<RecoToSimCollection> recToSimCollectionH;
    ev.getByLabel("tpRecoAssocGeneralTracks",recToSimCollectionH);
    recToSim = *(recToSimCollectionH.product());
  }
#ifdef Debug
  else
  {
    // For printing fakes
    edm::Handle<reco::TrackCollection>       recTrackHandle;
    ev.getByLabel(allTracks,                 recTrackHandle);
    const reco::TrackCollection* recTracks = recTrackHandle.product();

    for(reco::TrackCollection::const_iterator recTrack = recTracks->begin();
                                              recTrack!= recTracks->end();
                                              recTrack++)
      cerr << "\033[22;35m" << " RecTrack "
           << int(recTrack - recTracks->begin())
           << " q=" << (*recTrack).charge()
           << " eta=" << (*recTrack).eta() << " pt=" << (*recTrack).pt()
           << "\033[22;0m" << endl;
   
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
    //
  }
#endif

  edm::Handle<VertexCollection> recVertexCollection;
  ev.getByLabel(allVertices,    recVertexCollection);
  const VertexCollection * recVertices =
                                recVertexCollection.product();

  if(!associate)
  LogTrace("UDstProducer")
      << " [UDstProducer] recVertices = " << recVertices->size();

  for(VertexCollection::const_iterator vertex = recVertices->begin();
                                       vertex!= recVertices->end();
                                       vertex++)
  {
    TVertex recVertex;

    if(!associate)
    {
      recVertex.z = vertex->position().z();
    }

    // Go through rectracks
    for(Vertex::trackRef_iterator recTrack = vertex->tracks_begin();
                                  recTrack!= vertex->tracks_end();
                                  recTrack++)
    if(!associate)
    {
      TTrack r;

      r.isPrimary  = isPrimary(**recTrack, *vertex);

      if(r.isPrimary) // FIXME
      {

      switch((*recTrack)->algo())
      {
        case TrackBase::lowPtTripletStep    : r.algo = 1; break;
        case TrackBase::detachedTripletStep : r.algo = 2; break;
        case TrackBase::pixelPairStep       : r.algo = 3; break;
        default: r.algo = 0;
      }

      r.charge = (*recTrack)->charge();
      r.eta    = (*recTrack)->eta();

//    r.pt     = (*recTrack)->pt();
      r.pt = refitPtWithVertex(**recTrack, *vertex);

      r.phi    = (*recTrack)->phi();

      r.z      = (*recTrack)->dz (theBeamSpot->position());
      r.d0     = (*recTrack)->dxy(theBeamSpot->position());

      r.chi2   = (*recTrack)->chi2();
      r.ndf    = (*recTrack)->ndof();

      r.nTrackerHits = (*recTrack)->numberOfValidHits();
      
      // Remember index
      recTrackMap[(*recTrack).key()] =
         TTrack::aTrack_t(bunchCrossing->recVertices.size(),       // last
                                         recVertex.tracks.size()); // last

      // Store track 
      recVertex.tracks.push_back(r);
      }
    }
    else
    {
      if(recTrackMap.count(recTrack->key()) > 0)
      {
        int iv = recTrackMap[recTrack->key()].first;
        int it = recTrackMap[recTrack->key()].second;
        TTrack & r = bunchCrossing->recVertices[iv].tracks[it];

        // Associate to simTracks
        if(recToSim.find(*recTrack) != recToSim.end())
        {
          assocSimTracks_t simTracks_ = (assocSimTracks_t) recToSim[*recTrack];
  
          for(assocSimTracks_t::const_iterator simTrack_ = simTracks_.begin();
                                               simTrack_!= simTracks_.end();
                                               simTrack_++)
            if(simTrackMap.count(simTrack_->first.key()) > 0) // FIXME
              r.aTracks.push_back(simTrackMap[simTrack_->first.key()]);
        }
      }

#ifdef Debug
      if(r.aTracks.size() == 0)
        cerr << "\033[22;31m" << " RecTrack "
             << int(recTrack - vertex->tracks_begin()) << " fake"
             << " q=" << r.charge
             << " eta=" << r.eta << " pt=" << r.pt
             << "\033[22;0m" << endl;
#endif
    }

    if(!associate)
    {
      // Store vertex
      bunchCrossing->recVertices.push_back(recVertex);
    }
  }
}

/*****************************************************************************/
void UDstProducer::processVertices(const edm::Event& ev)
{
  // Process recvertices, recTracks
//  if(bunchCrossing->hfTowers.first  > 0 && FIXME
//     bunchCrossing->hfTowers.second > 0) 
  if(hasSimInfo)
  {
  simTrackMap.clear();
  recTrackMap.clear();
  processSimTracks(ev, false);
  }

  processRecTracks(ev, false);

  if(hasSimInfo)
  {
  processSimTracks(ev, true); // add associations
  processRecTracks(ev, true); // add associations
  }
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
  // Analyze FIXME
  processTrigger(ev);
  processHadronForward(ev);
  processEventRelated(ev);
  processVertices(ev);

   // Fill tree
  tree->Fill();
  bunchCrossing->Clear();
}

DEFINE_FWK_MODULE(UDstProducer);
