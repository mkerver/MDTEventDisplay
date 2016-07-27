#!/bin/bash
#if [[ "$1" == "-h" || "$1" == "--help" ]]; then
#  cat <<EOF
#  copytotmp.sh => Functions for condor jobs.
#  source this file to define functions for bash scripts
#
#  copytotmp() => Copies files to /tmp/$USER/\$1
#  cleantmp()  => Delete directory /tmp/$USER/\$1
#  do_condor() => Submit condor jobs for a list of ntuples
#  submit_job() => Submit individual condor job
#  print_jobfile() => Write condor jobfile
#  split_by_size() => Split a list of files into subfiles
#EOF
#  exit
#fi
#############################################################
#  copytotmp() => Copies files to /tmp/$USER/$1
#                 writes list of files in /tmp called ${list}_tmp.txt
#  $1 = List of data files to copy (text files, one name per line)
#                 writes list of files in /tmp called ${list}_tmp.txt
##############################################################
function copytotmp() {
  list=`basename $1 .txt`
  tmplist=${list}_tmp.txt
  tmpdir=/tmp/$USER/$list
  mkdir -p $tmpdir
  if [[ `grep lustre $list.txt` != "" ]]; then
    echo ====== `date +"%x %T"` Files on lustre - use $list.txt directly
    cp $list.txt $tmplist
  else
    echo ====== `date +"%x %T"` copying `cat $list.txt | wc -l` files in $list.txt to $tmpdir
    for file in `cat $list.txt`; do
#  If ntuple is on dcache
      if [[ $file =~ pnfs ]]; then
        dccp -t 20 -o 20 $file $tmpdir
      else
        cp $file $tmpdir
      fi  
      if [[ $? -eq 0 ]]; then
        bfile=`basename $file`
	echo $tmpdir/$bfile >> $tmplist
#      echo ====== `date +"%x %T"`   copied $bfile 
      else
        echo ====== `date +"%x %T"`   FAIL to copy $bfile
      fi
    done
    echo ====== `date +"%x %T"`   Copied `cat $tmplist | wc -l` files
  fi
}   #end copytotmp
##############################################################
#  cleantmp() => Cleanup ntuples and lockfile at end of job
#  $1 = List of data files that was copied to /tmp/$USER/$list 
##############################################################
function cleantmp() {
  list=`basename $1 .txt`
  rm -rf /tmp/$USER/$list
  echo ====== `date +"%x %T"` deleted directory /tmp/$USER/$list
}
##############################################################
#  do_condor() => Submit condor jobs for a list of ntuples
#  Variables needed (set by calling script):
#    $condor = condor command (e.g condor_submit)
#    $ntlist = List of data files  
#    $script = Shell script for condor job
#    $opts   = options for script (ntuple list is first argument)
#    $filesizemax = Max size (kB) of ntuples to write for a job
#                 = 0 no limit
#    $jobnum = start of jobnumbers, optional to set by user
##############################################################
function do_condor() {
  jobname=`basename $ntlist .txt`
#  Use jobnum if set by user, otherwise start at 0 
  if [[ $jobnum == "" ]]; then
    jobnum=0
  fi
  filesize=0
  nfiles=0
### Loop over filelist submitting jobs when data file size 
### is greater than $filesizemax
  for file in `cat $ntlist`; do
    echo $file >> tmplist.txt
    let nfiles=$nfiles+1
    let filesize=$filesize+`du $file | cut -f 1`
    if [[ $filesize -gt $filesizemax && $filesizemax -gt 0 ]]; then
      submit_job
    fi
  done  
### Submit any leftover files
  if [[ $nfiles -gt 0 ]]; then
    submit_job
  fi
  echo "Submitted $jobnum jobs"
}
##############################################################
# submit_job() => Submit individual condor job
##############################################################
submit_job() {   
  let jobnum=$jobnum+1
  if [[ $jobnum -lt 10 ]]; then
    jobdir=${jobname}_00$jobnum
  elif [[ $jobnum -lt 100 ]]; then
    jobdir=${jobname}_0$jobnum
  else
    jobdir=${jobname}_$jobnum
  fi
  mkdir -p $jobdir
  mv tmplist.txt $jobdir/$jobdir.txt
  print_jobfile
  condornum=`$condor $jobdir/$jobdir.job | grep cluster | cut -d " " -f 6` 
  echo "Submitted condor job $jobdir with $nfiles files and job number $condornum"
  filesize=0
  nfiles=0
}
##############################################################
# print_jobfile() => Write condor jobfile
# Variables needed
#  $jobdir = directory for condor job
##############################################################
print_jobfile() {
  cat <<EOF > $jobdir/$jobdir.job
Universe   = vanilla
GetEnv     = True
Executable = $script
Arguments  = $jobdir.txt $opts
Initialdir = $PWD/$jobdir
Input      = /dev/null
Output     = $jobdir.out
Error      = $jobdir.err
Log        = $jobdir.log
Queue
EOF
}
##############################################################
# split_by_size() => Split filelist into smaller lists
#   each containing less than $filesizemax total of files (in GB)
# Inputs
#  $1 = Original filelist 
#  $filesizemax = maxsize of files in singlelist (GB)
# Outputs
#  
##############################################################
split_by_size() {
  nfiles=1
  flist=$1.001
  filesize=0
  let filelim=$filesizemax*1000000   #convert to kB
### Loop over filelist  when data file size 
### is greater than $filesizemax
  for file in `cat $1`; do
    echo $file >> $flist
    let filesize=$filesize+`du $file | cut -f 1`
    if [[ $filesize -gt $filelim ]]; then
      filesize=0
      let nfiles=$nfiles+1
      if [[ $nfiles -lt 10 ]]; then
        flist=$1.00$nfiles
      elif [[ $nfiles -lt 100 ]]; then
        flist=$1.0$nfiles
      else
        flist=$1.$nfiles
      fi	
    fi
  done  
}
