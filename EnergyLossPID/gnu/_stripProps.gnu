set output "../eps/stripProps_$0.eps"
set label 1 "$0" at graph 0.9,0.9 right

thr   = `grep $0 ../data/stripProps.par | awk '{print $$2}'`
alpha = `grep $0 ../data/stripProps.par | awk '{print $$3}'`
sigma = `grep $0 ../data/stripProps.par | awk '{print $$4}'`

sum = 1e+9
if("$0" eq "TIB" ) sum = 80
if("$0" eq "TID" ) sum = 75
if("$0" eq "TEC3") sum = 85

set label 2 "t = %.1f [ADC]",          thr   at graph 0.1,0.9 left front
set label 3 "{/Symbol a} = %.3f",alpha,", r = %.3f",r(alpha) at graph 0.1,0.8 left front
set label 4 "{/Symbol s} = %.1f [ADC]",sigma at graph 0.1,0.7 left front

splot "<gunzip -c ../out/coupling_$0.dat.gz" w pm3d, \
       u, r(alpha)*u, 1                , \
       u, thr,        1                , \
       u,(sum-u < 30 ? sum-u : 1/0), 1 lt 0
