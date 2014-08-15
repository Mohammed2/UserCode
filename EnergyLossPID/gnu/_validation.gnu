ls = 1

ymax = 0.15
if($1 == 0.51) ymax = 0.05
if($1 == 0.70) ymax = 0.07
if($1 == 0.86) ymax = 0.08
if($1 == 0.93) ymax = 0.09
if($1 == 1.19) ymax = 0.10
if($1 == 1.39) ymax = 0.10
if($1 == 2.08) ymax = 0.12
if($1 == 3.49) ymax = 0.14

set yrange [1e-5:ymax]

set output "../eps/validation/$0_$3_$2.eps"

set multiplot

first = 1

pixel = (("$0" eq "PXB") || ("$0" eq "PXF"))

take = 1
if( (("$0" eq "PXF") && ("$3" eq "neg")) ) \
 call "__validation.gnu" "$0" $1  270e-4 "$3" "$2"
 unset label
take = 0

call "__validation.gnu" "$0" $1  270e-4 "$3" "$2"
call "__validation.gnu" "$0" $1  280e-4 "$3" "$2"
call "__validation.gnu" "$0" $1  290e-4 "$3" "$2"

take = 1
if(!(("$0" eq "PXF") && ("$3" eq "neg")) )\
 call "__validation.gnu" "$0" $1  300e-4 "$3" "$2"
 unset label
take = 0

unset label

#call "__validation.gnu" "$0" $1  310e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  320e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  330e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  340e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  350e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  360e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  370e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  380e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  390e-4 "$3" "$2"

take = 1
 call "__validation.gnu" "$0" $1  400e-4 "$3" "$2"
take = 0

#call "__validation.gnu" "$0" $1  410e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  420e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  430e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  440e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  450e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  460e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  470e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  480e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  490e-4 "$3" "$2"

take = 1
 call "__validation.gnu" "$0" $1  500e-4 "$3" "$2"
take = 0

#call "__validation.gnu" "$0" $1  510e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  520e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  530e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  540e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  550e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  560e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  570e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  580e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  590e-4 "$3" "$2"

take = 1
 call "__validation.gnu" "$0" $1  600e-4 "$3" "$2"
take = 0

#call "__validation.gnu" "$0" $1  610e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  620e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  630e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  640e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  650e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  660e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  670e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  680e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  690e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  700e-4 "$3" "$2"

#call "__validation.gnu" "$0" $1  710e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  720e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  730e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  740e-4 "$3" "$2"

if(!pixel) take = 1
 call "__validation.gnu" "$0" $1  750e-4 "$3" "$2"
take = 0

#call "__validation.gnu" "$0" $1  760e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  770e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  780e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  790e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  800e-4 "$3" "$2"

#call "__validation.gnu" "$0" $1  810e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  820e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  830e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  840e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  850e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  860e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  870e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  880e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  890e-4 "$3" "$2"

if(!pixel) take = 1
 call "__validation.gnu" "$0" $1  900e-4 "$3" "$2"
take = 0

#call "__validation.gnu" "$0" $1  910e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  920e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  930e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  940e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  950e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  960e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  970e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  980e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1  990e-4 "$3" "$2"
#call "__validation.gnu" "$0" $1 1000e-4 "$3" "$2"

# FIXME
take = 0
 call "__validation.gnu" "$0" $1 1050e-4 "$3" "$2"
take = 0

unset label
set nomultiplot
