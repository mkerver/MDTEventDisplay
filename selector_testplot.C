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
#include <TCanvas.h>
#include <TH1F.h>
#include <TH2F.h>
//  Put in full directory path for condor jobs
#include "muonfixedid.h"
#include "chamberlist.h"	
// Bookkeeping
int nevents=0;

//Histograms
TList *hlist;
TH1F *raw_adc;
TH1F *raw_tdc;
TH2F *raw_adc_ntube;
TH1F *rawadc_chamber[MAXCHAMBERS];
TH2F *rawMdt_gposX_gposY;
TH2F *rawMdt_gposY_gposZ;
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

//Book histograms
//  histlist = new TList();
  raw_adc = new TH1F("raw_adc","ADC",100,0.,500.);
  raw_tdc = new TH1F("raw_tdc","TDC",100,0.,3000.);
  raw_adc_ntube = new TH2F("raw_adc_ntube","ADC",100,0.,500.,1,0.,MAXCHAMBERS);
  rawMdt_gposX_gposY = new TH2F("rawMdt_gposX_gposY","rawMdt XvY",100,-15,15,100,-15,15);
  rawMdt_gposY_gposZ = new TH2F("rawMdt_gposY_gposZ","rawMdt YvZ",100,-15,15,100,-15,15);
//Loop over chambers
/*  for(int i=0;1<MAXCHAMBERS;i++){
     rawadc_chamber[i] = new TH1F(Form("rawADC_%s",chlist[i].calname),Form("%s raw ADC",chlist[i].calname),100,0,500); //chlist is array in chamberlist that contains detector names e.g BEE_1_2
 //  }

//loop over multilayer and chambers
  for(int j=0; j<MAXCHAMBERS; j++ ) {   //Chambers
    for( int i=0; i<2; i++ ) {           //ML
       int ml = i + 1 ;
       raw_adc[j][i] = 
       raw_tdc[j][i] =          
*/


   
  
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
  char cname[20];
  float  mmToM = 1./1000.;
//  Bookkeeping
  nevents++;

// Get event data - this gets the whole ntuple
  fChain->GetTree()->GetEntry(entry);

if(nevents%500==0){
  printf("Process: EVENT %i MDT segment hits=%i\n",nevents,mdt_nMdt);
 }
//Select the chamber of interest

// printf("number of chambers %i \n", MAXCHAMBERS);  
/* 
 for(int j=0; j<MAXCHAMBERS;j++){

  int nchamber = j;
  int mfid = rawMdt_id[nchamber];
  int ML = MdtML(mfid);
  int layer = MdtML(mfid); 
  int tube  = MdtTube(mfid);

// printf("tube number %i \n", tube);  
}

  for(int i=0; i<rawMdt_nRMdt;i++){ 
    int mfid = rawMdt_id[i];
    ChamberName(mfid,cname);
    int ichamber = checkchamber(cname,1,1,1);
//    raw_adc_ntube->Fill(rawMdt_adc[i],ichamber);
   }
*/

//Fill histograms
  for(int i=0; i<rawMdt_nRMdt;i++){
    int mfid = rawMdt_id[i];
    int ML = MdtML(mfid);
    int layer = MdtLayer(mfid); 
    int tube  = MdtTube(mfid);
    ChamberName(mfid,cname);
    int ichamber = checkchamber(cname,1,1,1);
//    printf("chamber: %i tube: %i layer: %i ML: %i \n",ichamber,tube,layer,ML);
    int phi = StationPhi(mfid);
    int eta = StationEta(mfid); 
    int station = StationID(mfid);
    const char *chambertype = ChamberType(station);
//endcap
//if(station>14 && station <26){
  if(chambertype[0]=='E'){
  //  printf("stationid: %i station: %s \n",station,chambertype);
    rawMdt_gposX_gposY-> Fill(rawMdt_gPosX[i]*mmToM,rawMdt_gPosY[i]*mmToM); 
  }
//barrel
if(chambertype[0]=='B'){
    rawMdt_gposY_gposZ-> Fill(rawMdt_gPosZ[i]*mmToM,rawMdt_gPosY[i]*mmToM); 
  // printf("stationid: %i station: %s \n",station,chambertype);
 } 
//    printf("phi: %i  eta: %i \n",phi,eta);
    raw_adc_ntube->Fill(rawMdt_adc[i],tube);
    if(ML==1 && layer==1 && tube==1){ 
       raw_adc-> Fill(rawMdt_adc[i]);
       raw_tdc-> Fill(rawMdt_tdc[i]);
              
     }
  }




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

//Draw Histograms
/*
  TCanvas *c1 = new TCanvas("c1", "testplots" , 800, 500);
  c1->Divide(2,1);
  
  rawMdt_gposX_gposY->Draw();
  c1->Print("mdt_XvYpos.png");

  c1->cd(1); 
  raw_adc->Draw();
  c1->Print("mdt_rawadc.png");
  c1->cd(2);
  raw_tdc->Draw();
  c1->Print("mdt_rawtdc.png");
  
  raw_adc_ntube->Draw();
  c1->Print("mdt_rawadc_ntube.png");
*/

TCanvas *c1 = new TCanvas("c1", "raw adc",700,900);
TCanvas *c2 = new TCanvas("c2", "xpos v ypos",700, 900);
TCanvas *c3 = new TCanvas("c3", "raw tdc",700, 900);
TCanvas *c4 = new TCanvas("c4", "ypos v zpos",700, 900);
//TPad *pad1 = new TPad("pad1", "raw adc")
//TPad *pad2 = new TPad("pad2", "xpos v ypos");
c1->cd();
raw_adc->Draw();
c2->cd();
raw_tdc->Draw();
c3->cd();
rawMdt_gposX_gposY->Draw();
c4->cd();
rawMdt_gposY_gposZ->Draw();



TFile f("testplots.root","recreate");
raw_adc->Write();
rawMdt_gposX_gposY->Write();
rawMdt_gposY_gposZ->Write();
raw_tdc->Write();
}

// I do not normally use these
void selector::SlaveBegin(TTree * ) {}
void selector::SlaveTerminate() {}
