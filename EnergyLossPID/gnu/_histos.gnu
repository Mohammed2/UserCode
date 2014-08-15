set label 1 "" at graph 0.6, graph 0.95 right front

gunzip(charge) = \
 sprintf("<gunzip -c ../out/histos_$0_%s_2.dat.gz", charge)

#
set output "../eps/histo_$0_$1_pos.eps"
set label 1 "positives" front

splot gunzip("pos") \
  u (exp($$1)):2:3 w pm3d, \
  exp(u),log(epsilon(exp(u)/mel)),1 t @elp w l lt 5 lc @green, \
  exp(u),log(epsilon(exp(u)/mpi)),1 t @pip w l lt 1, \
  exp(u),log(epsilon(exp(u)/mka)),1 t @kap w l lt 4, \
  exp(u),log(epsilon(exp(u)/mpr)),1 t @prp w l lt 3

#
set output "../eps/histo_$0_$1_neg.eps"
set label 1 "negatives" front

splot gunzip("neg") \
  u (exp($$1)):2:3 w pm3d, \
  exp(u),log(epsilon(exp(u)/mel)),1 t @elm w l lt 5 lc @green, \
  exp(u),log(epsilon(exp(u)/mpi)),1 t @pim w l lt 1, \
  exp(u),log(epsilon(exp(u)/mka)),1 t @kam w l lt 4, \
  exp(u),log(epsilon(exp(u)/mpr)),1 t @prm w l lt 3
