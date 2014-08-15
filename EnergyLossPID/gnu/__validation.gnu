bg = $1
l  = $2

set xrange [0:5]

Delta = epsilon(bg) * l

ok   = `gunzip -c ../out/depositCurves/sig_$0_$3_unknown_$1.dat.gz | \
        awk '{if($$5==$2) print $$2}' | \
        wc -l | awk '{print $$1}'`

if(ok == 1) \
  sn = `gunzip -c ../out/depositCurves/sig_$0_$3_unknown_$1.dat.gz | \
        awk '{if($$5==$2) print $$2}'`; else \
  sn = 0.

over = `gunzip -c ../out/depositCurves/his_$0_$3_unknown_$1.dat.gz | \
        awk '{if($$1==$2 && $$3>0) s++} END {print s+0}'`

if("$0" eq "PXB" ) shift = -4e-3
if("$0" eq "TIB" ) shift =  9e-3
if("$0" eq "TOB" ) shift = 13e-3

if("$0" eq "PXF" ) shift = 15e-3
if("$0" eq "TID" ) shift =  8e-3
if("$0" eq "TEC3") shift = 12e-3
if("$0" eq "TEC5") shift = 23e-3

amp = 0.001
resc = 1.

# FIXME
shift = 0.

if(take && over > 0) \
fit [0:2] prob(x) \
 "<gunzip -c ../out/depositCurves/his_$0_$3_unknown_$1.dat.gz | \
   awk '{if($$1 == $2) print}'" u 2:3 via amp #,shift,resc

#if(take && over > 0) \
#print "$0 $1 $2 $3 ",shift,shift_err,resc,resc_err

unset key

if(take && over > 0 && first == 1) \
 set label 2 "$0   $3   {/Symbol bg} = %.2f",bg at graph 0.75,0.2 center; \
 first = 0

if(take && over > 0) \
       set key at graph 0.95,1.025-0.075*ls Right nobox width -5 ; \
       else set nokey

set xrange [0:1]
if(take && over > 0 && ok == 1 && ("$4" ne "x")) \
 plot [0:0.6] "<gunzip -c ../out/depositCurves/his_$0_$3_unknown_$1.dat.gz | \
   awk '{if($$1 == $2) print}'" u 2:3:4 w e t \
  sprintf("l = %.0f {/Symbol m}m, {/Symbol s}_n = %.1f keV",l*1e+4,sn*1e+3) \
  ls ls, \
  prob(x) ls ls

if(take) if(ls == 1) unset label 2

if(take) ls = ls + 1
