#!/bin/bash
###  Location of ROOT scripts
rootdir="$HOME/root/macros"
print_help() {   #use -h or --help option to print help
  cat <<EOF
   Run a script in ROOT batch mode.  Script is compiled first.
     Edward Diehl 1/12/10

   Inputs:
   -NC => DO NOT compile root script (default=compile)
   \$1 = Root script to run either in \$PWD or $rootdir
   \$2... = Parameters for root script (depends on script)
         If parameter is a number is passed as a number.
         If parameter is not a number it is passed as a string
EOF
}
##############################################################
if [[ "$1" == "-h" || "$1" == "--help" ]]; then
  print_help; exit
fi
### parameter 1 is root script to use
###  Check that selector function exists
if [[ "$1" == "-NC" ]]; then
  nocompile=1
  shift 1
fi
#  Check PWD for scripts first
if [[ -e "$1" || -e "$1.C" ]]; then
  rootscript=`basename $1 .C`.C
#  Check rootdir for scripts second
elif [[ -e "$rootdir/$1" || -e "$rootdir/$1.C" ]]; then
  rootscript=$rootdir/`basename $1 .C`.C
else
  echo "ERROR: cannot find root script $1 or $rootdir/$1"
  exit
fi  
shift 1
### Check for parameters.  If they are not numbers put quotes around them.
### if parameter begins with "_" strip it off and pass it like as string
### in case you want to pass a number as a string.
if [[ "$1" != "" ]]; then
  if echo $1 | grep '^_'; then
    params="$params,\"`echo $1 | sed 's/^[_]*//'`\""
#  This grep is to find parameters which are numbers
  elif echo $1 | grep '^[-|+|0-9|.][.0-9]*$' &> /dev/null ; then
    params=$1
#  If parameter is not a number, then treat as string (put in quotes)
  else
    params=\"$1\"
  fi
  shift 1
### Parse arguments
  while [ $# -gt 0 ]; do
    if echo $1 | grep '^_'; then
      params="$params,\"`echo $1 | sed 's/^[_]*//'`\""
    elif echo $1 | grep '^[-|+|0-9|.][.0-9]*$' &> /dev/null ; then
      params="$params,$1"
    else
      params="$params,\"$1\""
    fi
    shift 1 
  done
fi

###  Run ROOT and hope for the best
echo Running "$rootscript($params)"
if [[ $nocompile -eq 1 ]]; then
  echo NO COMPILE
  root -l -b -q $rootscript\($params\)
else
# using one "+" only recompiles if script has changed
  root -l -b -q "$rootscript+($params)"
fi
###  Rename plots if they have funny characters in them
for file in *; do
  if [[ "$file" =~ [\#\ \{\}] ]]; then
    newfile=`echo $file | sed 's/[ #{}^]//g'`
    mv "$file" $newfile
  fi
done
