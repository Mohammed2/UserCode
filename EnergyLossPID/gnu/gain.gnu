load "myStyleEps.gnu"

PXB  = 51; set style line PXB  lt 1 lw 3 lc  0 pt 13
PXF  = 52; set style line PXF  lt 2 lw 3 lc  0 pt 12

TIB  = 53; set style line TIB  lt 1 lw 3 lc  1 pt 7
TID  = 54; set style line TID  lt 2 lw 3 lc @green pt 6
TOB  = 55; set style line TOB  lt 3 lw 3 lc  3 pt 9
TEC3 = 56; set style line TEC3 lt 4 lw 3 lc  4 pt 5
TEC5 = 57; set style line TEC5 lt 5 lw 3 lc  5 pt 8

set lmargin 5
set rmargin 2

set xlabel "Multiplicative gain correction"
set yrange [1:]
set style data his

grep(det) = sprintf("<grep %s ../data/gain_1.dat | \
                      ./histogram.csh 4 0.7 1.5 80 -", det)

#
set output "../eps/gain_h_pixel.eps"
plot \
 grep("PXB") t "PXB" ls PXB, \
 grep("PXF") t "PXF" ls PXF

grep(det) = sprintf("<grep %s ../data/gain_1.dat | \
                      ./histogram.csh 4 0.8 1.3 500 -", det)

#
set output "../eps/gain_h_strip.eps"
plot \
 grep("TIB")  t "TIB"  ls TIB, \
 grep("TID")  t "TID"  ls TID, \
 grep("TOB")  t "TOB"  ls TOB, \
 grep("TEC3") t "TEC3" ls TEC3, \
 grep("TEC5") t "TEC5" ls TEC5

set auto x
set auto y
#set log y; set format y "10^{%T}"

set xlabel "Hits on chip"

#
set output "../eps/gain_hits_pixel.eps"

set xtics 0,2000

grep(det) = sprintf("<grep %s ../data/gain_1.dat | \
                      ./histogram.csh 3 0. 10000. 100 -", det)

plot \
 grep("PXB") t "PXB" ls PXB, \
 grep("PXF") t "PXF" ls PXF

#
set output "../eps/gain_hits_strip.eps"
plot \
 grep("TIB")  t "TIB"  ls TIB, \
 grep("TID")  t "TID"  ls TID, \
 grep("TOB")  t "TOB"  ls TOB, \
 grep("TEC3") t "TEC3" ls TEC3, \
 grep("TEC5") t "TEC5" ls TEC5

unset log y; set format y
set xlabel "Precision of gain estimate"

set xtics 0,0.005

grep(det) = sprintf("<grep %s ../data/gain_1.dat | awk '{print sqrt($5)}' | \
                      ./histogram.csh 1 0. 0.02 100 -", det)

#
set output "../eps/gain_prec_pixel.eps"
plot \
 grep("PXB") t "PXB" ls PXB, \
 grep("PXF") t "PXF" ls PXF

#
set output "../eps/gain_prec_strip.eps"
plot \
 grep("TIB")  t "TIB"  ls TIB, \
 grep("TID")  t "TID"  ls TID, \
 grep("TOB")  t "TOB"  ls TOB, \
 grep("TEC3") t "TEC3" ls TEC3, \
 grep("TEC5") t "TEC5" ls TEC5
