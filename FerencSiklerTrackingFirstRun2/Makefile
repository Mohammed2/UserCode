all ::
	# cd ../.. ; scram b --fast -j 4
	# env USER_CXXFLAGS="-g -D=EDM_ML_DEBUG" scram b --fast -j 4
	# cd ../.. ; scram b -j 4
	# env USER_CXXFLAGS="-g -D=EDM_ML_DEBUG" scram b -j 4

run ::
	cd test ; cmsRun simuMinBiasReco.py >& /dev/null ; \
	 mv hadronTree.root hadronTree_minBias.root 
	cd test ; cmsRun simuDefaultReco.py >& /dev/null ; \
  	 mv hadronTree.root hadronTree_default.root 

