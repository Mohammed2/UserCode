import FWCore.ParameterSet.Config as cms

process = cms.Process("produceEnergyLoss")

process.load("Configuration.StandardSequences.Services_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")

process.load("UserCode.EnergyLossPID.EnergyLossProducer_cff")

###############################################################################
# Message logger
process.MessageLogger = cms.Service("MessageLogger",
  cerr = cms.untracked.PSet(
    threshold = cms.untracked.string('DEBUG'),
    DEBUG     = cms.untracked.PSet(
    limit     = cms.untracked.int32(10000000)
    )
  ),
  destinations    = cms.untracked.vstring('cerr'),
  suppressWarning = cms.untracked.vstring('refitterForEnergyLoss')
)

# Source
process.source = cms.Source("PoolSource",
  skipEvents = cms.untracked.uint32(0),
  fileNames  = cms.untracked.vstring(
    # lxplus0181
    'file:/tmp/sikler/AE672BD2-E874-E211-9728-001D09F24FBA.root'
    # 'file:/tmp/sikler/044A38FB-F364-E211-88AA-003048F1183E.root'
  )
)

process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32(1000)
)

# Global tag
process.GlobalTag.globaltag = 'GR_P_V43D::All'

# Tag
process.energyLossProducer.tag = cms.string('pPb')

# Schedule
process.schedule = cms.Schedule(process.produceEnergyLoss)

