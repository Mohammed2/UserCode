#!/bin/tcsh

if($#argv == 1) then

 set tag = $1

 source /afs/cern.ch/cms/LCG/LCG-2/UI/cms_ui_env.csh
 source /afs/cern.ch/cms/ccs/wm/scripts/Crab/crab.csh
 cmsenv

 set dir = .

 echo " recreating dir .."
 rm -rf $dir/$tag
 mkdir  $dir/$tag

 echo " creating jobs .."
 crab -cfg {$tag}.cfg -create

 echo " submitting jobs .."
 crab -submit -c $tag

else

 echo "Usage: submitCrab.csh <job>"

endif
