/**********************************************************
   selector_null.C

   This is just an template for a selector function.

   Edward Diehl 8-May-2007, updated 12-Jan-2010
**********************************************************/
#define selector_cxx
// The class definition in selector.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.

// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// Root > T->Process("selector.C")
// Root > T->Process("selector.C","some options")
// Root > T->Process("selector.C+")
//
#include <stdio.h>
#include <TObjArray.h>
#include "selector.h"
//  Put in full directory path for condor jobs
#include "/atlas/data18c/muoncal/root/macros/muonfixedid.h"

// Bookkeeping
int nevents=0;

/**************************************************************/
// The Begin() function is called at the start of the query.
// When running with PROOF Begin() is only called on the client.
// The tree argument is deprecated (on PROOF 0 is passed).
// E.D.: Book histograms etc
/**************************************************************/
void selector::Begin(TTree *) {
//  Can pass a string option which is 4th argument of runselector.C
//  You can retrieve the option in any function in this file.
  TString option = GetOption();
  printf("Begin: The option is %s\n",option.Data());
//  This code parses the options and finds options separated by white space
  TObjArray *Strings = option.Tokenize(" "); 
  if(Strings->GetEntriesFast()) {
    TIter iString(Strings);
    TObjString* os=0;
    Int_t j=0;
    while ((os=(TObjString*)iString())) {
      printf("token #%d ='%s'\n",j++,  os->GetString().Data());
    }
  }
  delete Strings;

//  Print help message to users
  if( option == "-h" ) {
    printf("selector_null: sample selector function\n");
    printf("    Edward Diehl  3/29/09\n");
    exit(0);    //Abort job
  }  
}

/***************************************************************/
// The Process() function is called for each entry in the tree (or possibly
// keyed object in the case of PROOF) to be processed. The entry argument
// specifies which entry in the currently loaded tree is to be processed.
// It can be passed to either selector::GetEntry() or TBranch::GetEntry()
// to read either all or the required parts of the data. When processing
// keyed objects with PROOF, the object is already loaded and is available
// via the fObject pointer.
//
// This function should contain the "body" of the analysis. It can contain
// simple or elaborate selection criteria, run algorithms on the data
// of the event and typically fill histograms.
//
// The processing can be stopped by calling Abort().  E.D: Abort() does not
// seem to work, but exit(0) does.
//
// Use fStatus to set the return value of TTree::Process().
// The return value is currently not used.  
// (E.D: so whats the point of settings its status?)
/***************************************************************/
Bool_t selector::Process(Long64_t entry) {
//  Bookkeeping
  nevents++;

// Get event data - this gets the whole ntuple
  fChain->GetTree()->GetEntry(entry);

  printf("Process: EVENT %i MDT segment hits=%i\n",nevents,mdt_nMdt);

// Get event data - this gets just individual item.
//  b_rpc_nRpcHits->GetEntry(entry);

  return kTRUE;
}

/**************************************************************/
// The Terminate() function is the last function to be called during
// a query. It always runs on the client, it can be used to present
// the results graphically or save the results to file.
/**************************************************************/
void selector::Terminate() {
  printf("Terminate: Processed %i events\n",nevents);
}

// I do not normally use these
void selector::SlaveBegin(TTree * ) {}
void selector::SlaveTerminate() {}
