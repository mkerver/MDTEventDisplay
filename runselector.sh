#!/bin/bash
###  Location of ROOT selector scripts
#rootdir="/atlas/data18c/muoncal/public/root"
##rootdir="/afs/cern.ch/user/k/kblack/scratch1/HEB_STUFF/Deihl/ROOT"
rootdir="/afs/cern.ch/user/m/mkerver/public/HEB/ROOT"
#  Suggested location for root macros
#rootdir="$HOME/root/macros"
###  Max size (kB) of ntuples to write for a job
filesizemax=20000000   #20GB
print_help() {   #use -h or --help option to print help
  cat <<EOF
   Run various flavors of selector functions to work with calib ntuples
     Edward Diehl 10/30/07

   runselector.sh <selector> <ntuplelist> [<Nevents>] [<opts>] [-b]  

   <selector> = which flavor of selector.C to run, e.g:
      = event     => print event data (MDT,TGC,RPC hits)
      = resi      => make hit residuals histograms
      = dumprt    => compare RTs in ntuple to RTs in ascii file
      = dumpt0    => dump T0s to a file
      = checksize => check size of arrays in ntuples (to adjust selector.h)
      Selector functions are in directory $rootdir
      and are called selector_XXX.C where XXX=event, rtplot, etc
   <ntuplelist> = name of ROOT file to analyse, or list of ROOT files 
        default = file_list.txt
   <Nevents> = N => number of files to analyse 0=All
        -N => number of events to analyse
   <opts> = option for selector function (function depends on selector function)
    -b = Run as condor job.
         Creates a condor job directory called <ntuplelist>_<selector_name><N>
         where <ntuplelist> is taken from the name of the list
                of ntuples for the job (with ".txt" removed)
         <selector_name> is the name of the selector function
         <N> is a unique integer, starting from 1.  That is,
         a unique integer will be found in order to create a
	 unique directory name so you do not overwrite an
	 existing directory.

         Multiple condor jobs will be submitted containing a partial
         list of ntuples in order to keep the disk size of ntuples per 
         job below filesizemax=$filesizemax kb

  Operation: selector_<selector>.C, selector.h (or
  selector_<selector>.h if exists) are copied to /tmp/\$USER/selector
  as selector.C and selector.h, respectively.  

EOF
}
##############################################################
# Write condor jobfile
##############################################################
print_jobfile() {
  cat <<EOF > $jobdir/$jobdir.job
Universe   = vanilla
GetEnv     = True
Executable = /afs/atlas.umich.edu/home/muoncal/public/root/sb
Arguments  = $jobdir.txt $maxf $opts
Initialdir = $PWD/$jobdir
Input      = /dev/null
Output     = $jobdir.out
Error      = $jobdir.err
Log        = $jobdir.log
Queue
EOF
}
##############################################################
# Submit condor job
##############################################################
submit_job() {
  let njobs=$njobs+1  
# Find a unique jobdir
  let jobnum=$jobnum+1
  jobdir=$jobname$jobnum
  while [[ -e $jobdir ]]; do
    let jobnum=$jobnum+1
    jobdir=$jobname$jobnum
  done
  mkdir -p $jobdir
  mv tmplist.txt $jobdir/$jobdir.txt
  if [[ -e selector_$selector.C ]]; then
    cp selector_$selector.C $jobdir/selector.C
  elif [[ -e $rootdir/selector_$selector.C ]]; then
    cp $rootdir/selector_$selector.C $jobdir/selector.C
  fi
  if [[ -e selector_$selector.h ]]; then
    cp selector_$selector.h $jobdir/selector.h
  elif [[ -e $rootdir/selector_$selector.h ]]; then
    cp $rootdir/selector_$selector.h $jobdir/selector.h
  else
    cp $rootdir/selector_null.h $jobdir/selector.h
  fi
#  This bit only applies to selector_dumpt0.C but is harmless 
  if [[ -e t0_list.txt ]]; then
    cp t0_list.txt $jobdir
#  for dumprt
  elif [[ -e rtlist.txt ]]; then
    cp rtlist.txt $jobdir
  fi
  print_jobfile
  condor_submit $jobdir/$jobdir.job
  echo "Submitted condor job $jobdir with $nfiles files"
  filesize=0
  nfiles=0
}
##############################################################
#  MAIN
##############################################################
### Parse arguments
while [ $# -ne 0 ]; do
  if [[ "$1" == "-h" || "$1" == "--help" ]]; then
    print_help; exit
### Set for batch job
  elif [[ $1 == "-b" ]]; then
    batch=1
### parameter 1 selector function to use
  elif [[ $selector == "" ]]; then
    selector=$1
###  Check that selector function exists
    if [[ ! -e "$rootdir/selector_$selector.C" && ! -e selector_$selector.C ]]; then
      echo "  ERROR: No selector function selector_$selector.C in PWD or $rootdir"
      echo "  List of available selector functions:"
      ls $rootdir/selector_*.C
      exit
    fi  
### parameter 2 list or root files or root file
  elif [[ $filelist == "" ]]; then
### Check if it is a list of root files or is a root file
    if [[ -e "$1" ]]; then
#      if [[ `file $1 | grep -o ASCII` == "ASCII" ]]; then
#        filelist=$1
      if [[ `file $1 | grep -o ROOT` == "ROOT" ]]; then
#      elif [[ `echo $1 | grep -o root` == "root" ]]; then
        echo $1 > tmplist.txt
        filelist=tmplist.txt
      else
	filelist=$1
#        echo "ERROR: $1 is not a filelist or ROOT file"; exit
      fi
    elif [[ -e "$1.txt" ]]; then
      filelist=$1.txt
    elif [[ -e "$1.root" ]]; then
      echo $1.root > tmplist.txt
      filelist=tmplist.txt
    else
      echo "  ERROR: Cannot find a file list or root file named $1"
      exit
    fi
### parameter 3 max files/events
  elif [[ $maxf == "" ]]; then
    maxf=$1
### parameter 4 option for selector function 
  elif [[ $opts == "" ]]; then
    opts=$1
  fi 
  shift 1 
done
### If no selector function chosen, get one from user
if [[ $selector == "" ]]; then
  echo "  ERROR:  No selector function specified.  Use -h for help"
  echo "  List of available selector functions:"
  ls $rootdir/selector_*.C
  exit
fi

### Set default file list
if [[ $filelist == "" ]]; then 
  if [[ -e file_list.txt ]]; then
    $filelist=file_list.txt
  else
    echo "  ERROR: No default filelist file_list.txt"; exit
  fi
fi
### Set max events if not already set
if [[ "$maxf" == "" ]]; then
  maxf=0
fi  

### Interactive job
if [[ $batch != 1 ]]; then

### Copy the selector you want to use to $tmpdir
### Use local copy if exists
  if [[ -e selector_$selector.C ]]; then
#    cp selector_$selector.C  $rootdir/selector.C 
    cp selector_$selector.C  selector.C 
    echo USING local selector_$selector.C
  elif [[ -e $rootdir/selector_$selector.C ]]; then
#    cp $rootdir/selector_$selector.C  $rootdir/selector.C 
    cp $rootdir/selector_$selector.C  selector.C 
    echo USING $rootdir/selector_$selector.C
  fi

###  Use specific selector.h if it exists.
  if [[ -e selector_$selector.h ]]; then
#    cp selector_$selector.h  $rootdir/selector.h
    cp selector_$selector.h  selector.h
    echo USING local selector_$selector.h 
  elif [[ -e selector.h ]]; then
#    cp selector.h  $rootdir/selector.h
    cp selector.h  selector.h
    echo USING local selector.h 
  elif [[ -e $rootdir/selector_$selector.h ]]; then
#    cp $rootdir/selector_$selector.h  $rootdir/selector.h
    cp $rootdir/selector_$selector.h  selector.h
    echo USING $rootdir/selector_$selector.h 
  else 
#    cp $rootdir/selector_null.h  $rootdir/selector.h
    cp $rootdir/selector_null.h  selector.h
    echo Using $rootdir/selector_null.h
  fi
###  Run ROOT and hope for the best
  root -l -b -q "$rootdir/runselector.C(\"$filelist\",$maxf,\"$opts\")" 
### cleanup - do not cleanup in case made output files.
  rm -f selector.C selector.h selector_C.so

### ELSE run batch job
else 
  jobnum=0
  njobs=0
  filesize=0
  nfiles=0
  jobname="`basename $filelist .txt`_$selector"
### Loop over filelist submitting jobs when data file size 
### is greater than $filesizemax
  while read file; do
    echo $file >> tmplist.txt
    let nfiles=$nfiles+1
    let filesize=$filesize+`du $file | cut -f 1`
    if [[ $filesize -gt $filesizemax ]]; then
      submit_job
    fi
  done  < $filelist
### Submit any leftover files
#  if [[ $nfiles -gt 0 && $jobnum -lt 10 ]]; then
  if [[ $nfiles -gt 0 ]]; then
    submit_job
  fi
  echo "Submitted $njobs jobs"
fi
