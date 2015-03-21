import FWCore.ParameterSet.Config as cms

process = cms.Process("DataReconstruction")

process.load("Configuration.StandardSequences.Services_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load("Configuration.StandardSequences.GeometryDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")

###############################################################################
# Message logger
process.load("FWCore.MessageLogger.MessageLogger_cfi")

process.MessageLogger = cms.Service("MessageLogger",
    categories = cms.untracked.vstring(
      'MinBiasTracking', 'pixel3Vertices', 'NewVertices'
    ),
    debugModules = cms.untracked.vstring('*'),
    cerr = cms.untracked.PSet(
        threshold = cms.untracked.string('DEBUG'),
        DEBUG = cms.untracked.PSet(
            limit = cms.untracked.int32(1000000)
        )
    ),
    destinations = cms.untracked.vstring('cerr'),
    suppressError   = cms.untracked.vstring('globalPrimTracks','globalSecoTracks','globalTertTracks')
)

 
###############################################################################
# Source
process.source = cms.Source("PoolSource",
    duplicateCheckMode = cms.untracked.string('noDuplicateCheck'),
    skipEvents = cms.untracked.uint32(0),
    fileNames = cms.untracked.vstring(
      'file:///tmp/sikler/B232391B-0B9E-E411-A3AC-0025905A6136.root'
    )
  # fileNames = '/store/relval/CMSSW_7_4_0_pre5/RelValMinBias_13/GEN-SIM-RECO/MCRUN2_73_V9_postLS1beamspot-v1/00000/B232391B-0B9E-E411-A3AC-0025905A6136.root'
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100)
)

###############################################################################
# Agglomerative vertexing
import UserCode.FerencSiklerVertexing.NewVertexProducer_cfi
process.allVertices = UserCode.FerencSiklerVertexing.NewVertexProducer_cfi.newVertices.clone()
process.allVertices.TrackCollection = 'allTracks'

###############################################################################
# Produce udst
process.produceMicroDst = cms.EDAnalyzer("UDstProducer",
    hasSimInfo  = cms.bool(False),
    allVertices = cms.string('allVertices'),
    allTracks   = cms.string('allTracks')
)

###############################################################################
# Paths

process.load("RecoLocalTracker.Configuration.RecoLocalTracker_cff")

process.lreco = cms.Path(process.siPixelRecHits
                       * process.siStripMatchedRecHits)

# Minimum bias tracking and related
process.load("RecoVertex.BeamSpotProducer.BeamSpot_cfi")
process.load("RecoLocalTracker.SiPixelClusterizer.SiPixelClusterizer_cfi")
process.load("RecoLocalTracker.SiPixelRecHits.SiPixelRecHits_cfi")
process.load("RecoTracker.MeasurementDet.MeasurementTrackerEventProducer_cfi")
process.load("RecoPixelVertexing.PixelLowPtUtilities.siPixelClusterShapeCache_cfi")
process.load("RecoPixelVertexing.PixelLowPtUtilities.MinBiasTracking_cff")

# Runge-Kutta
from TrackingTools.TrackFitters.RungeKuttaFitters_cff import *
KFFittingSmootherWithOutliersRejectionAndRK.EstimateCut = cms.double(50.)
KFFittingSmootherWithOutliersRejectionAndRK.LogPixelProbabilityCut = cms.double(-16.)

# Provide new shape files (all hits for pixels, only clear hits for strips)
from RecoPixelVertexing.PixelLowPtUtilities.ClusterShapeHitFilterESProducer_cfi import *
ClusterShapeHitFilterESProducer.PixelShapeFile = cms.string('RecoPixelVertexing/PixelLowPtUtilities/data/pixelShape_simHiteq1.par')
ClusterShapeHitFilterESProducer.StripShapeFile = cms.string('RecoPixelVertexing/PixelLowPtUtilities/data/stripShape_simHiteq1.par')

process.greco = cms.Path(process.siPixelClusterShapeCache
                       * process.MeasurementTrackerEvent
                       * process.minBiasTracking
                       * process.allVertices)

process.postp = cms.Path(process.produceMicroDst)

###############################################################################
# Global tag
process.GlobalTag.globaltag = 'MCRUN2_73_V9::All'

###############################################################################
# Schedule
process.schedule = cms.Schedule(process.lreco,
                                process.greco,
                                process.postp)

