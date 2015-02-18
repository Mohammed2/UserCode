import FWCore.ParameterSet.Config as cms

process = cms.Process("Reconstruction")

process.load("Configuration.StandardSequences.Services_cff")
process.load("Configuration.StandardSequences.Mixing")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.Simulation_cff")
process.load("Configuration.StandardSequences.RawToDigi_cff")

process.load("RecoLocalTracker.Configuration.RecoLocalTracker_cff")
process.load("RecoLocalTracker.SiPixelRecHits.PixelCPEESProducers_cff")

# Tracker local reco
process.load("RecoLocalTracker.Configuration.RecoLocalTracker_cff")

# Beamspot
process.load("RecoVertex.BeamSpotProducer.BeamSpot_cfi")
process.load("RecoPixelVertexing.PixelLowPtUtilities.MinBiasTracking_cff")

process.load("RecoPixelVertexing.PixelLowPtUtilities.siPixelClusterShapeCache_cfi")

process.load("RecoTracker.MeasurementDet.MeasurementTrackerEventProducer_cfi")

process.load("FWCore.MessageLogger.MessageLogger_cfi")

###############################################################################
# Message logger
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
    destinations = cms.untracked.vstring('cerr')
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
    input = cms.untracked.int32(10000)
)

###############################################################################
# Agglomerative vertexing
import UserCode.FerencSiklerVertexing.NewVertexProducer_cfi

process.allVertices = UserCode.FerencSiklerVertexing.NewVertexProducer_cfi.newVertices.clone()
process.allVertices.TrackCollection = 'allTracks'
process.allVertices.PtMin = cms.double(0.1)

###############################################################################
# Produce udst
process.produceMicroDst = cms.EDAnalyzer("UDstProducer",
    hasSimInfo  = cms.bool(True),
    allVertices = cms.string('allVertices'),
    allTracks   = cms.string('allTracks')
)

###############################################################################
# Paths
process.lreco = cms.Path(process.siPixelRecHits
                       * process.siStripMatchedRecHits)

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

