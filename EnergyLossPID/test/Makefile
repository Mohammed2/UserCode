eos = /afs/cern.ch/project/eos/installation/cms/bin/eos.select

#
eospath = eos/cms/store/group/phys_heavyions/$(USER)
job     = udst_pPb

# Number of hadronTree files available
nfil = $(shell $(eos) ls $(eospath)/$(job) | wc -l)

# Number of chunks
npro = 8

# from 1 to npro
list = 1 2 3 4 5 6 7 8

# nice?
nice = 

#
args = -nfiles $(nfil) -nchunks $(npro) -eospath $(eospath)/$(job)

#
all :: .extractDeposit .extractHistos

#
.extractDeposit : ../out/data_2.root
	$(nice) extractDeposit
	gzip -f ../out/depositCurves/*.dat
	touch $@

#
.extractHistos : .extractHistos_-1 \
                 .extractHistos_0 \
                 .extractHistos_1 \
                 .extractHistos_2
	gzip -f ../out/coupling_*.dat
	gzip -f ../out/histos_*.dat
	touch $@

.extractHistos_% : ../out/data_%.root
	$(nice) extractHistos -pass $*
	touch $@

#############
# Pass 2

# Process for surveyMap and depositMap and fitProbs
../out/data_2_%.root : ../out/gain_1.dat
	$(nice) processData $(args) \
          -chunk $(word 3,$(subst _, ,$(patsubst %.root,%,$@))) \
          -pass 2 -applyGain -surveyMap -depositMap -fitProbs

#############
# Pass 1

# Re-calibrate
../out/gain_1.dat : ../out/data_1.root
	$(nice) calibrateGain $(args) -pass 1

# Re-process for calibGain and surveyMap
../out/data_1_%.root : ../data/stripProps.par ../out/gain_0.dat
	$(nice) processData $(args) \
          -chunk $(word 3,$(subst _, ,$(patsubst %.root,%,$@))) \
          -pass 1 -applyGain -calibGain -surveyMap

#############
# Pass 0

# Calibrate
../out/gain_0.dat : ../out/data_0.root
	$(nice) calibrateGain $(args) -pass 0

# Process for calibGain and surveyMap
../out/data_0_%.root : ../data/stripProps.par
	$(nice) processData $(args) \
          -chunk $(word 3,$(subst _, ,$(patsubst %.root,%,$@))) \
          -pass 0 -calibGain -surveyMap

#############
# Pass -1

# Strip props
../data/stripProps.par : ../data/stripThresholds.par ../out/data_-1.root
	$(nice) evalStripProps

#############
# Process for coupledHits
../out/data_-1_%.root : list.txt
	$(nice) processData $(args) \
          -chunk $(word 3,$(subst _, ,$(patsubst %.root,%,$@))) \
          -pass -1 -coupledHits

#############
# Root
../out/data_%.root : \
       $(foreach i,$(list),../out/data_%_$(i).root)
	$(nice) hadd -f $@ $^

#############
# File list
list.txt :
	$(eos) ls $(eospath)/$(job) > $@

clean :
	rm -f ../out/data_*_?.root
	rm -f ../out/hits_*.bin

fullclean :
	rm -f list.txt
	#
	rm -f ../data/gain_?.dat
	rm -f ../data/stripProps.par
	#
	rm -f ../out/*.dat
	rm -f ../out/*.dat.gz
	rm -f ../out/depositCurves/*.dat
	rm -f ../out/depositCurves/*.dat.gz
	rm -f ../out/data_*.root
	rm -f ../out/mostprob_?.dat
	#
	rm -f ../eps/*.eps
	rm -f ../eps/*.pdf
	rm -f ../eps/validation/*.eps
	rm -f ../eps/validation/*.pdf
