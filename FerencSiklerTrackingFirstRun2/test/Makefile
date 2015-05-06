# tags = simuMinBiasReco
# tags = dataMinBiasReco

# tags = simuMinBiasPile_2_10 simuMinBiasPile_2_20 \
#        simuMinBiasPile_2_40 simuMinBiasPile_2_60 \
#        simuMinBiasPile_3_10 simuMinBiasPile_3_20 \
#        simuMinBiasPile_3_40 simuMinBiasPile_3_60

# tags = simuMinBiasPile_1_10 simuMinBiasPile_1_20 \
#        simuMinBiasPile_1_30 simuMinBiasPile_1_40 \
#        simuMinBiasPile_1_60 \
#        simuMinBiasPile_2_30 simuMinBiasPile_3_30

# tags = simuMinBiasReco dataMinBiasReco \
#        simuPythia8M simuPythia6Z2 simuEpos simuPythia8C simuHerwig

tags = simuMinBiasReco

all :

sub :
	@ for tag in $(tags) ; do \
	 rm -rf $$tag ; \
	 crab -cfg $$tag.cfg -create ; \
	 crab -submit -c $$tag ; \
	done

stat :
	@ for tag in $(tags) ; do \
	 crab -status -c $$tag ; \
	done

get :
	@ for tag in $(tags) ; do \
	 crab -get -c $$tag ; \
	done

kill :
	@ for tag in $(tags) ; do \
	 crab -kill all -c $$tag ; \
	done
