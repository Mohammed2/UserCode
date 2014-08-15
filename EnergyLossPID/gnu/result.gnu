load "myStyleEps.gnu"

set style data d

set xlabel "p [GeV/c]"
set ylabel "{/Symbol e} [MeV/cm]"

set log

set xrange [0.1:30]
set yrange [1:30]

unset key

set label 1 "" at graph 0.9,0.9 right

set output "../eps/result_all.eps"
set label 1 "All"
plot \
 "<gunzip -c ../out/result.dat.gz" u 1:2

set output "../eps/result_highpurity.eps"
set label 1 "High purity (> 95%), |{/Symbol h}| < 1"
plot \
 "<gunzip -c ../out/result.dat.gz | awk '{if($5>0.95) print}'" u 1:2 lt 1, \
 "<gunzip -c ../out/result.dat.gz | awk '{if($6>0.95) print}'" u 1:2 lt 2, \
 "<gunzip -c ../out/result.dat.gz | awk '{if($7>0.95) print}'" u 1:2 lt 3

set output "../eps/result_highexclusion.eps"
set label 1 "High exclusion (2{/Symbol s}), |{/Symbol h}| < 1"
plot \
 "<gunzip -c ../out/result.dat.gz | \
   awk '{if( ( $8>-n &&  $8<n) && \
            !( $9>-n &&  $9<n) && \
            !($10>-n && $10<n) ) print}' n=2" u 1:2 lt 1, \
 "<gunzip -c ../out/result.dat.gz | \
   awk '{if(!( $8>-n &&  $8<n) && \
             ( $9>-n &&  $9<n) && \
            !($10>-n && $10<n) ) print}' n=2" u 1:2 lt 2, \
 "<gunzip -c ../out/result.dat.gz | \
   awk '{if(!( $8>-n &&  $8<n) && \
            !( $9>-n &&  $9<n) && \
             ($10>-n && $10<n) ) print}' n=2" u 1:2 lt 3
