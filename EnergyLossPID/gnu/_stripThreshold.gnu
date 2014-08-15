load "myStyleEps.gnu"

set style data his

TIB  = 53; set style line TIB  lt 1 lw 3 lc  1 pt 7
TID  = 54; set style line TID  lt 2 lw 3 lc @green pt 6
TOB  = 55; set style line TOB  lt 3 lw 3 lc  3 pt 9
TEC3 = 56; set style line TEC3 lt 4 lw 3 lc  4 pt 5
TEC5 = 57; set style line TEC5 lt 5 lw 3 lc  5 pt 8

###############################################
set output "../eps/stripProps_threshold.eps"

set xlabel "Cluster deposit [ADC]"
set ylabel "Fraction"

set xrange [-0.5:50.5]

thr = `grep TIB  ../data/stripProps.par | awk '{print $$2}'`
t0 = thr+0.01
set arrow from t0,graph 0.6 to t0,graph 0.4 lt 1

thr = `grep TID  ../data/stripProps.par | awk '{print $$2}'`
t0 = thr+0.01
set arrow from t0,graph 0.6 to t0,graph 0.4 lt 2

thr = `grep TOB  ../data/stripProps.par | awk '{print $$2}'`
t0 = thr+0.01
set arrow from t0,graph 0.6 to t0,graph 0.4 lt 3

thr = `grep TEC3 ../data/stripProps.par | awk '{print $$2}'`
t0 = thr+0.01
set arrow from t0,graph 0.6 to t0,graph 0.4 lt 4

thr = `grep TEC5 ../data/stripProps.par | awk '{print $$2}'`
t0 = thr+0.01
set arrow from t0,graph 0.6 to t0,graph 0.4 lt 5

gunzip(det) = sprintf("<gunzip -c ../out/coupling_%s.dat.gz | \
   awk '{if(NF==8) { a[$$5]+=$$3; s+=$$3 } } END {for(x=0; x<254; x++) \
   print x,(a[x]+0)/s}'" , det)

plot \
 gunzip("TIB")  u 1:2 t "TIB"  ls TIB, \
 gunzip("TID")  u 1:2 t "TID"  ls TID, \
 gunzip("TOB")  u 1:2 t "TOB"  ls TOB, \
 gunzip("TEC3") u 1:2 t "TEC3" ls TEC3, \
 gunzip("TEC5") u 1:2 t "TEC5" ls TEC5

unset arrow

########################################
set output "../eps/stripProps_total.eps"
set xrange [-0.5:255.5]
replot
