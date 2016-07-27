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
TH2F *EveYZ;
TH2F *tgcHits_YZ;
TH2F *mdtHits_YZ;
TH2F *trkHits_YZ;
TH1F *trkHits_width;
TH1F *mdtHits_width;
TH1F *trkHits_resi;
TLine *trackline;
/**************************************************************/
void selector::Begin(TTree *) {
  TString option = GetOption();
  printf("Begin: The option is %s\n",option.Data());
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
  if( option == "-h" ) {
    printf("selector_null: sample selector function\n");
    printf("    Edward Diehl  3/29/09\n");
    exit(0);    //Abort job
  }
//Book Histograms
   EveYZ = new TH2F("EveYZ", "EveYvZ",100 , 11.0,16.0, 10, 1.0, 11.0);
   tgcHits_YZ = new TH2F("tgcHits_YZ", "hitsYvZ", 1000, 11.0,16.0, 100, 1.0, 11.0); 
   mdtHits_YZ = new TH2F("mdtHits_YZ", "hitsYvZ", 1000, 11.0,16.0, 100, 1.0, 11.0); 
   trkHits_YZ = new TH2F("trkHits_YZ", "hitsYvZ", 1000, 11.0,16.0, 100, 1.0, 11.0); 
   mdtHits_width = new TH1F("mdtHits_width" , "RoadWidth" , 501, 0.0,0.5 );
   trkHits_width = new TH1F("trkHits_width" , "RoadWidth" , 501, 0.0,0.5 );
   trkHits_resi = new TH1F("trkHits_resi" , "trkHits_resi" , 100, -1,1 );
}
/***************************************************************/
// The Process() function is called for each entry in the tree (or possibly
/***************************************************************/
Bool_t selector::Process(Long64_t entry) {
//  Bookkeeping
  nevents++;
 // if(trk_nTracks==0) return kTRUE;
 if(nevents!=624)return kTRUE;
  fChain->GetTree()->GetEntry(entry);
  float mmToM = 1./1000.;
  int nMdtHits = 0;
//cout<<nevents<<endl;
//Event Display
  for(float i= 13.30; i<13.55; i=i+0.001 ){
     for(float j = 1.0; j< 15.0; j=j+0.01){
     EveYZ->Fill(i,j);
     }
   }
  for(float l= 14.63; l<14.83; l=l+0.001 ){
    for(float n = 1.0; n< 15.0; n=n+0.01){
     EveYZ->Fill(l,n);
     }
   }
  for(float m= 15.05;m<15.25; m=m+0.001 ){
    for(float k = 1.0; k< 15.0; k=k+0.01){
     EveYZ->Fill(m,k);
     }
   }


//Loop over tracks

   int nRPCupper = 0;
   int nRPClower = 0;
   int trackhits = 0; 
   int mdtontrk = 0;
   float rpcYmin = 0.0;
   float rpcYmax = 0.0;
   float rpcZmin = 0.0;
   float rpcZmax = 0.0;
   float theta = 3.14159/6.0;
   for(int i=0;i<trk_nTracks;i++){
      bool bm = false;
      bool bo = false;
      bool bi = false;
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
              if(TechID(mfid)==2  && chambertype[0]=='T' && trkHit_posZ[j]*mmToM>12.0 && chambertype[2]=='E' && 1.9<trkHit_posY[j]*mmToM<12.0 &&  TMath::Abs(trkHit_posX[j])<TMath::Sin(theta)*trkHit_posY[j]){
//cout<<"here!"<<endl;

//cout<<bm<<" "<<bi<<" "<<bo<<endl;
                if(chambertype[1]=='2' && eta<6){
                    bm = true;
                 }
                 else if(chambertype[1]=='3'){
                    bo = true;
                 }
                 else if(chambertype[1]=='1'){
                    bi = true;
                 }
               }
             }
           }
         }
       }
     }

//select good tracks and loop over hits

//cout<<bm<<" "<<bi<<" "<<bo<<endl;
   if(bi==true && bo==true && bm == true){
//cout<<nevents<<endl;
//return kTRUE;
//cout<<"phi: "<<trk_phi[i]<<" eta:"<<(-0.5*TMath::ln(TMath::Tan(trk_theta[i])))<<" p:"<<p<<endl;
     for(int k=0;k<trkHit_nHits;k++){
       int trackindex = trkHit_trackIndex[k];
       int mfid = trkHit_id[k];
       int type = trkHit_type[k];
       int station = StationID(mfid);
       int eta = StationEta(mfid);
       int sector = StationPhi(mfid);
       const char *chambertype=ChamberType(station);
       if(trackindex==i){
       if(trkHit_posZ[k]*mmToM>12.0 && 1.9<trkHit_posY[k]*mmToM<12.0 &&  TMath::Abs(trkHit_posX[k]*mmToM)<(TMath::Sin(theta)*trkHit_posY[k]*mmToM)){
//    cout<<"Tech ID: "<<TechID(mfid)<<" Phi: "<<sector<<" Eta: "<<eta<<" track type:"<<trkHit_type[k]<<" z: "<<trkHit_posZ[k]*mmToM<<" Y: "<<trkHit_posY[k]*mmToM<<" x: "<<trkHit_posX[k]*mmToM<<endl;
//cout<<cTechID(mfid,ctech)<<endl;  
cout<<chambertype<<"   "<<station<<endl;
           trkHits_YZ->Fill(trkHit_posZ[k]*mmToM,trkHit_posY[k]*mmToM);
             if(TechID(mfid)==2){
               tgcHits_YZ->Fill(trkHit_posZ[k]*mmToM,trkHit_posY[k]*mmToM);
               if(trkHit_posZ[k]*mmToM>15.0){
                 rpcYmax = rpcYmax+trkHit_posY[k]*mmToM;   
                 rpcZmax = rpcZmax+trkHit_posZ[k]*mmToM;   
                 nRPCupper++;
                }
              if(trkHit_posZ[k]*mmToM<13.6){            
                 rpcYmin = rpcYmin+trkHit_posY[k]*mmToM;   
                 rpcZmin = rpcZmin+trkHit_posZ[k]*mmToM;   
                 nRPClower++;
                }
              }
            }
          }
        }
   float roadwidth = 0.0;
   float upperbound = 0.0;
   float lowerbound = 0.0;
   rpcYmin = rpcYmin/nRPClower;   
   rpcYmax = rpcYmax/nRPCupper;   
   rpcZmin = rpcZmin/nRPClower;   
   rpcZmax = rpcZmax/nRPCupper;   
   while(roadwidth<0.5){
     float trackslope = (rpcYmax-rpcYmin)/(rpcZmax-rpcZmin);
     for(int m=0; m<trkHit_nHits;m++){
        int mfid = trkHit_id[m];
        if((TechID(mfid)==0 || TechID(mfid)==3) && trkHit_trackIndex[m]==i && trkHit_type[m]!=4 && trkHit_posZ[m]*mmToM>10.0 && trkHit_posZ[m]*mmToM<17.0){
          upperbound = ((trkHit_posZ[m]*mmToM-rpcZmin)*trackslope) + roadwidth + rpcYmin;
//if(TechID(mfid)==0 && roadwidth>0.4997) cout<<trkHit_type[i]<<endl;
          lowerbound = ((trkHit_posZ[m]*mmToM-rpcZmin)*trackslope) - roadwidth + rpcYmin;
          if(roadwidth>=0.499)trkHits_resi->Fill(trkHit_resi[m]);
          if(lowerbound<(trkHit_posY[m]*mmToM) && (trkHit_posY[m]*mmToM)<upperbound){
             trkHits_width->Fill(roadwidth);
   //   if(roadwidth>0.49) cout<<"y: "<<trkHit_posY[m]<<" z: "<<trkHit_posZ[m]<<" lower:" <<lowerbound<<" upper: "<<upperbound<<endl;
            }
          }
        }
       for(int n=0; n<rawMdt_nRMdt;n++){
         if(rawMdt_adc[n]>0){
           upperbound = ((rawMdt_gPosZ[n]*mmToM-rpcZmin)*trackslope) + roadwidth + rpcYmin;
           lowerbound = ((rawMdt_gPosZ[n]*mmToM-rpcZmin)*trackslope) - roadwidth + rpcYmin;
           if(lowerbound<(rawMdt_gPosY[n]*mmToM) && (rawMdt_gPosY[n]*mmToM)<upperbound){
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
//chambertype[1] = layers in z-direction i.e. 1,2, or 3

//if(nevents%500==0)
 trackline = new TLine(rpcZmin,rpcYmin,rpcZmax,rpcYmax);
 trackline->SetLineColor(kRed);
  return kTRUE;
}

/**************************************************************/
// The Terminate() function is the last function to be called during
/**************************************************************/
void selector::Terminate() {
  printf("Terminate: Processed %i events\n",nevents);
  TCanvas *c1 = new TCanvas("c1","EveYvZ",700,900);
  TCanvas *c2 = new TCanvas("c2","mdtRoadWidth",700,900);
  TCanvas *c3 = new TCanvas("c3","trkHits residual",700,900);
  TCanvas *c4 = new TCanvas("c4","trkRoadWidth",700,900);


  c1->cd();
  EveYZ->Draw();
  trackline->Draw();
  
  EveYZ->SetMarkerColor(21);
  tgcHits_YZ->SetMarkerColor(kRed);
  mdtHits_YZ->SetMarkerColor(kBlue);
  trkHits_YZ->SetMarkerColor(kGreen+1);
  mdtHits_YZ->SetMarkerSize(0.5);
  tgcHits_YZ->SetMarkerSize(.5);
  trkHits_YZ->SetMarkerSize(.8);
  trkHits_YZ->SetMarkerStyle(5);
  mdtHits_YZ->SetMarkerStyle(8);
  tgcHits_YZ->SetMarkerStyle(8);
  mdtHits_YZ->Draw("same");
  trkHits_YZ->Draw("same");
  tgcHits_YZ->Draw("same");
 
  c2->cd();
  trkHits_width->Draw();
  mdtHits_width->SetLineColor(kRed);
  mdtHits_width->Draw("same");
  trkHits_width->SetMaximum(150);
  mdtHits_width->SetMaximum(150);
  c2->Update();

  c3->cd();
  trkHits_resi->Draw();
  
  c4->cd();
  trkHits_width->Draw();
 
  TFile f("TGCplots.root","recreate");
 // mdtHits_width->Write();
  trkHits_resi->Write();
  c1->Write();
  c2->Write(); 
  c4->Write(); 
}

// I do not normally use these
void selector::SlaveBegin(TTree * ) {}
void selector::SlaveTerminate() {}
