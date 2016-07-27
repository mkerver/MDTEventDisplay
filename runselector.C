/************************************************************

   runselector.C($1,$2,$3)      Edward Diehl 

   Runs a selector.C on a chain of calibration ntuples. 

   Inputs:
    $1 => Name of ntuple file list, one file per line
         default = "file_list.txt" 
    $2 => Number of files to process in list
       = 0 Process all (default)
       < 0 Process only -$2 events from first file (test mode)
    $3 => Option to be passed to selector function.  Meaning depends on
          selector function.

  Interactively run as:
   root> .x runselector.C("file_list",100)

  On command line run as:
    root -l -b -q 'runselector.C("file_list",100)' 

  version  Date        Comment
    1.0    5/2/07
    2.0   29/3/09   Add compile option.  Clean up code
************************************************************/
#include <fstream>
#include <stdio.h>
#include <TFile.h>
#include <TKey.h>
#include <TDirectory.h>
#include <TChain.h>

void runselector( char *filelist="file_list.txt", int maxfiles=0, const char *option="" ) {
  int  nfiles=0,maxevents=0;
  char fname[255];

// Handle arguments
  if( maxfiles < 0 ) {
    maxevents = -maxfiles;
    maxfiles = -1;
  } else if( maxfiles == 0 ) {
    maxfiles = -1;
  }

// Open list of calib ntuples
  ifstream fin(filelist);
  if( !fin || !fin.is_open() ) {
    printf("ERROR: cannot open file list %s\n",filelist);
    exit(1);
  }

//  Open first file on list and determine if ntuple is region ntuple or regular
  fin>>fname;
  TFile *f = new TFile(fname);
  if( f && f->IsZombie() ) {   
    printf("Error with ROOT file %s\n",fname);
    return;
  } 
  printf("Opened ROOT file %s\n",fname);
  TKey *key;
  TIter next(gDirectory->GetListOfKeys());
  while ((key = (TKey*)next())) {
    if( strcmp(key->GetClassName(),"TDirectoryFile")==0 ) {
      if( strcmp(key->GetTitle(),"PatternNtupleMaker") == 0 ) {
        printf("Found normal calib ntuple\n");
      } else {
        printf("Found region ntuple for %s\n",key->GetTitle());
      }
      break;
    }
  }
  TChain* chain = new TChain(Form("%s/Segments",key->GetTitle()));
  f->Close();

//  loop over ntuple files and add to chain
  while( !fin.eof() && nfiles!=maxfiles ) {
    nfiles++;
    chain->Add(fname);
    printf("***** Adding %i %s\n",nfiles,fname);
    fin>>fname; 
  }
  printf("***** Added %i files %ld entries\n",nfiles,(long) chain->GetEntries());

//  Process files - always compile script
  if( maxevents ) chain->Process("selector.C++",option,maxevents);
  else            chain->Process("selector.C++",option);
}
