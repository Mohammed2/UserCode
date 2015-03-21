# tag = simuMinBiasPile
# tag = simuMinBiasReco
tag = dataMinBiasReco

all :

sub :
	rm -rf $(tag)
	crab -cfg $(tag).cfg -create
	crab -submit -c $(tag)

stat :
	crab -status -c $(tag)

get :
	crab -get -c $(tag)
