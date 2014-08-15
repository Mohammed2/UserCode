set key top right Left reverse samplen 2 width +1 box noauto 
set bar small

# Margin
set lmargin at screen 0.15
set rmargin at screen 0.95
set bmargin at screen 0.175
set tmargin at screen 0.95

# Terminal
set term post eps enh color dashed dl 2 "Helvetica" 25 size 5in,4.5in

set pointsize 1.5
set tics scale 2,1

set mxtics 5
set mytics 5

set macros
set fit logfile "/dev/null" errorvariables

# Styles, colors
green = "rgb \"dark-green\""

set style line  1 lt 1 lw 3 lc 1      pt 6
set style line  2 lt 2 lw 3 lc @green pt 8 
set style line  3 lt 3 lw 3 lc 3      pt 4
set style line  4 lt 4 lw 3 lc 4      pt 12
