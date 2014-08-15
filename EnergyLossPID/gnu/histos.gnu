load "myStyleEps.gnu"
load "epsilon.gnu"

set colorbox front

set key height +0.3

# FIXME rgb
color1(gray) = sqrt(gray)     + exp(-gray/0.01)
color2(gray) = gray**3        + exp(-gray/0.01)
color3(gray) = sin(2*pi*gray) + exp(-gray/0.01)

set palette model RGB functions color1(gray), color2(gray), color3(gray)

set lmargin 1
set rmargin 0
set tmargin 0
set bmargin 1

set lmargin at screen 0.15
set rmargin at screen 0.95
set bmargin at screen 0.175
set tmargin at screen 0.95

set pm3d corners2color c1 explicit map

set xlabel "p [GeV/c]"
set ylabel "log({/Symbol e}/[MeV/cm])"

set xrange [0.1:10]
set log x

set xtics ("0.1" 0.1, "0.2" 0.2, "" 0.3, "" 0.4, "0.5" 0.5, \
           "" 0.6, "" 0.7, "" 0.8, "" 0.9, \
           "1" 1, "2" 2, "" 3, "" 4, "5" 5, "10" 10)
set xtics ("0.1" 0.1, "0.2" 0.2, "" 0.3, "" 0.4, "0.5" 0.5, \
           "" 0.6, "" 0.7, "" 0.8, "" 0.9, \
           "1" 1, "2" 2, "" 3, "" 4, "5" 5, "10" 10)

set yrange [0.0:3.5]

set parametric
set urange [-3:3.5]

pip_ = 11; set style line pip_ lt 1 lw 3 lc  1 pt 7
pim_ = 12; set style line pim_ lt 1 lw 3 lc  1 pt 6
kap_ = 13; set style line kap_ lt 2 lw 3 lc @green pt 9
kam_ = 14; set style line kam_ lt 2 lw 3 lc @green pt 8
prp_ = 15; set style line prp_ lt 3 lw 3 lc  3 pt 5
prm_ = 16; set style line prm_ lt 3 lw 3 lc  3 pt 4
elp_ = 17; set style line elp_ lt 4 lw 3 lc  5 pt 11
elm_ = 18; set style line elm_ lt 4 lw 3 lc  5 pt 10

elp = "\"e^+\""
elm = "\"e^{/Symbol -}\""
pip = "\"{/Symbol p}^+\""
pim = "\"{/Symbol p}^{/Symbol -}\""
kap = "\"K^+\""
kam = "\"K^{/Symbol -}\""
prp = "\"p\""
prm = "\"@^{/Symbol -}p\""

fpion(p) = epsilon(p/mpi)
fkaon(p) = epsilon(p/mka)
fprot(p) = epsilon(p/mpr)
fsigm(p) = epsilon(p/msi)

fpika(p) = (fpion(p)+fkaon(p))/2
fkapr(p) = (fkaon(p)+fprot(p))/2

set colorbox vertical user origin 0.825,0.6 size 0.03,0.3 front
set cbtics textcolor lt -1# offset -1.5,0

set auto cb
set auto z

set log z
set log cb ; set format cb "10^{%T}"

set key off

call "_histos.gnu" "pix" "log"
call "_histos.gnu" "str" "log"
call "_histos.gnu" "all" "log"

#
set key on
unset log z
unset log cb
set format cb "%.1t"

set colorbox origin graph 0.675,graph 0.55
set cbtics

call "_histos.gnu" "pix" "lin"
call "_histos.gnu" "str" "lin"
call "_histos.gnu" "all" "lin"
