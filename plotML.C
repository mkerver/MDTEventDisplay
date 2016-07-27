//***********************************************************/
//   Make plots of ML data 
//   using the ML.root file made by selector_ML.C
//   
//   plotML( const char *fname, const char *chamber )
//  fname = name of a root file to process from selector_ML.C
//  chamber  = chamber to fit (hardware name) [optional, default is fit all chambers]
//
//***********************************************************/
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TFile.h>
#include <TDirectory.h>
#include <TH1.h>
#include <TH1F.h>
#include <TH2.h>
#include <TProfile.h>
#include <TMath.h>
#include <TPaveText.h>
#include <TPad.h>
#include <TF1.h>
#include <TGaxis.h>
#include <TGraphErrors.h>
#include <TPaveStats.h>

#include "plotresi.h"

//***********************************************************/
// Make raw plots and fit residual plots from calib ntuples 
//  fname = list of root files to process (one file name per line)
//          OR name of a root file to process
//************************************************************/
void plotML( const char *fname, const char *chamber='\0' ) {
  Double_t values1[VALUESMAX],values2[VALUESMAX];
  Double_t params[2]={1.,5.};   //for DrawResi [0]=scale [1]=number of RMSs

//  Set global style options
  gStyle->SetStatX(0.97);      //set statbox x location
  //  gStyle->SetStatY(1.0);   //set statbox y location
  gStyle->SetStatW(0.3);       //set statbox width
  gStyle->SetStatH(0.2);       //set statbox height - fonts scale with size
  gStyle->SetOptStat(1110);    //set control flag for stat display
  gStyle->SetOptTitle(1);      //turn off histogram titles
  gStyle->SetTitleH(0.05);     //set histo title height
  gStyle->SetLabelSize(0.05, "X");
  gStyle->SetLabelSize(0.05, "Y");
  gStyle->SetTitleXSize(0.05);
  gStyle->SetTitleYSize(0.05);

// Extract filename without .root extension
  int ii=0;
  for( ; ii<254; ii++ ) {
    if( fname[ii] != '.' && fname[ii] != '\0' ) {
      filename[ii] = fname[ii];
    } else {
      filename[ii] = '\0';
      break;
    }
  }
// Remove _ from filename to use for plot names
  for( ii=0; filename[ii]!='\0'; ii++ ) {
    if( filename[ii] == '_' ) plotname[ii] = ' ';
    else                      plotname[ii] = filename[ii];                 
  }
  plotname[ii]='\0';

// Reset chamber loop limits if chamber specified
  int imin=0, imax=MAXCHAMBERS;
  if( chamber != '\0' && chamber[0] != 'A' ) {
    int idx = MDTindex( chamber );
    if( idx == -1 ) {
      printf("ERROR: Unknown chamber %s\n",chamber);
      return;
    } 
    imin = idx;
    imax = idx+1;
  }

//  Open ROOT file
  TFile *f = new TFile(fname);
  if( f && f->IsZombie() ) {   
    printf("Error with ROOT file %s\n",fname);
    f->Close();
    return;
  } 
  printf("Opened %s (%s)\n",fname,filename);

//  Set global style options
  gStyle->SetStatX(0.94);      //set statbox x location
  gStyle->SetStatY(0.9);       //set statbox x location
  gStyle->SetStatW(0.3);       //set statbox width
  gStyle->SetStatH(0.2);       //set statbox height - fonts scale with size
  gStyle->SetTitleX(0.5);
  gStyle->SetTitleY(1.05);
  gStyle->SetTitleH(0.2);
  gStyle->SetTitleW(0.95);

// Open stats files, write headers
  std::ofstream foutadc, foutres;
  if( chamber != '\0' ) {
    foutadc.open(Form("%s_ADC_%s_ML.txt",filename,chamber));
    foutres.open(Form("%s_Segment_Residuals_%s_ML.txt",filename,chamber));
  } else {
    foutadc.open(Form("%s_ADC_ML.txt",filename));
    foutres.open(Form("%s_Segment_Residuals_ML.txt",filename));
  }
  if( foutadc.is_open()==0 || foutres.is_open()==0 ) {
    printf("ERROR: cannot open stats files\n");
    return; 
  } 
  WriteADCHeader( foutadc );
  WriteResiHeader( foutres );

  gStyle->SetOptStat(0);
  TCanvas *c1 = new TCanvas("c1","MLplot",800,600);
  c1->SetTicks(0,1);

// Print chamber tube hit maps
  for( int j=imin; j<imax; j++ ) { 
    f->cd("/");
    char cham[20];
    sprintf(cham,"%s",chlist[j].hardname);

    // Check that chamber directory exists before cd'ing to it
    TDirectoryFile* tdir = (TDirectoryFile*) f->Get(cham);
    if( tdir == 0 ) {
      printf("No directory for %s\n",cham);
      continue;
    }
    tdir->cd();

    DrawResiML(Form("%s_ML1_SegRes",cham),Form("%s_ML2_SegRes",cham),values1,values2,params,Form("%s #color[4]{ML1}+#color[2]{ML2} Segment Residuals",cham),Form("%s_ML_SegRes",cham));
    if( values1[0] > 0. ) {   //skip empty ML
      foutres << Form("%s ML1",chlist[j].hardname);
      for( int k=0; k<15; k++ ) foutres << " " << values1[k];
      foutres << "\n";
    }
    if( values2[0] > 0. ) {   //skip empty ML
      foutres << Form("%s ML2",chlist[j].hardname);
      for( int k=0; k<15; k++ ) foutres << " " << values2[k];
      foutres << "\n";
    }

    gStyle->SetOptStat("eMR"); 
    Double_t rawdata[6]={0.,0.,0.,0.,0.,0.}, segdata[6]={0.,0.,0.,0.,0.,0.},raw80data[6]={0.,0.,0.,0.,0.,0.};
    Drawh1h1(Form("%s_ML1_rawADC",cham),Form("%s_ML2_rawADC",cham),rawdata,Form("%s ML1+#color[2]{ML2} Raw ADC ",cham),Form("%s_ML_RawADC",cham),1);
    Drawh1h1(Form("%s_ML1_rawADCcut80",cham),Form("%s_ML2_rawADCcut80",cham),raw80data,Form("%s ML1+#color[2]{ML2} Raw ADC>80",cham),Form("%s_ML_RawADC80",cham),1);
    Drawh1h1(Form("%s_ML1_segADC",cham),Form("%s_ML2_segADC",cham),segdata,Form("%s ML1+#color[2]{ML2} Segment ADC",cham),Form("%s_ML_SegADC",cham),1);
// Only write ADC info for chambers with hits
    if( rawdata[0] > 0. ) {       
      foutadc << Form("%s ML1",cham);
      for( int k=0; k<3; k++ ) foutadc << " " << rawdata[k];
      for( int k=0; k<3; k++ ) foutadc << " " << segdata[k];
      for( int k=0; k<3; k++ ) foutadc << " " << raw80data[k];
      foutadc << " " << 100*(rawdata[0]-raw80data[0])/rawdata[0];
    }
    if( rawdata[3] > 0. ) {       
      foutadc << Form("\n%s ML2",cham);
      for( int k=3; k<6; k++ ) foutadc << " " << rawdata[k];
      for( int k=3; k<6; k++ ) foutadc << " " << segdata[k];
      for( int k=3; k<6; k++ ) foutadc << " " << raw80data[k];
      foutadc << " " << 100*(rawdata[3]-raw80data[3])/rawdata[3];
      foutadc << "\n";
    } 
    Drawh1h1(Form("%s_ML1_rawTDC",cham),Form("%s_ML2_rawTDC",cham),raw80data,Form("%s ML1+#color[2]{ML2} Raw TDC",cham),Form("%s_ML_RawTDC",cham),1);
    Drawh1h1(Form("%s_ML1_segTDC",cham),Form("%s_ML2_segTDC",cham),segdata,Form("%s ML1+#color[2]{ML2} Segment TDC",cham),Form("%s_ML_SegTDC",cham),1);
    gStyle->SetOptStat(10);       //set control flag for stat display
    Drawh1h1(Form("%s_ML1_DriftRadius",cham),Form("%s_ML2_DriftRadius",cham),segdata,Form("%s ML1+#color[2]{ML2} Drift Radius",cham),Form("%s_ML_DriftRadius",cham),1);
    Drawh2(Form("%s_ML1_rawADCTDC",cham),"col");
    Drawh2(Form("%s_ML2_rawADCTDC",cham),"col");
  }   //end loop over chambers

  foutadc.close();
  foutres.close();
  f->Close();
}  //end plotML
