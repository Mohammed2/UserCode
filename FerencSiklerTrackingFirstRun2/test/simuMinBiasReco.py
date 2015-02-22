import FWCore.ParameterSet.Config as cms

process = cms.Process("MinBiasReconstruction")

process.load("Configuration.StandardSequences.Services_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load("Configuration.StandardSequences.GeometryDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.RawToDigi_cff")
process.load("Configuration.StandardSequences.Digi_cff")

###############################################################################
# Message logger
process.load("FWCore.MessageLogger.MessageLogger_cfi")

process.MessageLogger = cms.Service("MessageLogger",
    categories = cms.untracked.vstring(
      'pixel3Vertices', 'NewVertices', 'UDstProducer'
    ),
    debugModules = cms.untracked.vstring('*'),
    cerr = cms.untracked.PSet(
        threshold = cms.untracked.string('DEBUG'),
        DEBUG = cms.untracked.PSet(
            limit = cms.untracked.int32(0)
        ),
        FwkReport = cms.untracked.PSet(
            optionalPSet = cms.untracked.bool(True),
            reportEvery = cms.untracked.int32(10),
            limit = cms.untracked.int32(10000000)
        )
    ),
    destinations = cms.untracked.vstring('cerr'),
    suppressWarning = cms.untracked.vstring('siStripZeroSuppression'),
    suppressError   = cms.untracked.vstring('globalPrimTracks','globalSecoTracks','globalTertTracks')
)

###############################################################################
# Source
process.source = cms.Source("PoolSource",
    duplicateCheckMode = cms.untracked.string('noDuplicateCheck'),
    skipEvents = cms.untracked.uint32(0),
    fileNames = cms.untracked.vstring(
       'file:///tmp/sikler/820E70BE-F39D-E411-80BA-0025905A6138.root',
       'file:///tmp/sikler/8E5AC8DC-FC9D-E411-AE20-003048FFD770.root',
       'file:///tmp/sikler/9805E1CF-F59D-E411-B3F1-003048FFCB9E.root',
    ),
    inputCommands = cms.untracked.vstring(
       'keep *',
       'drop *_mix_MergedTrackTruth_HLT'
    )
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
    hasSimInfo  = cms.bool(True),
    allVertices = cms.string('allVertices'),
    allTracks   = cms.string('allTracks')
)

###############################################################################
# Paths

# Mixing
process.load("SimGeneral.MixingModule.mixNoPU_cfi")
from SimGeneral.MixingModule.digitizers_cfi import *
process.mix.digitizers = cms.PSet(theDigitizersValid)
process.mix.digitizers.mergedtruth.select.ptMinTP = cms.double(0.01)
process.mix.playback = cms.untracked.bool(True)

# Runge-Kutta
from TrackingTools.TrackFitters.RungeKuttaFitters_cff import *
KFFittingSmootherWithOutliersRejectionAndRK.EstimateCut = cms.double(50.)
KFFittingSmootherWithOutliersRejectionAndRK.LogPixelProbabilityCut = cms.double(-16.)

process.gsimu = cms.Path(process.mix)
process.ldigi = cms.Path(process.RawToDigi)

# Local reco
process.load("RecoLocalTracker.Configuration.RecoLocalTracker_cff")
process.lreco = cms.Path(process.trackerlocalreco)

# Minimum bias tracking and related
process.load("RecoVertex.BeamSpotProducer.BeamSpot_cfi")
process.load("RecoLocalTracker.SiPixelClusterizer.SiPixelClusterizer_cfi")
process.load("RecoLocalTracker.SiPixelRecHits.SiPixelRecHits_cfi")
process.load("RecoTracker.MeasurementDet.MeasurementTrackerEventProducer_cfi")
process.load("RecoPixelVertexing.PixelLowPtUtilities.siPixelClusterShapeCache_cfi")
process.load("RecoPixelVertexing.PixelLowPtUtilities.MinBiasTracking_cff")

# Provide new shape files (all hits for pixels, only clear hits for strips)
from RecoPixelVertexing.PixelLowPtUtilities.ClusterShapeHitFilterESProducer_cfi import *
ClusterShapeHitFilterESProducer.PixelShapeFile = cms.string('RecoPixelVertexing/PixelLowPtUtilities/data/pixelShape_simHiteq1.par')
ClusterShapeHitFilterESProducer.StripShapeFile = cms.string('RecoPixelVertexing/PixelLowPtUtilities/data/stripShape_simHiteq1.par')

# Global reco
process.greco = cms.Path(process.offlineBeamSpot
                       * process.siPixelClusters
                       * process.siPixelRecHits
                       * process.MeasurementTrackerEvent
                       * process.siPixelClusterShapeCache
                       * process.minBiasTracking
                       * process.allVertices)

# Postprocessing, association
process.load("SimTracker.TrackerHitAssociation.clusterTpAssociationProducer_cfi")
process.tpClusterProducer.trackingParticleSrc = cms.InputTag('mix', '')

process.load("SimTracker.TrackAssociation.trackingParticleRecoTrackAsssociation_cfi")
process.tpRecoAssocGeneralTracks = process.trackingParticleRecoTrackAsssociation.clone()
process.tpRecoAssocGeneralTracks.label_tr = cms.InputTag("allTracks")
process.tpRecoAssocGeneralTracks.label_tp = cms.InputTag("mix","")

process.load("SimTracker.TrackAssociation.quickTrackAssociatorByHits_cfi")
process.quickTrackAssociatorByHits.SimToRecoDenominator = cms.string('reco')
process.quickTrackAssociatorByHits.Purity_SimToReco = cms.double(0.50) #was 0.75
process.quickTrackAssociatorByHits.Cut_RecoToSim    = cms.double(0.50) #was 0.75
process.quickTrackAssociatorByHits.ThreeHitTracksAreSpecial = cms.bool(False)

process.postp = cms.Path(process.tpClusterProducer
                       * process.tpRecoAssocGeneralTracks
                       * process.produceMicroDst)

###############################################################################
# Global tag
process.GlobalTag.globaltag = 'MCRUN2_73_V9::All'

###############################################################################
# Schedule
process.schedule = cms.Schedule(process.gsimu,
                                process.ldigi,
                                process.lreco,
                                process.greco,
                                process.postp)
