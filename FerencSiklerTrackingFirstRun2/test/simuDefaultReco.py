import FWCore.ParameterSet.Config as cms

process = cms.Process("DefaultReconstruction")

process.load("Configuration.StandardSequences.Services_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.load("SimGeneral.MixingModule.mixNoPU_cfi")
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')

###############################################################################
# Message logger
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
    suppressWarning = cms.untracked.vstring('siStripZeroSuppression')
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
   input = cms.untracked.int32(5000)
)

###############################################################################
# Produce udst
process.produceMicroDst = cms.EDAnalyzer("UDstProducer",
    hasSimInfo  = cms.bool(True),
    allVertices = cms.string('offlinePrimaryVertices'),
    allTracks   = cms.string('generalTracks')
)

###############################################################################
# Paths
from SimGeneral.MixingModule.digitizers_cfi import *
process.mix.digitizers = cms.PSet(theDigitizersValid)
process.mix.digitizers.mergedtruth.select.ptMinTP = cms.double(0.01)
process.mix.playback = cms.untracked.bool(True)

process.gsimu = cms.Path(process.mix)

process.raw2digi_step = cms.Path(process.RawToDigi)
process.reconstruction_step = cms.Path(process.reconstruction)

process.load("SimTracker.TrackAssociation.trackingParticleRecoTrackAsssociation_cfi")
process.tpRecoAssocGeneralTracks = process.trackingParticleRecoTrackAsssociation.clone()
process.tpRecoAssocGeneralTracks.label_tr = cms.InputTag("generalTracks")

process.tpRecoAssocGeneralTracks.label_tp = cms.InputTag("mix","")

process.load("SimTracker.TrackAssociation.quickTrackAssociatorByHits_cfi")

# Take reco as denominator
process.quickTrackAssociatorByHits.SimToRecoDenominator = cms.string('reco')

# shared hits /  rectrack hits
process.quickTrackAssociatorByHits.Cut_RecoToSim = cms.double(0.50) #was 0.75

# shared hits /  rectrack hits
process.quickTrackAssociatorByHits.Purity_SimToReco = cms.double(0.50) #was 0.75

process.quickTrackAssociatorByHits.associateStrip = cms.bool(False)

process.load("SimTracker.TrackerHitAssociation.clusterTpAssociationProducer_cfi")
process.tpClusterProducer.trackingParticleSrc = cms.InputTag('mix', '')

process.postp = cms.Path(process.tpClusterProducer
                       * process.tpRecoAssocGeneralTracks
                       * process.produceMicroDst)

###############################################################################
# Global tag
process.GlobalTag.globaltag = 'MCRUN2_73_V9::All'

###############################################################################
# Schedule
process.schedule = cms.Schedule(process.gsimu,
                                process.raw2digi_step,
                                process.reconstruction_step,
                                process.postp)
##
from SLHCUpgradeSimulations.Configuration.postLS1Customs import customisePostLS1
process = customisePostLS1(process)

