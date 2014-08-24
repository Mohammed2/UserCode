maxcpu = 4

b ::
	scram b --reset

bm ::
	scram b --reset -j $(maxcpu)

clean ::
	rm -rf ../../../tmp
	rm -rf ../../../lib

run ::
	cd test ; make -j $(maxcpu)
	cd gnu  ; make -j $(maxcpu)
	cd eps  ; make -j $(maxcpu)
	cd tex  ; make

tagFiles ::
ifdef tag
	 cd data; cp gain_1.dat           gain_$(tag).dat
	 cd data; cp stripProps.par stripProps_$(tag).par
	 cp out/mostprob_2.dat   data/mostprob_$(tag).dat
else
	 @ echo "Error: tag is empty; please use 'make tagFiles tag=<string>'"
endif


ps ::
	@ ps uax | grep processData   | grep -v grep | grep -v sh | \
	  awk '{print $$10,$$11,$$18,$$19,$$20,$$21}'
	@ ps uax | grep calibrateGain | grep -v grep | grep -v sh | \
	  awk '{print $$10,$$11,$$18,$$19}'
