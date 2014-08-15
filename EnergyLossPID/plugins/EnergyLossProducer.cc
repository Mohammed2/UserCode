#include "EnergyLossProducer.h"

#include "FWCore/ParameterSet/interface/FileInPath.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/DeDxData.h"

#include "TrackingTools/PatternTools/interface/Trajectory.h"

#include "../interface/DataHandler.h"
#include "../interface/HadronClusters.h"
#include "../interface/TTrack.h"

using namespace std;

typedef edm::ValueMap<vector<float> > DeDxProDValueMap;

#define DebugOutput

/*****************************************************************************/
EnergyLossProducer::EnergyLossProducer(const edm::ParameterSet& ps) : pset(ps)
{
  cerr << "\033[22;31m" << "Getting parameters.."
       << "\033[22;0m"  << endl;

  trackProducer      = ps.getParameter<string>("trackProducer");
  trajectoryProducer = ps.getParameter<string>("trajectoryProducer");
  tag                = ps.getParameter<string>("tag");

  cerr << "\033[22;31m" << "Setting up products.."
       << "\033[22;0m"  << endl;
  produces<reco::DeDxDataValueMap>("energyLossPixHits");
  produces<reco::DeDxDataValueMap>("energyLossStrHits");
  produces<reco::DeDxDataValueMap>("energyLossAllHits");

/*
  produces<      DeDxProDValueMap>("energyLossAllHits:probability");
  produces<      DeDxProDValueMap>("energyLossAllHits:deviation");
*/
}

/*****************************************************************************/
EnergyLossProducer::~EnergyLossProducer()
{
}

/*****************************************************************************/
void EnergyLossProducer::beginJob()
{
#ifdef DebugOutput
  file.open("../out/result.dat");
#endif
}

/*****************************************************************************/
void EnergyLossProducer::endJob()
{
#ifdef DebugOutput
  file.close();
#endif
}

/*****************************************************************************/
void EnergyLossProducer::readFitResults()
{
  char fileName[256];
  sprintf(fileName,"UserCode/EnergyLossPID/data/mostprob_%s.dat", tag.c_str());

  edm::FileInPath fileInPath(fileName);
  ifstream file(fileInPath.fullPath().c_str());

  cerr << " from " << fileName << endl;

  while(!file.eof())
  {
    int icha,ieta,ipt, d;
    float mue, p;

    file >> icha >> ieta >> ipt >> p;

    for(int k = 0; k < K ; k++)
    {
      pair<pair<int,int>,pair<int,int> > key(pair<int,int>(icha,ieta),
                                             pair<int,int>( ipt,   k));

      file >> mean[key] >> mue >> amp[key];
    }

    pair<pair<int,int>,int> key(pair<int,int>(icha,ieta), ipt);

    file >> scale[key] >> d >> d;
  }

  file.close();
}

/*****************************************************************************/
void EnergyLossProducer::beginRun(edm::Run & run, edm::EventSetup const & es)
{
  cerr << "\033[22;31m" << "Starting DataHandler (" << tag << ").."
       << "\033[22;0m"  << endl;

  theDataHandler = new DataHandler(tag);
  theDataHandler->applyGain = true;
  theDataHandler->beginJob();

  cerr << "\033[22;31m" << "Starting HadronClusters.."
       << "\033[22;0m"  << endl;
  theClusters = new HadronClusters(es, pset);

  // read fit results 
  cerr << "\033[22;31m" << "Reading fit amplitudes.."
       << "\033[22;0m"  << endl;
  readFitResults();
}

/*****************************************************************************/
pair<vector<float>, vector<float> > EnergyLossProducer::getProbabilities
   (const TTrack & track, const pair<double,double> & epsilon)
{
  pair<vector<float>,vector<float> > 
   res(vector<float>(3,0.), vector<float>(3,999.));

  if(track.eta > etaMin && track.eta < etaMax &&
     track.pt  >  ptMin && track.pt  <  ptMax)
  {  
    //
    int icha = (track.charge > 0 ? pos : neg);
    int ieta = int((track.eta - etaMin)/(etaMax - etaMin) * etaBins);
    int ipt  = int((track.pt  -  ptMin)/( ptMax -  ptMin) *  ptBins);

    //
    double    logde = log(epsilon.first);
    double siglogde = sqrt(epsilon.second) / epsilon.first;

    //
    pair<pair<int,int>,int> key(pair<int,int>(icha,ieta), ipt);

    double sig = scale[key] * siglogde;

    for(int k = 0; k < K; k++)
    {
      pair<pair<int,int>,pair<int,int> > key(pair<int,int>(icha,ieta),
                                             pair<int,int>( ipt,   k));
  
      double q = (logde - mean[key]) / sig;
  
      if(fabs(q) < 10) // FIXME
      {
        res.first[k]  = amp[key] * exp(-0.5*q*q); // division by sig not needed
        res.second[k] = q;
      }
    }

    double s = 0.;
      for(int k = 0; k < K; k++) s += res.first[k];

    if(s > 0.)
      for(int k = 0; k < K; k++) res.first[k] /= s;
  }

  return res;
}

/*****************************************************************************/
void EnergyLossProducer::produce(edm::Event& ev, const edm::EventSetup& es)
{
  // Get track collection
  edm::Handle<reco::TrackCollection> trackHandle;
  ev.getByLabel(trackProducer,       trackHandle);

  const vector<reco::Track> & trackCollection =
                                 *(trackHandle.product());

  auto_ptr<reco::DeDxDataValueMap> outputPix(new reco::DeDxDataValueMap);
  auto_ptr<reco::DeDxDataValueMap> outputStr(new reco::DeDxDataValueMap);
  auto_ptr<reco::DeDxDataValueMap> outputAll(new reco::DeDxDataValueMap);
/*
  auto_ptr<      DeDxProDValueMap> outputPro(new       DeDxProDValueMap);
  auto_ptr<      DeDxProDValueMap> outputDev(new       DeDxProDValueMap);
*/

  reco::DeDxDataValueMap::Filler fillerPix(*outputPix);
  reco::DeDxDataValueMap::Filler fillerStr(*outputStr);
  reco::DeDxDataValueMap::Filler fillerAll(*outputAll);
/*
        DeDxProDValueMap::Filler fillerPro(*outputPro);
        DeDxProDValueMap::Filler fillerDev(*outputDev);
*/

  // Get trajectory collection
  edm::Handle<vector<Trajectory> > trajeHandle;
  ev.getByLabel(trajectoryProducer,     trajeHandle);
  const vector<Trajectory> & trajeCollection =
                           *(trajeHandle.product());

  vector<reco::DeDxData> estimatePix;
  vector<reco::DeDxData> estimateStr;
  vector<reco::DeDxData> estimateAll;
/*
  vector<vector<float> > estimatePro;
  vector<vector<float> > estimateDev;
*/

  // Take all trajectories
  int j = 0;
  for(vector<Trajectory>::const_iterator traje = trajeCollection.begin();
                                         traje!= trajeCollection.end();
                                         traje++, j++)
  {
    TTrack track;

    track.charge = trackCollection[j].charge();
    track.eta    = trackCollection[j].eta();
    track.pt     = trackCollection[j].pt();

    theClusters->analyzeRecTrajectory(*traje, track);

    pair<double,double> epsilon = 
      theDataHandler->processTrack(track, estimatePix,
                                          estimateStr,
                                          estimateAll);

    //
    pair<vector<float>, vector<float> > res = getProbabilities(track, epsilon);

    vector<float> pro = res.first;
    vector<float> dev = res.second;

#ifdef DebugOutput
    double p = trackCollection[j].pt() *
          cosh(trackCollection[j].eta());

    // DataFormats/TrackReco/src/DeDxData.cc always returns dEdxError() == -1
    file << " " << p 
         << " " << estimateAll.back().dEdx()                 // epsilon
         << " " << sqrt(epsilon.second)                      // sigma(epsilon)
         << " " << estimateAll.back().numberOfMeasurements() // nhits
         << " " << pro[0] // prob pion
         << " " << pro[1] // prob kaon
         << " " << pro[2] // prob prot (sum = 1)
         << " " << dev[0] // deviation pion
         << " " << dev[1] // deviation kaon
         << " " << dev[2] // deviation prot (in units of sigma)
         << endl; 
#endif

/*
    estimatePro.push_back(pro);
    estimateDev.push_back(dev);
*/
  }

  fillerPix.insert(trackHandle, estimatePix.begin(), estimatePix.end());
  fillerStr.insert(trackHandle, estimateStr.begin(), estimateStr.end());
  fillerAll.insert(trackHandle, estimateAll.begin(), estimateAll.end());
/*
  fillerPro.insert(trackHandle, estimatePro.begin(), estimatePro.end());
  fillerDev.insert(trackHandle, estimateDev.begin(), estimateDev.end());
*/

  fillerPix.fill();
  fillerStr.fill();
  fillerAll.fill();
/*
  fillerPro.fill();
  fillerDev.fill();
*/

  // Put back result to event
  ev.put(outputPix, "energyLossPixHits");
  ev.put(outputStr, "energyLossStrHits");
  ev.put(outputAll, "energyLossAllHits");
/*
  ev.put(outputPro, "energyLossAllHits:probability");
  ev.put(outputDev, "energyLossAllHits:deviation");
*/
}

