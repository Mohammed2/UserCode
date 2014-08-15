#!/usr/bin/awk -f

BEGIN {
 mpi = 0.139570
 mka = 0.493677
 mpr = 0.938272
} {
  if(NF > 3)
  {
    p = $4

  # pion
  if(id == 0 || id == 1)
    if(p/mpi > 1.5 && p < 1.0) print p/mpi,exp( $5), $6*exp( $5)

  # kaon
  if(id == 0 || id == 2)
    if(p/mka > 1.0 && p < 1.0) print p/mka,exp( $8), $9*exp( $8)

  # prot
  if(id == 0 || id == 3)
    if(p/mpr > 1.0 && p < 1.7) print p/mpr,exp($11),$13*exp($11)
  }
}
