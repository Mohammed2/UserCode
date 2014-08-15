gp = gnuplot

.SECONDARY :

all :: .gain .stripThreshold .stripProps .validation .histos \
       .fitmostprobable .result

.result : result.gnu myStyleEps.gnu \
          ../out/result.dat.gz
	$(gp) $<
	touch $@

.fitmostprobable : fitMostProbable.gnu myStyleEps.gnu select.awk \
                   ../out/mostprob_1.dat ../out/mostprob_2.dat
	$(gp) $<
	touch $@

.stripThreshold : stripThreshold.gnu _stripThreshold.gnu myStyleEps.gnu \
                  $(wildcard ../out/coupling_*.dat.gz)
	$(gp) $<
	touch $@

.stripProps : stripProps.gnu _stripProps.gnu myStyleEps.gnu \
              ../data/stripProps.par \
              $(wildcard ../out/coupling_*.dat.gz)
	$(gp) $<
	touch $@

.gain : gain.gnu myStyleEps.gnu ../data/gain_1.dat
	$(gp) $<
	touch $@

.histos : histos.gnu _histos.gnu epsilon.gnu myStyleEps.gnu \
       $(wildcard ../out/histos_*_*_2.dat.gz)
	$(gp) $<
	touch $@

.validation : validation.gnu _validation.gnu \
              _validation_per_det.gnu \
              __validation.gnu myStyleEps.gnu \
                $(wildcard ../out/depositCurves/his_*_*.dat.gz) \
                $(wildcard ../out/depositCurves/sig_*_*.dat.gz)
	which gnuplot
	$(gp) $<
	touch $@

