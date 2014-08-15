load "myStyleEps.gnu"
load "epsilon.gnu"

set samples 1000
set pointsize 1.5

set xlabel offset 0,0.5

nu     = 0.65
sigma0 = 2e-3
b      = 0.095 #0.1

sigmaD_(y) = sqrt((sigma0 + b * y)**2 + sn**2) 

sigmaD(y) = sigmaD_(y) * resc

Sqr(x) = x**2

chi2(y) = (Delta < y - nu * sigmaD(y) ? \
    -2*nu*(Delta - y)/sigmaD(y) - Sqr(nu) : \
      Sqr((Delta - y)/sigmaD(y))) + 2*log(sigmaD(y))

prob(y) = amp * exp(-chi2(y - shift)/2)
ay = 1; by = 0.01

amp = 1e-3

# FIXME
set xrange [0:2]

set xlabel "Deposit [MeV]"
set ylabel "Probability density [MeV^{-1}]" offset 2,0

###########################################
call "_validation_per_det.gnu" "PXB"  "pos"
call "_validation_per_det.gnu" "PXF"  "pos"
call "_validation_per_det.gnu" "TIB"  "pos"
call "_validation_per_det.gnu" "TID"  "pos"
call "_validation_per_det.gnu" "TOB"  "pos"
call "_validation_per_det.gnu" "TEC3" "pos"
call "_validation_per_det.gnu" "TEC5" "pos"

###########################################
call "_validation_per_det.gnu" "PXB"  "neg"
call "_validation_per_det.gnu" "PXF"  "neg"
call "_validation_per_det.gnu" "TIB"  "neg"
call "_validation_per_det.gnu" "TID"  "neg"
call "_validation_per_det.gnu" "TOB"  "neg"
call "_validation_per_det.gnu" "TEC3" "neg"
call "_validation_per_det.gnu" "TEC5" "neg"

