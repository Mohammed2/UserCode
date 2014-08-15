import FWCore.ParameterSet.Config as cms

# Tracker local reco
from RecoLocalTracker.Configuration.RecoLocalTracker_cff import *

# Refitter
from RecoTracker.TrackProducer.TrackRefitters_cff import *

# Track refitter
from RecoTracker.TrackProducer.TrackRefitter_cfi import *
refitterForEnergyLoss     = TrackRefitter.clone()
refitterForEnergyLoss.src = 'generalTracks'

# Energy Loss
energyLossProducer   = cms.EDProducer("EnergyLossProducer",
  trackProducer      = cms.string('refitterForEnergyLoss'),
  trajectoryProducer = cms.string('refitterForEnergyLoss'),
  tag = cms.string('pPb')
)

# Micro Dst
microDstProducer  = cms.EDAnalyzer("MicroDstProducer",
  allVertices     = cms.string('offlinePrimaryVertices'),
  allTracks       = cms.string('refitterForEnergyLoss'),
  allTrajectories = cms.string('refitterForEnergyLoss')
)

# Paths
produceEnergyLoss = cms.Path(refitterForEnergyLoss *
                             energyLossProducer)

produceMicroDst   = cms.Path(refitterForEnergyLoss *
                               microDstProducer)
