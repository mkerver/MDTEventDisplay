//***********************************************************/
//   Make fit residual & T0Fit plots from calib ntuples
//   using the resi.root file made by selector_resi.C
//   
//   plotresi( const char *fname, int iflag=0 )
//  fname = name of a root file to process from selector_resi.C
//  iflag = 0 Make basic plots (default)
//          1 => Make chamberlevel T0fit and segment/track residual plots
//          2 => Make chamberlevel raw ADC, TDC plots
//  chamber  = chamber (hardware name) or region to fit [optional, default is fit all chambers]
//           Rx => Select all chambers in splitter region x
//
//   After this run sortresiplots.sh => Sort plots made by plotresi.C
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
#include "edplot.h"
#include "chamberlist.h"
#include "chamberwheel.h"
#include "splitter_regions.h"

///const char chend[4]="png";
//char filename[255],plotname[255];

//  Canvases for plots
TCanvas *c1, *c2, *c3;

//***********************************************************/
// Make raw plots and fit residual plots from calib ntuples 
//  fname = list of root files to process (one file name per line)
//          OR name of a root file to process
//************************************************************/
void plotresi( const char *fname, int iflag=0, const char *chamber='\0' ) {
  Double_t values[VALUESMAX],values2[VALUESMAX];
  Double_t xpos[12]={0.5,1.5,2.5,3.5,4.5,5.5,6.5,7.5,8.5,9.5,10.5,11.5}, xpose[12]={12*0.};
  Double_t ypos[12], ypose[12];
  Double_t params[2]={1.,5.};   //for DrawResi [0]=scale [1]=number of RMSs
  TH1F *h;
  int  ii;

//   gROOT->LoadMacro("/afs/atlas.umich.edu/home/muoncal/root/macros/AtlasStyle.C");
//   SetAtlasStyle();

// Reset chamber loop limits if chamber specified
  int imin=0, imax=MAXCHAMBERS, region=0;
  if( chamber[0] == 'R' ) {
    region = atoi(&chamber[1]);     
  } else if( chamber != '\0' ) {
    int idx = MDTindex( chamber );
    if( idx == -1 ) {
      printf("ERROR: Unknown chamber %s\n",chamber);
      return;
    } else {
      imin = idx;
      imax = idx+1;
    }
  }


//  Book canvases
  c1 = new TCanvas("c1","resiplot",600,400);
  c1->SetGrid();
  c1->SetTicks(1,1);
  c1->SetRightMargin(0.03);
  c2 = new TCanvas("c2","resiplot2",1400,400);
  c2->SetGrid();
  c2->SetTicks(1,1);
  // For ADC/TDC plots
  c3 = new TCanvas("c3","ADCplot",600,400);
  c3->SetTicks(1,0);
  c3->SetRightMargin(0.06);

  c1->cd();

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
  for( ii=0; ii<254; ii++ ) {
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
    else                     plotname[ii] = filename[ii];                 
  }
  plotname[ii]='\0';

//  Open ROOT file
  TFile *f = new TFile(fname);
  if( f && f->IsZombie() ) {   
    printf("Error with ROOT file %s\n",fname);
    f->Close();
    return;
  } 
  printf("Opened %s (%s)\n",fname,filename);

//  Plot various histograms
  TGraphErrors *tzwheel,*resiwheel;

  char cauthor[2][20] = { "Standalone", "Combined" };
  char htype  [4][10] = { "all", "best", "T0fit", "noT0fit" };
  char ebtype [3][10] = { "barrel", "endcap", "all" };

// Figure out what track was used in the plots
  f->cd("/Tracks");
  int iauthors[3] = {206,200,0};  //Run2 Tier0, Run2 calstream, Run1
  for( ii=0; ii<3; ii++ ) {
    TH1 *h1 = (TH1 *) gDirectory->Get(Form("TrkNtracksTrk%i",iauthors[ii]));
    if( h1 ) {
      sprintf(cauthor[0],Form("Trk%i",iauthors[ii]));
      break;
    }
  }
  if( ii == 3 ) {
    printf("Could not determine track author\n!");
    return;
  }

// ADC, TDC plots
  if( iflag == 2 ) {
    Double_t data[6]={0.,0.,0.,0.,0.,0.}, data2[6]={0.,0.,0.,0.,0.,0.};

//  Set global style options
    gStyle->SetStatX(0.99);      //set statbox x location
    gStyle->SetStatW(0.35);      //set statbox width
    gStyle->SetStatH(0.2);       //set statbox height - fonts scale with size
    gStyle->SetTitleX(0.5);
    gStyle->SetTitleY(1.05);
    gStyle->SetTitleH(0.2);
    gStyle->SetTitleW(0.95);

    f->cd("/Raw");

    std::ofstream foutadc;
// Open stats files, write headers
    foutadc.open(Form("%s_ADC.txt",filename));
    if( foutadc.is_open() == 0 ) {
      printf("ERROR: cannot open ADCTDC stats file\n");
      return; 
    } 
    WriteADCHeader( foutadc );
    gStyle->SetOptStat(0);
// Print chamber tube hit maps
    c2 = new TCanvas("c2","resiplot2",800,600);
    for( int j=imin; j<imax; j++ ) { 
//  If only doing a specific region check if chamber is in it.
      if( region && chamber_regions[j] != region ) continue;

      f->cd(Form("/Chamber/%s",chlist[j].hardname));
      Drawh2(Form("SegHitOcc_%s"     ,chlist[j].hardname),"colz");
      Drawh2(Form("RawHitOcc_%s"     ,chlist[j].hardname),"colz");
      //      Drawh2(Form("RawHitOcccut50_%s",chlist[j].hardname),"colz");
    }
    // Do not do this if have selected individual chambers
    if( imax == MAXCHAMBERS ) {
    c2 = new TCanvas("c2","resiplot2",800,800);
    c2->SetGrid();
    c2->SetTicks(1,1);
    c2->SetLogz(1);
    for( int j=0; j<12; j++ ) {
      f->cd("/Raw");
      gStyle->SetOptStat("eMR"); 
      Drawh1h1(Form("rawADC_%s" ,chwheel[j]),Form("ADC_%s",chwheel[j]),data,Form("Raw and Segment ADC %s",chwheel[j]));
      gStyle->SetOptStat(10);       //set control flag for stat display
      TH1 *h1 = (TH1 *) gDirectory->Get(Form("rawTDC_%s",chwheel[j]));
      if( h1 == 0 ) {
         printf("ERROR: No histogram %s\n",Form("rawTDC_%s",chwheel[j]));
      } else {
        TH1 *h2 = (TH1 *) gDirectory->Get(Form("rawTDCcut50_%s",chwheel[j]));
        f->cd("/Segments");
        c1->cd();
        Drawh1h1h1_(h1,h2,Form("TDC_%s",chwheel[j]),data,Form("All/ADC>50/Segment TDC %s",chwheel[j]));
        c2->cd();
        Drawh2(Form("ADCTDC_%s",chwheel[j]),"col");
      }
      //      Drawh2(Form("rawADCTDC_%s",chwheel[j]),"colz"); 
    }
    for( int j=0; j<3; j++ ) {
      gStyle->SetOptStat("eMR"); 
      c1->cd();
      Drawh1h1(Form("rawADC_%s",ebtype[j]),Form("ADC_%s",ebtype[j]),data,Form("Raw and Segment ADC %s",ebtype[j]));
      c2->cd();
      Drawh2(Form("ADCTDC_%s",ebtype[j]),"col");
      //      Drawh2(Form("rawADCTDC_%s",ebtype[j]),"colz");
    }
    }   //end not doing individual chambers

    for( int j=imin; j<imax; j++ ) { 
//  If only doing a specific region check if chamber is in it.
      if( region && chamber_regions[j] != region ) continue;
      f->cd(Form("/Chamber/%s",chlist[j].hardname));
      gStyle->SetOptStat("eMR"); 
      gStyle->SetStatH(0.2);       //set statbox height - fonts scale with size
      
      c1->cd();
      Drawh1h1(Form("rawADC_%s",chlist[j].hardname),Form("ADC_%s",chlist[j].hardname),data,Form("Raw and Segment ADC %s",chlist[j].hardname));
// Only write ADC info for chambers with hits
      if( data[0] > 0. ) {       
        foutadc << chlist[j].hardname << " CHM";
	for( int k=0; k<6; k++ ) foutadc << " " << data[k];
	for( int k=0; k<3; k++ ) foutadc << " " << data2[k];
	foutadc << "\n";
      } 
      gStyle->SetStatH(0.3);       //set statbox height - fonts scale with size
      gStyle->SetOptStat(10);       //set control flag for stat display
      //      Drawh1h1h1(Form("rawTDC_%s",chlist[j].hardname),Form("rawTDCcut50_%s",chlist[j].hardname),Form("TDC_%s",chlist[j].hardname),data,Form("All/ADC>50/Segment TDC %s",chlist[j].hardname));
      Drawh1h1(Form("rawTDC_%s",chlist[j].hardname),Form("TDC_%s",chlist[j].hardname),data,Form("Raw and Segment TDC %s",chlist[j].hardname));
      c2->cd();
      Drawh2(Form("ADCTDC_%s",chlist[j].hardname),"col");
      //      Drawh2(Form("rawADCTDC_%s",chlist[j].hardname),"colz");
    }
    c2->SetLogz(0);
    foutadc.close();
    return;
  }

//  Make colored background histograms
//     TCanvas *tzcan = new TCanvas("tzcan","tzcan", 700,500);
//     for( int iw=0; iw<12; iw++ ) { 
//       TH1F *h1 = (TH1F *) gDirectory->Get(Form("TOFpr%s_%s",cauthor[0],chwheel[iw]));
//       TH1F *h2 = (TH1F *) gDirectory->Get(Form("TOFpr%s_%s",cauthor[1],chwheel[iw]));
//       makebackground(tzcan,iw,h1,h2,0.,90.);
//       h1 = (TH1F *) gDirectory->Get(Form("SegPullPr%s_%s",cauthor[0],chwheel[iw]));
//       h2 = (TH1F *) gDirectory->Get(Form("SegPullPr%s_%s",cauthor[1],chwheel[iw]));
//       makebackground(tzcan,iw,h1,h2,-1.,1.);
//     }
    //    return;
//  Open Standalone & Combined stats files
  if( iflag == 1 ) {
    std::ofstream fout[2],foutrk[2],fout0fit[2];
// Open stats files, write headers
    for( int i=0; i<1; i++ ) {   //ONLY STANDALONE for M7
      //      fout[i].open(Form("%s_%s.txt",filename,cauthor[i]));
      fout[i].open(Form("%s_Segment_Residuals.txt",filename));    //cauthor[i]
      if( fout[i].is_open() == 0 ) {
	printf("ERROR: cannot open Segment stats file\n");
	return; 
      } 
      foutrk[i].open(Form("%s_Track_Residuals.txt",filename));    //cauthor[i]));
      if( foutrk[i].is_open() == 0 ) {
 	printf("ERROR: cannot open Track stats file\n");
 	return; 
      } 
      fout0fit[i].open(Form("%s_T0Fit.txt",filename));
      if( fout0fit[i].is_open() == 0 ) {
	printf("ERROR: cannot open T0Fit stats file\n");
	return; 
      } 
      WriteResiHeader( fout[i] );
      WriteResiHeader( foutrk[i] ); 
      WriteT0FitHeader( fout0fit[i] ); 
    }

// Loop over Standalone, Combined
    char hname[80] = "";
    for( int i=0; i<1; i++ ) {   //ONLY STANDALONE for M7

      for( int j=imin; j<imax; j++ ) { 
//  If only doing a specific region check if chamber is in it.
	if( region && chamber_regions[j] != region ) continue;
        f->cd(Form("/Chamber/%s",chlist[j].hardname));
// Segment residuals
        sprintf(hname,"SegRes_%s",chlist[j].hardname);
	h = (TH1F *) gDirectory->Get(hname);
	if( h != NULL) {
	  c1->cd();
	  DrawResi(h,values2,params,filename);
          if( values2[0] > 0. ) {   //skip empty chambers
  	    fout[i] << chlist[j].hardname << " CHM";
	    for( int k=0; k<15; k++ ) fout[i] << " " << values2[k];
	    fout[i] << "\n";
	  }
	} else {
          printf("ERROR: could not read histo %s\n",hname);
	}
// T0fits
        sprintf(hname,"T0fit_%s",chlist[j].hardname);
	h = (TH1F *) gDirectory->Get(hname);
	if( h != NULL) {
	  c1->cd();
	  DrawGaus(h,values,filename);
          if( values[0] > 0. ) {   //skip empty chambers
	    fout0fit[i] << chlist[j].hardname << " CHM";
	    for( int k=0; k<8; k++ ) fout0fit[i] << " " << values[k];
	    fout0fit[i] << "\n";
	  }
	} else {
          printf("ERROR: could not read histo %s\n",hname);
	}
// Trk residuals
        sprintf(hname,"TrkHitResi%s_%s",cauthor[i],chlist[j].hardname);
 	h = (TH1F *) gDirectory->Get(hname);
 	if( h != NULL) {
	  c1->cd();
 	  DrawResi(h,values2,params,filename);
          if( values2[0] > 0. ) {   //skip empty chambers
	    foutrk[i] << chlist[j].hardname << " CHM";
	    for( int k=0; k<15; k++ ) foutrk[i] << " " << values2[k];
	    foutrk[i] << "\n";
	  }
	} else {
          printf("ERROR: could not read histo %s\n",hname);
 	}
//	if( i ) Drawh1(Form("TDC_%s",chlist[j].hardname));
// check for bad chambers 
// 	if( TMath::Abs(values[1]) > 15. || TMath::Abs(values[3]) > 10. || values[4] > 15. ) {
// 	  printf("BADT0 %s %s %.0f",chlist[j].hardname,cauthor[i],values[0]);
// 	  for( int k=1; k<5; k++ ) printf(" %.1f",values[k]);
// 	  printf(" %.0f",values2[0]);
// 	  for( int k=1; k<8; k++ ) printf(" %.1f",values2[k]);
// 	  printf("\n");
// 	}
      }     //end loop over chambers
      fout[i].close();
      fout0fit[i].close();
      foutrk[i].close();
    }  //end loop over tracking algorithms
    return;
  }    //end if iflag==1

// Segment plots
  f->cd("/Segments");
  c2->cd();
  Drawh2("SegResRh2");
  c1->cd();
  DrawResi("SegResAll_best4",values,params,filename);

// loop over regions
  c1->cd();
  for( int j=0; j<12; j++ ) { 
    DrawGaus(Form("T0fit_%s",chwheel[j]),values,filename);
    ypos[j]  = values[0];
    ypose[j] = values[1];
  }
  tzwheel = new TGraphErrors( 12, xpos, ypos, xpose, ypose);
  for( int j=0; j<12; j++ ) { 
    c1->cd();
    DrawResi(Form("SegRes_%s",chwheel[j]),values,params,filename);
    ypos[j]  = values[0];
    ypose[j] = values[1];
    c2->cd();
    Drawh2(Form("SegResRh2_%s",chwheel[j]));
  }
  resiwheel = new TGraphErrors( 12, xpos, ypos, xpose, ypose);
 
// Loop over htype: "all", "best", "T0fit", "noT0fit" };
  for( int j=0; j<4; j++ ) {
    c1->cd();
    DrawResi(Form("SegResAll_%s",htype[j]),values,params,filename);
    Drawh1(Form("SegHits_%s",htype[j]));
    if( j < 2 ) {
      c1->cd();
      DrawGaus(Form("T0fit_%s",htype[j]),values);
      //	Drawh1(Form("T0fit_%s",htype[j]));
      Drawh1(Form("NSegs_%s",htype[j]));
      Drawh1(Form("DriftTime_%s",htype[j]));
      Drawh1(Form("DriftRadius_%s",htype[j]),"","e");
      DrawResi(Form("SegResAll_best4_%s",ebtype[j]),values,params,filename);
      c2->cd();
      Drawh2(Form("SegResRh2_%s",ebtype[j]));
    }
  }    //end loop over j

//  Track histograms
  f->cd("/Tracks");
  // Make endcap/barrel trk residual plots
  TH1F *trk_res_barrel = make_h1("trk_res_barrel","Track Residuals Barrel",200, -1000., 1000., "Residuals #mum");
  TH1F *trk_res_endcap = make_h1("trk_res_endcap","Track Residuals Endcap",200, -1000., 1000., "Residuals #mum");					 

  for( int i=0; i<1; i++ ) {
    c1->cd();
    Drawh1(Form("TrkNtracks%s",cauthor[i]));
    Drawh1(Form("TrkHits%s",cauthor[i]));
    Drawh1(Form("TrkHitHoles%s",cauthor[i]));
    Drawh1(Form("TrkHitError%s",cauthor[i]));
    Drawh1(Form("TrkHitPull%s",cauthor[i]));
    Drawh1(Form("TrkHitDriftRadius%s",cauthor[i]),"","e");
    DrawResi(Form("TrkHitResi%s",cauthor[i]),values,params,filename);
    Drawh1(Form("TrkPhi%s",cauthor[i]));
    Drawh1(Form("TrkEta%s",cauthor[i]));
    c2->cd();
    Drawh2(Form("TrkResRh2%s",cauthor[i]));
    c1->cd();
    Drawh2(Form("TrkEtaPhih2%s",cauthor[i]),"colz");
    for( int j=0; j<12; j++ ) { 
      h = (TH1F *) gDirectory->Get(Form("TrkRes%s_%s",cauthor[i],chwheel[j]));
      if( j < 3 || ( j>5 && j<9 ) ) trk_res_barrel->Add(h);
      else trk_res_endcap->Add(h);
      c1->cd();
      DrawResi(Form("TrkRes%s_%s",cauthor[i],chwheel[j]),values,params,filename);
      c2->cd();
      Drawh2(Form("TrkResRh2%s_%s",cauthor[i],chwheel[j]));
    }
    c1->cd();
    DrawResi(trk_res_barrel,values,params,filename);
    DrawResi(trk_res_endcap,values,params,filename);
  }      //end loop over i (standalone, combined)

// M7 Skip plotting stuff by wheel.  Do chamber maps instead.
//  c1->cd();
//  Draw Tzero vs wheel
//  gStyle->SetOptStat(0);
//  TLegend *tzlegend = new TLegend( 0.70, 0.21, 0.89, 0.41 );
//  TH1F *tzbckgd   = new TH1F("tzbckgd","tz background",12,0.,12.);
//  TH2F *tzwheelh2 = new TH2F("tzwheelh2","T0fit by wheel",12,0.,12.,100,-15.,15.); 
//  tzwheelh2->GetYaxis()->SetTitle("T0fit [ns]");
//  AddHistBinLabels(tzwheelh2,chwheel);
//  Plot background histogram
//  Background( c1, tzbckgd, tzwheelh2, 18, 0); 
//  Draw T0 TGraphs
//  for( int i=0; i<2; i++ ) {
//    tzwheel[i]->SetMarkerColor(i+1);
//    tzwheel[i]->SetMarkerStyle(i+19);
//    tzwheel[i]->Draw("P");
//    tzlegend->AddEntry(tzwheel[i],cauthor[i]);
//  }
//  tzlegend->Draw();
//  c1->Print(Form("%s_tzwheel.png",filename));

// Do residuals by wheel
//  TLegend *wheelegend = new TLegend( 0.70, 0.69, 0.89, 0.89 );
//  TH2F *resiwheelh2 = new TH2F("resiwheelh2","Segment Residuals by wheel",12,0.,12.,100,0,250.); 
//  resiwheelh2->GetYaxis()->SetTitle("Residual width [#mum]");
//  AddHistBinLabels(resiwheelh2,chwheel);
//  Plot background histogram
//  Background( c1, tzbckgd, resiwheelh2, 18, 0); 
//  Draw Residual TGraphs
//  for( int i=0; i<2; i++ ) {
//    resiwheel[i]->SetMarkerColor(i+1);
//    resiwheel[i]->SetMarkerStyle(i+19);
//    resiwheel[i]->Draw("P");
//    wheelegend->AddEntry(tzwheel[i],cauthor[i]);
//  }
//  wheelegend->Draw();
//  c1->Print(Form("%s_resiwheel.png",filename));

  f->Close();

}  //end plotresi
