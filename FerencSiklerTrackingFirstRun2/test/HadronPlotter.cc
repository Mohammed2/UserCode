#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "../interface/PlotRecHits.h"
#include "../interface/PlotRecTracks.h"
#include "../interface/PlotSimTracks.h"

#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "DataFormats/VZero/interface/VZero.h"
#include "DataFormats/VZero/interface/VZeroFwd.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include <fstream>
#include <iomanip>

using namespace std;

/*****************************************************************************/
class HadronPlotter : public edm::EDAnalyzer
{
  public:
    explicit HadronPlotter(const edm::ParameterSet& ps);
    ~HadronPlotter();
    virtual void beginJob();
    virtual void endJob();
    virtual void beginRun(edm::Run const & run,
                          edm::EventSetup const & es);
    virtual void analyze (const edm::Event& ev, const edm::EventSetup& es);

  private:
    void printVertices(const edm::Event& ev, ofstream& file);
    string trackProducer;

    const MagneticField* theMagField;

    bool hasSimInfo;
};

/*****************************************************************************/
HadronPlotter::HadronPlotter(const edm::ParameterSet& ps)
{
  hasSimInfo    = ps.getParameter<bool>("hasSimInfo");

  trackProducer = ps.getParameter<string>("trackProducer");
}

/*****************************************************************************/
HadronPlotter::~HadronPlotter()
{
}

/*****************************************************************************/
void HadronPlotter::beginJob()
{
}

/*****************************************************************************/
void HadronPlotter::endJob()
{
}

/*****************************************************************************/
void HadronPlotter::beginRun(edm::Run const & run,
                             edm::EventSetup const & es)
{
  // Get magnetic field
  edm::ESHandle<MagneticField> magField;
  es.get<IdealMagneticFieldRecord>().get(magField);
  theMagField = magField.product();
}

/*****************************************************************************/
void HadronPlotter::printVertices(const edm::Event& ev, ofstream& file)
{
  // Get vertices
  edm::Handle<reco::VertexCollection> vertexHandle;
  ev.getByLabel("allVertices",     vertexHandle);
  const reco::VertexCollection* vertices = vertexHandle.product();

  file << ", RGBColor[0,0.8,0], AbsolutePointSize[7]";

  edm::LogVerbatim("MinBiasTracking")
       << " [HadronPlotter] vertices : "
       << vertices->size();

  for(reco::VertexCollection::const_iterator vertex = vertices->begin();
                                             vertex!= vertices->end();
                                             vertex++)
  {
    file << ", If[rt, {Point[{" << vertex->position().x()
                 << "," << vertex->position().y()
                 << ",(" << vertex->position().z() << "-zs)*mz}]" << endl;
    file << ", Text[StyleForm[\"V\", URL->\"Vertex ("
         <<vertex->position().x()<<","
         <<vertex->position().y()<<","
         <<vertex->position().z()<<") cm | Tracks="
         << vertex->tracksSize() << "\"]"
         << ", {" << vertex->position().x()
           << "," << vertex->position().y()
           << ",(" << vertex->position().z() << "-zs)*mz}, {0,-1}]}]" << endl;
  }
}

/*****************************************************************************/
void HadronPlotter::analyze(const edm::Event& ev, const edm::EventSetup& es)
{
  edm::LogVerbatim("MinBiasTracking") << "[HadronPlotter]";

  ofstream file("event.m");
  file << fixed << setprecision(3);

  // start graphics 
  file << "Graphics3D[";

  // start physics
  file << "{";
  PlotRecHits theRecHits(es,file);
  theRecHits.printRecHits(ev);

  PlotRecTracks theRecTracks(es,trackProducer,hasSimInfo,file);
  theRecTracks.printRecTracks(ev);

  if(hasSimInfo)
  {
    PlotSimTracks theSimTracks(es,file);
    theSimTracks.printSimTracks(ev);
  }

  printVertices(ev,file);

  // region (tracker + ecal)
  int mx = 120; int my = 120; int mz = 300;

  // beam line
  file << ", RGBColor[0.7,0.7,0.7]";

  for(int z = -mz; z < mz; z += mz/30)
    file << ", Line[{{0,0,("<<z<<"-zs)*mz}, {0,0,("<<z+mz/30<<"-zs)*mz}}]" << endl;

  // box
  file << ", RGBColor[0,0,0]";
  for(int iz = -1; iz <= 1; iz+=2)
  {
    file << ", Line[{";
    file << "{"<<-mx<<","<<-my<<",("<<iz*mz<<"-zs)*mz}, ";
    file << "{"<< mx<<","<<-my<<",("<<iz*mz<<"-zs)*mz}, ";
    file << "{"<< mx<<","<< my<<",("<<iz*mz<<"-zs)*mz}, ";
    file << "{"<<-mx<<","<< my<<",("<<iz*mz<<"-zs)*mz}, ";
    file << "{"<<-mx<<","<<-my<<",("<<iz*mz<<"-zs)*mz}";
    file << "}]";
  }

  for(int ix = -1; ix <= 1; ix+=2)
  for(int iy = -1; iy <= 1; iy+=2)
  {
    file << ", Line[{{"<<ix*mx<<","<<iy*my<<",("<<-mz<<"-zs)*mz},";
    file <<         "{"<<ix*mx<<","<<iy*my<<",("<< mz<<"-zs)*mz}}]";
  }

  // stop physics
  file << "}";

  // options
  file << ", PlotRange->{{"<<-mx<<","<<mx<<"}, {"
                           <<-my<<","<<my<<"}, {"
                           <<-mz<<"*z,"<<mz<<"*z}}";
  file << ", PlotLabel->\"" << "run:"    << ev.id().run()
                            << " event:" << ev.id().event() << "\"";
  file << ", Boxed->False, AxesLabel->{x,y,z}";
  file << "]";

  file.close();

  system("zip -q -m event.zip event.m ; mv event.zip ../data");

  edm::LogVerbatim("MinBiasTracking") << "[HadronPlotter] event plotted";
  cerr << "[HadronPlotter] event plotted" << endl;

  while(getchar() == 0);
} 

DEFINE_FWK_MODULE(HadronPlotter);
