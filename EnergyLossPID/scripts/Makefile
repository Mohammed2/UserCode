eos = /afs/cern.ch/project/eos/installation/cms/bin/eos.select

#
eospath = /eos/cms/store/group/phys_heavyions/$(USER)
job      = udst_pPb

all ::
	echo ""

#
submit ::
	./submitCrab.csh $(job)

status ::
	crab -status -c $(job)

stat ::
	@ crab -status -c $(job) | \
                awk '{if($$2=="Y" || $$2=="N") \
                        a[substr($$3,1,6),"\t",$$4,"\t",$$5]++} \
                        END {for(x in a) print a[x],"\t",x}'
list ::
	@ $(eos) ls -l $(eospath)/$(job)

size ::
	@ $(eos) ls -l $(eospath)/$(job) | \
	 awk '{s += $$5} END {printf("%.2f GByte\n", s*1e-9)}'

remove ::
	$(eos) rm -r $(eospath)/$(job)

kill ::
	crab -kill 1-100 -c $(job)

get ::
	crab -get -c $(job)

#
test_udst ::
	cmsRun produceMicroDst.py

test_dedx ::
	cmsRun produceEnergyLoss.py
