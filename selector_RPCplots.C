/**********************************************************
   selector_null.C
   This is just an template for a selector function.
   Edward Diehl 8-May-2007, updated 12-Jan-2010
**********************************************************/
#define selector_cxx
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
#include "TMath.h"
#include "selector.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TLine.h"
//  Put in full directory path for condor jobs
#include "muonfixedid.h"
#include "chamberlist.h"
// Bookkeeping
int nevents=0;
//Histograms
TH2F *rpc_gposX_gposZ;
TH2F *rpc_gposY_gposZ;
TH2F *EveYZ;
TH2F *rpcHits_YZ;
TH2F *mdtHits_YZ;
TH2F *trkHits_YZ;
TH1F *mdtHits_width;
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
//Book Histograms
   rpc_gposX_gposZ = new TH2F("rpc_gposX_gposZ" , "RPC XvZ", 100,3,3.3,100, 0,2);
   rpc_gposY_gposZ = new TH2F("rpc_gposY_gposZ" , "RPC YvZ", 100,3,3.5,100, 6,8);
   EveYZ = new TH2F("EveYZ", "EveYvZ", 10, 2.3, 3.7, 100, 6.5, 7.7); 
   rpcHits_YZ = new TH2F("rpcHits_YZ", "hitsYvZ", 100, 2.3, 3.7, 100, 6.5, 7.7); 
   mdtHits_YZ = new TH2F("rpcHits_YZ", "hitsYvZ", 100, 2.3, 3.7, 100, 6.5, 7.7); 
   trkHits_YZ = new TH2F("trkHits_YZ", "hitsYvZ", 100, 2.3, 3.7, 100, 6.5, 7.7); 
   mdtHits_width = new TH1F("mdtHits_width" , "hitsvRoadWidth" , 501, 0.0,0.5 );
}
/***************************************************************/
// The Process() function is called for each entry in the tree (or possibly
/***************************************************************/
Bool_t selector::Process(Long64_t entry) {
//  Bookkeeping
  nevents++;
 // if(trk_nTracks==0) return kTRUE;
if(nevents==616){
//if(event_eventNumber== 651149407 && event_runNumber==299184){ 
// Get event data - this gets the whole ntuple
  fChain->GetTree()->GetEntry(entry);
  float mmToM = 1./1000.;
  int nMdtHits = 0;
  //printf("Process: EVENT %i MDT segment hits=%i\n",nevents,mdt_nMdt);
//Fill XvY and YvZ RPC histo  
/*  for(int i=0;i<trk_nTracks;i++){
    if(1./(trk_qOverP[i])>20000.0 || 1./(trk_qOverP[i])<-20000.0){
      for(int j=0;j<trkHit_nHits;j++){
        int mfid = trkHit_id[j];
        int station = StationID(mfid);
        int eta = StationEta(mfid);
        int sector = StationPhi(mfid);
        const char *chambertype=ChamberType(station);
        if(TechID(mfid)==3 && chambertype[0]=='B' && chambertype[1]=='M' && chambertype[2]=='L' && sector==3 && eta==2){
        //  printf("Event number: %i Track number: %i Hit number: %i station: BML\n",nevents,i,j);
          rpc_gposX_gposZ->Fill(trkHit_posZ[j]*mmToM,trkHit_posX[j]*mmToM);
          rpc_gposY_gposZ->Fill(trkHit_posZ[j]*mmToM,trkHit_posY[j]*mmToM);
        //  printf("x: %f y: %f z: %f \n", trkHit_posX[j], trkHit_posY[j], trkHit_posZ[j]);        
         }
       if(mdt_gPosX[j]<roadwidth/2. && mdt_gPosX[j]>-roadwidth/2. && TechID(mfid)==0 && chambertype[0]=='B' && chambertype[1]=='M' && chambertype[2]=='L' && sector==4){
           nMdtHits++;
         } 
       }
     }
   }
*/ 
// Get event data - this gets just individual item.
//  b_rpc_nRpcHits->GetEntry(entry);


//Event Display
  for(float i= 2.0; i<4.0; i=i+0.001 ){
     for(float j = 6.758; j< 6.842; j=j+0.001){
     EveYZ->Fill(i,j);
     }
   }
  for(float l= 2.0; l<4.0; l=l+0.001 ){
    for(float n = 7.436; n< 7.520; n=n+0.001){
     EveYZ->Fill(l,n);
     }
   }

//Loop over track hits

   int trackhits = 0; 
   int mdtontrk = 0;
   float rpcYmin = 1000.0;
   float rpcYmax = 0.0;
   float rpcZmin = 1000.0;
   float rpcZmax = 0.0;
   int trackindexMax = 0;
   int trackindexMin = 0;

   for(int j=0;j<trkHit_nHits;j++){
      int trackindex = trkHit_trackIndex[j];
      float p = 1.0/trk_qOverP[j];
      if(p>20000.0 || p<-20000.0){
        int mfid = trkHit_id[j];
        int type = trkHit_type[j];
        int author = trk_author[trackindex];
         if(mfid != -1){
	   int station = StationID(mfid);
           int eta = StationEta(mfid);
           int sector = StationPhi(mfid);
           const char *chambertype=ChamberType(station);
     //    printf("event %i  track: %i hit: %i id: %i author: %i type: %i \n",nevents,trackindex,j,mfid,author, type);
           if(chambertype[0]=='B' && chambertype[1]=='M' && chambertype[2]=='L' && sector==3 && eta==2){
             trkHits_YZ->Fill(trkHit_posZ[j]*mmToM,trkHit_posY[j]*mmToM);
             trackhits++;
             if(TechID(mfid)==3){
               rpcHits_YZ->Fill(trkHit_posZ[j]*mmToM,trkHit_posY[j]*mmToM);
  //           printf("event %i  track: %i rpchit: %i  id: %i \n",nevents,trackindex,j,mfid);
              if(trkHit_posY[j]*mmToM>rpcYmax){
                   rpcYmax = trkHit_posY[j]*mmToM;   
                   rpcZmax = trkHit_posZ[j]*mmToM;   
                   trackindexMax = trkHit_trackIndex[j];
                  }
               if(trkHit_posY[j]*mmToM<rpcYmin){            
                   rpcYmin = trkHit_posY[j]*mmToM;   
                   rpcZmin = trkHit_posZ[j]*mmToM;   
                   trackindexMin = trkHit_trackIndex[j];
                  }
                }
            /* for(int m=0; m<rawMdt_nRMdt;m++){
               int rawmfid = rawMdt_id[m];
               if(rawMdt_adc[m]>50){
                 if(rawmfid == mfid){
                  //printf("event %i  track: %i trkhit: %i mdthit: %i id: %i \n",nevents,trackindex,j,m,mfid);
                   int runnumber = event_runNumber;
                   int eventnumber = event_eventNumber;
                   printf("event: %i run: %i  eventnum: %i\n", eventnumber,runnumber,nevents);
                   return kTRUE;
                   mdtontrk++;
                 }
               }
             }*/
           }
         }
       }
     }

//loop over rawmdt hits
  
  for(int m=0; m<rawMdt_nRMdt;m++){
     int rawmfid = rawMdt_id[m];
     if(rawMdt_adc[m]>50){
        mdtHits_YZ->Fill(rawMdt_gPosZ[m]*mmToM, rawMdt_gPosY[m]*mmToM);
      }
    }
// printf("Event: %i track hits: %i mdthits on track: %i\n",nevents,trackhits,mdtontrk);

//Roadwidth

  if(trackindexMin != trackindexMax)cout<< "different tracks!!"<<endl;
  float roadwidth = 0.0;
  float upperbound = 0.0;
  float lowerbound = 0.0;
  while(roadwidth<0.5){
    int mdtHits = 0;
    float trackslope = (rpcYmax-rpcYmin)/(rpcZmax-rpcZmin);
    for(int m=0; m<rawMdt_nRMdt;m++){
      if(rawMdt_adc[m]>50){
        upperbound = (((rawMdt_gPosY[m]*mmToM)-rpcYmin)/trackslope) + rpcZmin + (roadwidth/2.0);  
        lowerbound = (((rawMdt_gPosY[m]*mmToM)-rpcYmin)/trackslope) + rpcZmin - (roadwidth/2.0);  
        if( lowerbound<(rawMdt_gPosZ[m]*mmToM) && (rawMdt_gPosZ[m]*mmToM)<upperbound){
          mdtHits++;
          mdtHits_width->Fill(roadwidth);
        }
      }
    }
roadwidth= roadwidth+ 0.001;
 }
}//closes if(nevents== ) 
return kTRUE;
}

/**************************************************************/
// The Terminate() function is the last function to be called during
/**************************************************************/
void selector::Terminate() {
  printf("Terminate: Processed %i events\n",nevents);
  Float_t RPCbottom = 6.8;
  TCanvas *c1 = new TCanvas("c1","XvZ",700,900);
  TCanvas *c2 = new TCanvas("c2","YvZ",700,900);
  TCanvas *c3 = new TCanvas("c3","EveYvZ",700,900);
  TCanvas *c4 = new TCanvas("c4","mdtRoadWidth",700,900);
  TLine *RPCline = new TLine(-15,RPCbottom,15,RPCbottom);
  TLine *trkline = new TLine(-15,RPCbottom,15,RPCbottom);
  RPCline->SetLineWidth(1);
  RPCline->SetLineColor(kRed);

  c1->cd();
  rpc_gposX_gposZ->Draw();
  c2->cd();
  rpc_gposY_gposZ->Draw();
  c2->Update();
  RPCline->Draw(); 
  c3->cd();
  EveYZ->Draw();
  
  
  EveYZ->SetMarkerColor(21);
  rpcHits_YZ->SetMarkerColor(kRed);
  mdtHits_YZ->SetMarkerColor(kBlue);
  trkHits_YZ->SetMarkerColor(kGreen+1);
  mdtHits_YZ->SetMarkerSize(0.5);
  rpcHits_YZ->SetMarkerSize(.5);
  trkHits_YZ->SetMarkerSize(.8);
  trkHits_YZ->SetMarkerStyle(5);
  mdtHits_YZ->SetMarkerStyle(8);
  rpcHits_YZ->SetMarkerStyle(8);
  mdtHits_YZ->Draw("same");
  trkHits_YZ->Draw("same");
  rpcHits_YZ->Draw("same");
 
  c4->cd();
//  mdtHits_width->SetMinimum(1500);
//  mdtHits_width->SetMaximum(1800);
  mdtHits_width->Draw();
 
  TFile f("RPCplots.root","recreate");
  rpc_gposX_gposZ->Write();
  rpc_gposY_gposZ->Write();
  mdtHits_width->Write();
  c3->Print("canvas.root");
}

// I do not normally use these
void selector::SlaveBegin(TTree * ) {}
void selector::SlaveTerminate() {}
