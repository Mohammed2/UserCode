load "myStyleEps.gnu"

pk1 = 1.0 # pion, kaon, 1
pk2 = 1.2 # pion, kaon, 2

pr1 = 1.7 # prot 1
pr2 = 2.0 # prot 2

set xlabel "p [GeV/c]"

set xtics ("0.1" 0.1, "0.2" 0.2, "0.5" 0.5, "1" 1, "2" 2, "3" 3)
set log x

set ylabel offset 2,0

el_ = "\"e\""
pi_ = "\"{/Symbol p}\""
ka_ = "\"K\""
pr_ = "\"p\""

pip_ = 11; set style line pip_ lt 1 lw 3 lc 1 pt 7
pim_ = 12; set style line pim_ lt 1 lw 3 lc 1 pt 6
kap_ = 13; set style line kap_ lt 2 lw 3 lc 4 pt 9
kam_ = 14; set style line kam_ lt 2 lw 3 lc 4 pt 8
prp_ = 15; set style line prp_ lt 3 lw 3 lc 3 pt 5
prm_ = 16; set style line prm_ lt 3 lw 3 lc 3 pt 4
elp_ = 17; set style line elp_ lt 5 lw 3 lc @green pt 13
elm_ = 18; set style line elm_ lt 5 lw 3 lc @green pt 12

mel = 0.511e-3
mpi = 0.139570
mka = 0.493677
mpr = 0.938272
mde = 1.8756

set style line 22 lt 1 lw 3 lc 0

##############################################################################
set style data p

set ylabel "Probability ({/Symbol p},K,p) or rescaler ({/Symbol a})"

set key at graph 0.225, graph 0.5

set output "../eps/fitted_mean_and_rescale_1.eps"
plot [0.1:3][0:1.6] "../out/mostprob_1.dat" \
     u 4:($4 < pk1 ?  $7 : 1/0) t @pi_ ls pim_, \
  "" u 4:($4 < pk1 ? $10 : 1/0) t @ka_ ls kam_, \
  "" u 4:($4 < pr1 ? $13 : 1/0) t @pr_ ls prm_, \
  "" u 4:($4 < pr1 ? $14 : 1/0) t "{/Symbol a}" ls elm_, \
  1 ls 21

set output "../eps/fitted_mean_and_rescale_2.eps"
plot [0.1:3][0:1.6] "../out/mostprob_2.dat" \
     u 4:($4 < pk2 ?  $7 : 1/0) t @pi_ ls pim_, \
  "" u 4:($4 < pk2 ? $10 : 1/0) t @ka_ ls kam_, \
  "" u 4:($4 < pr2 ? $13 : 1/0) t @pr_ ls prm_, \
  "" u 4:($4 < pr2 ? $14 : 1/0) t "{/Symbol a}" ls elm_, \
  1 ls 21

##############################################################################
set style data e

#
f(x) = a0 + a1*x + a2*x**2 + a3*x**3

fit [][log(2.5):log(6.5)] f(log(x)) \
  "<./select.awk id=0 ../out/mostprob_2.dat" \
  u 1:(log($2)):($2/$3) via a0,a1,a2,a3

set key at graph 0.225, graph 0.5 box

set ylabel "{/Symbol \341}ln({/Symbol e}/[MeV/cm]){/Symbol \361}"

#
set output "../eps/mostProbable_vs_p_1.eps"

plot [0.1:3][log(2.5):log(6.5)] "../out/mostprob_1.dat" \
    u 4:($4 < pk1 ? $5 : 1/0):6  t @pi_ ls pim_, \
 "" u 4:($4 < pk1 ? $8 : 1/0):9  t @ka_ ls kam_, \
 "" u 4:($4 < pr1 ? $11: 1/0):12 t @pr_ ls prm_, \
 (x < pk1 ? f(log(x/mpi)) : 1/0) ls 22, \
 (x < pk1 ? f(log(x/mka)) : 1/0) ls 22, \
 (x < pr1 ? f(log(x/mpr)) : 1/0) ls 22

#
set output "../eps/mostProbable_vs_p_2.eps"

plot [0.1:3][log(2.5):log(6.5)] "../out/mostprob_2.dat" \
    u 4:($4 < pk2 ? $5 : 1/0):6  t @pi_ ls pim_, \
 "" u 4:($4 < pk2 ? $8 : 1/0):9  t @ka_ ls kam_, \
 "" u 4:($4 < pr2 ? $11: 1/0):12 t @pr_ ls prm_, \
 (x < pk2 ? f(log(x/mpi)) : 1/0) ls 22, \
 (x < pk2 ? f(log(x/mka)) : 1/0) ls 22, \
 (x < pr2 ? f(log(x/mpr)) : 1/0) ls 22
