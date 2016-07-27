/**********************************************************
   selector_null.C
   This is just an template for a selector function.
   Edward Diehl 8-May-2007, updated 12-Jan-2010
**********************************************************/
#define selector_cxx
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
   rpc_gposY_gposZ = new TH2F("rpc_gposY_gposZ" , "RPC YvZ", 100,3,3.5,100, 6,8);

   EveYZ = new TH2F("EveYZ", "EveYvZ", 10, -11.0, 11.0, 100, 6.5, 7.7); 
   rpcHits_YZ = new TH2F("rpcHits_YZ", "hitsYvZ", 1000, -11.0,11.0, 100, 6.5, 7.7); 
   mdtHits_YZ = new TH2F("rpcHits_YZ", "hitsYvZ", 1000, -11.0,11.0, 100, 6.5, 7.7); 
   trkHits_YZ = new TH2F("trkHits_YZ", "hitsYvZ", 1000, -11.0,11.0, 100, 6.5, 7.7); 
   mdtHits_width = new TH1F("mdtHits_width" , "hitsvRoadWidth" , 501, 0.0,0.5 );
}
/***************************************************************/
// The Process() function is called for each entry in the tree (or possibly
/***************************************************************/
Bool_t selector::Process(Long64_t entry) {
//  Bookkeeping
  nevents++;
 // if(trk_nTracks==0) return kTRUE;
if(nevents!=1400) return kTRUE;
  fChain->GetTree()->GetEntry(entry);
  float mmToM = 1./1000.;
  int nMdtHits = 0;

for(int i==0;i<2000;i++){
 
TCanvas *c[i] = new TCanvas[i](sprint("canvas_%d"i),100,-10,10,100,-10,10);

 
 sprint(hname,"hist_%d", i+1);
 histo1[i] = (TH2F*)datafile->Get(hname);

 histo1[i]->Fill(trkHit_posZ[i]*mmToM,trkHit_posY[i]*mmToM);
 histo1[i]->Draw(); 
}
//Event Display
  for(float i= -11.0; i<11.0; i=i+0.01 ){
     for(float j = 6.758; j< 6.842; j=j+0.001){
     EveYZ->Fill(i,j);
     }
   }
  for(float l= -11.0; l<11.0; l=l+0.01 ){
    for(float n = 7.436; n< 7.520; n=n+0.001){
     EveYZ->Fill(l,n);
     }
   }

//Loop over tracks

   int trackhits = 0; 
   int mdtontrk = 0;
   float rpcYmin = 1000.0;
   float rpcYmax = 0.0;
   float rpcZmin = 1000.0;
   float rpcZmax = 0.0;

   for(int i=0;i<trk_nTracks;i++){
      bool bm, bo, bi = false;
      float p = 1.0/trk_qOverP[i];
      int author = trk_author[i];
      if(author == 208){
        if((p>20000.0 || p<-20000.0) && TMath::Abs(p)<1.0e8){
        for(int j=0;j<trkHit_nHits;j++){
          int trackindex = trkHit_trackIndex[j];
          int mfid = trkHit_id[j];
          int type = trkHit_type[j];
	  int station = StationID(mfid);
          int eta = StationEta(mfid);
          int sector = StationPhi(mfid);
          const char *chambertype=ChamberType(station);
          if(trackindex==i){
            if(mfid != -1){
              if(TechID(mfid)==0 && sector==3  && chambertype[0]=='B'){
                 if(chambertype[1]=='M' && eta<6){
                    bm = true;   
                 }
                 else if(chambertype[1]=='O'){
                    bo = true;
                 }
                 else if(chambertype[1]=='I'){
                    bi = true;
                 }
               }
             }
           }
         }
       }
     }

//select good tracks and loop over hits

   if(bi && bo && bm == true){
     // cout<<trk_author[i]<<endl;
     for(int k=0;k<trkHit_nHits;k++){
       int trackindex = trkHit_trackIndex[k];
       int mfid = trkHit_id[k];
       int type = trkHit_type[k];
       int station = StationID(mfid);
       int eta = StationEta(mfid);
       int sector = StationPhi(mfid);
       const char *chambertype=ChamberType(station);
       if(trackindex==i){
         if(chambertype[0]=='B' && chambertype[1]=='M' && chambertype[2]=='L' && sector==3 && eta<6){
           trkHits_YZ->Fill(trkHit_posZ[k]*mmToM,trkHit_posY[k]*mmToM);
           if(trk_author[trackindex]==200) cout<<trk_author[trackindex]<<endl;          
             if(TechID(mfid)==3){
               rpcHits_YZ->Fill(trkHit_posZ[k]*mmToM,trkHit_posY[k]*mmToM);
               cout<<"Y: "<<trkHit_posY[k]<<" z: "<<trkHit_posZ[k]<<" x: "<<trkHit_posX[k]<<" type: "<<trkHit_type[k]<<" resi: "<<trkHit_resi[k]<<endl;
               if(trkHit_posY[k]*mmToM>rpcYmax){
                 rpcYmax = trkHit_posY[k]*mmToM;   
                 rpcZmax = trkHit_posZ[k]*mmToM;   
                }
              if(trkHit_posY[k]*mmToM<rpcYmin){            
                 rpcYmin = trkHit_posY[k]*mmToM;   
                 rpcZmin = trkHit_posZ[k]*mmToM;   
                }
              }
            }
          }
        }
   float roadwidth = 0.0;
   float upperbound = 0.0;
   float lowerbound = 0.0;
   while(roadwidth<0.5){
     float trackslope = (rpcYmax-rpcYmin)/(rpcZmax-rpcZmin);
     for(int m=0; m<trkHit_nHits;m++){
        int mfid = trkHit_id[m];
        if(TechID(mfid)==0 && trkHit_trackIndex[m]==i && trkHit_type[m]!=4){
            upperbound = (((trkHit_posY[m]*mmToM)-rpcYmin)/trackslope) + rpcZmin + (roadwidth/2.0);  
            lowerbound = (((trkHit_posY[m]*mmToM)-rpcYmin)/trackslope) + rpcZmin - (roadwidth/2.0);  
          if( lowerbound<(trkHit_posZ[m]*mmToM) && (trkHit_posZ[m]*mmToM)<upperbound){
            mdtHits_width->Fill(roadwidth);
          }
        }
      }
     roadwidth= roadwidth+ 0.001;
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
 return kTRUE;
}

/**************************************************************/
// The Terminate() function is the last function to be called during
/**************************************************************/
void selector::Terminate() {
  printf("Terminate: Processed %i events\n",nevents);
  TCanvas *c3 = new TCanvas("c3","EveYvZ",700,900);
  TCanvas *c4 = new TCanvas("c4","mdtRoadWidth",700,900);

  c3->cd();
  EveYZ->Draw();
  
  EveYZ->SetMarkerColor(21);
  rpcHits_YZ->SetMarkerColor(kRed);
  mdtHits_YZ->SetMarkerColor(kBlue);
  trkHits_YZ->SetMarkerColor(kGreen+2);
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
  mdtHits_width->Draw();
 
  TFile f("width.root","recreate");
  mdtHits_width->Write();
  c3->Write();
//  c3->Print("canvas2.root"); 

for(int i=0;i<nevents;i++){
  TCanvas


}

}

// I do not normally use these
void selector::SlaveBegin(TTree * ) {}
void selector::SlaveTerminate() {}
