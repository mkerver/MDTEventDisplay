/***********************************************************
*  fit time and ADC histograms in a time.root file from selector_time.C 
* 
* OPTIONS
*  filename = name of a root file to process <fname>.root
*  cham     = chamber (hardware name) or region to fit [optional, default is fit all chambers]
*             Rx => Select all chambers in splitter region x
*  flag     = "" default (do nothing)
*           = S Fit all chambers starting with chamber (parameter 2)
*           = P Make Segment - track time PNG plots
*           = L Use log scale on plots
*           = T Make Tube plots
*           = M Make ML plots
*           = Z Make Mezzcard plots
*
* Outputs 
*         <fname>_fit.root    => ROOT file of time and ADC fits (copy of original, but with fit functions)
*         <fname>_ADCfit.txt  => text file of ADC fit parameters
*         <fname>_timefit.txt => text file of time fit parameters
*         PNG files of all fits
*
**********************************************************/
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TH1.h>
#include <TF1.h>
#include <TFile.h>
#include <TDirectory.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TPaveText.h>
#include <TLatex.h>
#include "/afs/atlas.umich.edu/home/muoncal/public/root/edplot.h"
#include "/afs/atlas.umich.edu/home/muoncal/public/root/chamberlist.h"
#include "/afs/atlas.umich.edu/home/muoncal/public/root/muonfixedid.h"
#include "/afs/atlas.umich.edu/home/muoncal/public/root/splitter_regions.h"
#include "/afs/atlas.umich.edu/home/muoncal/public/root/T0fit.h"

const int NDATA=20;         //number of data items written to stats file
const double MINHITS=1000.; //min histogram hits to do ADC fit.

void timefit( const char *rootfile, const char *cham="BIL1A01", const char *flag="" );
int FitTdcHist(TH1F *h, float data[NDATA], float *pardata=0, const int print=0);
int FitAdcHist(TH1F *h, float data[NDATA], const int print=0 );
int TdcMLPlot(TH1F *h[2]);
int AdcMLPlot(TH1F *h[2]);
void plotmap(TH1F *h1, TH2F *h2, const int zerolimit=1 );
void plott0s(TH1F *tube_T0_h1, TH1F *Chamber_Tube_h1, TH1F *T0_ML_h1, TH1F *T0_Mezz_h1, TH1F *T0_Tube_h1);
void plott0s2(TH1F *tube_T0_h1, TH1F *T0_Mezzcard_h1, TH1F *T0_MezzTube_h1);

int refit=0;    //number of T0s refit
TCanvas *c1;
char fname[100]="";
std::ofstream fout;

/***********************************************************
* Make fits and plots of time and ADC histograms  
************************************************************/
void timefit( const char *rootfile, const char *cham, const char *flag ) {
  int region = 0;
  int startflag=0;
  int makeplots=0;
  int tubeplots=0;
  int mlplots=0;
  int mzplots=0;
  int logy=0;

  // parse flag
  for( int i=0; flag[i]!='\0'; i++ ) { 
    if( flag[i] == 'S' )      startflag=1;
    else if( flag[i] == 'P' ) makeplots=1;
    else if( flag[i] == 'L' ) logy=1;
    else if( flag[i] == 'T' ) tubeplots=1;
    else if( flag[i] == 'M' ) mlplots=1;
    else if( flag[i] == 'Z' ) mzplots=1;
  }  
  c1 = new TCanvas("c1","chamberplot",800,600);
  if( logy ) c1->SetLogy(1);
  gStyle->SetStatW(0.48);
  gStyle->SetStatX(0.98);
  gStyle->SetStatY(0.90);
  gStyle->SetStatH(0.35);
  gStyle->SetTitleH(0.1);
  gStyle->SetTitleX(0.5);
    
// Reset chamber loop limits if chamber specified
  int imin=0, imax=MAXCHAMBERS;
  if( cham[0] != '\0' ) {
    if( cham[0] == 'R' ) {
     region = atoi(&cham[1]);     
    } else {
      int idx = MDTindex( cham );
      if( idx == -1 ) {
	printf("ERROR: Unknown chamber %s\n",cham);
	return;
      } else {
	imin = idx;
	imax = idx+1;
	if( startflag ) imax = MAXCHAMBERS;
      }
    }
  }
  
// Open ROOT file
  TFile *f = new TFile(rootfile);
  if( f && f->IsZombie() ) {   
    printf("Error with ROOT file %s\n",rootfile);
    f->Close();
    return;
  } 
  printf("Opened input ROOT file %s\n",rootfile);

// Loop over chamber folders and fit histograms
  if( makeplots ) {
//  Set global style options
    gStyle->SetStatX(0.94);      //set statbox x location
    gStyle->SetStatW(0.3);       //set statbox width
    gStyle->SetStatH(0.2);       //set statbox height - fonts scale with size
    gStyle->SetTitleX(0.5);
    gStyle->SetTitleY(1.05);
    gStyle->SetTitleH(0.2);
    gStyle->SetTitleW(0.95);
    for( int i=imin; i<imax; i++ ) { 
//  If only doing a specific region check if chamber is in it.
      if( region && chamber_regions[i] != region ) continue;

      TH1F *chamber_dtime_h1 = (TH1F *) f->Get(Form("%s/%s_dTime",chlist[i].hardname,chlist[i].hardname));
      if( chamber_dtime_h1 ) {
	chamber_dtime_h1->Draw();
        c1->Print(Form("%s_dTime.png",chlist[i].hardname));
      }
    }
    return;
  }

// Open ROOT file for refit histograms
// Long winded way of removing path on input ROOT file name
  char newrootfile[100],pname[80];
  int ilen = strlen(rootfile);
  int ibeg=0;
  for( int ii=ilen; ii>=0; ii-- ) {
    if( rootfile[ii] == '/' ) {
      ibeg=ii+1;
      break;
    }
  }
  sprintf(fname,"%s",&rootfile[ibeg]);
  ilen = strlen(fname);
  fname[ilen-5] = '\0';            //remove .root ending
  if( cham[0] != '\0' && cham[0] != 'R' ) {
    sprintf(newrootfile,"%s_fit_%s.root",fname,cham);
  } else {
    sprintf(newrootfile,"%s_fit.root",fname);
  }

  TFile *froot = new TFile(newrootfile,"recreate");
  if( froot && froot->IsZombie() ) {   
    printf("Error opening ROOT file for new fits %s\n",newrootfile);
    froot->Close();
    return;
  } 
  printf("Opened ROOT file for new fits %s\n",newrootfile);

  // Save general histograms 
  char genhist[16][20] = { "chamber_hits", "Trk_Author", "Trk_Tracks", "track_hits", 
		      "track_chambers", "segment_hits", "segment_hits_comb",
		      "Trk_Momentum", "Trk_Pt", "Trk_Phi", "Trk_Eta", "Trk_Eta_Phi", 
		      "Trk_P_Phi", "Trk_P_Eta", "Trk_Pt_Phi", "Trk_Pt_Eta" };
  for( int ih=0; ih<16; ih++ ) {
    TH1F *h1 = (TH1F *) f->Get(genhist[ih]);
    safeWrite( h1 );
  }

//  Open stats file
  if( cham[0] != '\0' && cham[0] != 'R' ) {
    fout.open(Form("%s_timefitstats_%s.txt",fname,cham),std::fstream::out);
  } else {
    fout.open(Form("%s_timefitstats.txt",fname),std::fstream::out);
  }
  if( fout.is_open() == 0 ) {
    printf("ERROR: cannot open stats file %s_timefitstats\n",fname);
    return; 
  } 
// Print header of stats file
//                        0    1     2        3        4           5           6            7         8       9    10             11           12           13       14             15        16            17        18          19      
  fout << "chamber type Events T0 T0_Err T0_Slope T0_Slope_Err Background Background_Err T0_#chi^{2} Tmax Tmax_Err Tmax_Slope Tmax_Slope_Err Tmax_#chi^{2} ADC_peak ADC_peak_Err ADC_width ADC_width_Err ADC_skew ADC_skew_Err ADC_#chi^{2}\n";
  fout << "units     x     x  [ns]  [ns]    [ns]     [ns]          x           x             x       [ns]    [ns]     [ns]        [ns]           x           [ns]      [ns]         [ns]       [ns]          x        x            x\n"; 
  fout << "min       x     0   -10    0      2         0           0           0             0        720       0        1           0           0            110         0           25          0         1.4       0            0\n"; 
  fout << "max       x     0    80    0      5         0          30           0            70        800       0       18           0          10            150         0           40          0         2.6       0           10\n";

// Loop over chamber folders and fit histograms
  for( int i=imin; i<imax; i++ ) { 
//  If only doing a specific region check if chamber is in it.
    if( region && chamber_regions[i] != region ) continue;
    f->cd();

//init arrays
//  Store chamber fit results in chamdata.  These are used to set 
//  initial values of parameters for ML and mezzcard fits.
    float chamdata[NDATA] = {-1};
    char chamber[10];
    sprintf(chamber,"%s",chlist[i].hardname);
    sprintf(pname,"%s/%s",chamber,chamber);

    // Fit chamber-level T0
    TH1F *chamber_time_h1 = (TH1F *) f->Get(Form("%s_Time",pname));
    if( FitTdcHist(chamber_time_h1,chamdata,0,1) ) continue;

    TH1F *T0_Chamber_h1=0, *T0_ML_h1=0, *T0_Mezz_h1=0, *T0_Mezzcard_h1=0, *T0_MezzTube_h1=0, *T0_Tube_h1=0;

    int nml = chlist[i].num_ml;
    int nmezz = MDTmaxmezz(chamber); //includes non-existant mezzcards in cutouts, if any
    int tottubes = 24*nmezz;         //includes non-existant tubes in cutouts, if any
    int tubes = chlist[i].ntl_ml1;
    if( chlist[i].ntl_ml2 > chlist[i].ntl_ml1 ) tubes = chlist[i].ntl_ml2;
    int layers = chlist[i].nly_ml1;
    if( chlist[i].nly_ml2 > chlist[i].nly_ml1 ) layers = chlist[i].nly_ml2;

    T0_Chamber_h1 = make_h1(Form("%s_T0_Chamber_h1",chamber),Form("%s Chamber T0",chamber),
				  1,1.,tottubes+1,"tube","T0 [ns]");
    T0_Chamber_h1->SetBinContent(1,chamdata[1]);
    T0_Chamber_h1->SetBinError(1,chamdata[2]);

    TH1F *chamber_ADC_h1 = (TH1F *) f->Get(Form("%s_ADC",pname));
    FitAdcHist(chamber_ADC_h1,chamdata,1);

    fout << chamber << " CHM" << Form(" %.0f",chamdata[0]);
    for( int k=1; k<NDATA; k++ ) fout << Form(" %.2f",chamdata[k]); 
    fout << "\n";

    // Fit multilayer data
    TH1F *ml_time_h1[2] = {};
    TH1F *ml_ADC_h1[2]  = {};
    T0_ML_h1 = make_h1(Form("%s_T0_ML_h1",chamber),Form("%s ML T0",chamber),
		       nml,1.,tottubes+1,"tube","T0 [ns]");
    for( int m=0; m<nml; m++ ) { 
      float data[NDATA] = {-1};
      ml_time_h1[m] = (TH1F *) f->Get(Form("%s_Time_ML%i",pname,m+1));
      if( FitTdcHist(ml_time_h1[m],data,chamdata,mlplots) ) continue;

      T0_ML_h1->SetBinContent(1+m,data[1]);
      T0_ML_h1->SetBinError(1+m,data[2]);

      ml_ADC_h1[m] = (TH1F *) f->Get(Form("%s_ADC_ML%i",pname,m+1));
      FitAdcHist(ml_ADC_h1[m],data,mlplots);
      fout << chamber << " ML" << m+1 << Form(" %.0f",data[0]);
      for( int k=1; k<NDATA; k++ ) fout << Form(" %.2f",data[k]); 
      fout << "\n";
    }
    // These plot both ML histograms on same plot
    TdcMLPlot(ml_time_h1);
    AdcMLPlot(ml_ADC_h1);

    // Fit mezzcard data
    int mezzstep = 8;     //number of tubes/layer in a mezzcard
    if( chlist[i].nly_ml1 == 4 ) mezzstep = 6;
    int mezzbins = tottubes/mezzstep;

    T0_Mezz_h1 = make_h1(Form("%s_T0_Mezz_h1",chamber),Form("%s Mezz T0",chamber),
			       mezzbins,1.,tottubes+1,"tube","T0 [ns]");
    T0_Mezzcard_h1 = make_h1(Form("%s_T0_Mezzcard_h1",chamber),Form("%s Mezz T0",chamber),
			     nmezz, 0.,nmezz,"Mezzcard","T0 [ns]");
    TH1F *mezz_time_h1[26] = {};
    TH1F *mezz_ADC_h1[26] = {};
    for( int m=0; m<nmezz; m++ ) { 
      float data[NDATA] = {-1};

      mezz_time_h1[m] = (TH1F *) f->Get(Form("%s_Time_Mezz%i",pname,m));
      if( FitTdcHist(mezz_time_h1[m],data,chamdata,mzplots) ) continue;
      T0_Mezzcard_h1->SetBinContent(m+1,data[1]);
      T0_Mezzcard_h1->SetBinError(m+1,data[2]);

      mezz_ADC_h1[m] = (TH1F *) f->Get(Form("%s_ADC_Mezz%i",pname,m));
      FitAdcHist(mezz_ADC_h1[m],data,mzplots);
      fout << chamber << " MZ" << m << Form(" %.0f",data[0]);
      for( int k=1; k<NDATA; k++ ) fout << Form(" %.2f",data[k]); 
      fout << "\n";
    }

// Look for tube-level histogram if processing by region
    TH1F *tube_time_h1[2][4][78] = {};    
    TH1F *tube_ADC_h1[2][4][78] = {};    
    TH1F *tube_T0_h1=0, *tube_T0slope_h1=0, *tube_Tmax_h1=0, *tube_TmaxSlope_h1=0;
    TH1F *tube_ADCpeak_h1=0, *tube_ADCwidth_h1=0;
    TH2F *tube_T0_map_h2=0, *tube_T0slope_map_h2=0, *tube_Tmax_map_h2=0, *tube_TmaxSlope_map_h2=0;
    TH2F *tube_ADCpeak_map_h2=0, *tube_ADCwidth_map_h2=0;

    //Only look for tube histograms if individual chamber or region selected
    if( cham[0] != '\0' ) {
      T0_Tube_h1 = make_h1(Form("%s_T0_Tube_h1",chamber),Form("%s Tube T0",chamber),
			      tottubes,1.,tottubes+1,"tube","T0 [ns]");
      T0_MezzTube_h1 = make_h1(Form("%s_T0_MezzcTube_h1",chamber),Form("%s Tube T0 per Mezzcard",chamber),
			     tottubes, 0.,nmezz,"Mezzcard","T0 [ns]");

      tube_T0_h1 = make_h1(Form("%s_tube_T0",chamber),Form("%s Tube T0",chamber),
				  100,-10.,90.,"T0 [ns]");
      tube_T0_map_h2 = make_h2(Form("%s_T0_map",chamber),Form("%s T0 Map",chamber),
				  tubes,1.,1+tubes,2*layers+1,1.,2*layers+2,"tube number","layer");
      tube_T0slope_h1 = make_h1(Form("%s_tube_T0slope",chamber),Form("%s Tube T0slope",chamber),
				  100,0.,5.,"T0slope [ns]");
      tube_T0slope_map_h2 = make_h2(Form("%s_T0slope_map",chamber),Form("%s T0slope Map",chamber),
				  tubes,1.,1+tubes,2*layers+1,1.,2*layers+2,"tube number","layer");
      tube_Tmax_h1 = make_h1(Form("%s_tube_Tmax",chamber),Form("%s Tube Tmax",chamber),
				  100,700.,800.,"Tmax [ns]");
      tube_Tmax_map_h2 = make_h2(Form("%s_Tmax_map",chamber),Form("%s Tmax Map",chamber),
				 tubes,1.,1+tubes,2*layers+1,1.,2*layers+2,"tube number","layer");
      tube_TmaxSlope_h1 = make_h1(Form("%s_tube_TmaxSlope",chamber),Form("%s Tube TmaxSlope",chamber),
				  100,0.,10.,"TmaxSlope [ns]");
      tube_TmaxSlope_map_h2 = make_h2(Form("%s_TmaxSlope_map",chamber),Form("%s TmaxSlope Map",chamber),
				      tubes,1.,1+tubes,2*layers+1,1.,2*layers+2,"tube number","layer");
      tube_ADCpeak_h1 = make_h1(Form("%s_tube_ADCpeak",chamber),Form("%s Tube ADCpeak",chamber),
				  100,90.,190.,"ADCpeak [ns]");
      tube_ADCpeak_map_h2 = make_h2(Form("%s_ADCpeak_map",chamber),Form("%s ADCpeak Map",chamber),
				  tubes,1.,1+tubes,2*layers+1,1,2*layers+2,"tube number","layer");
      tube_ADCwidth_h1 = make_h1(Form("%s_tube_ADCwidth",chamber),Form("%s Tube ADCwidth",chamber),
				  100,10.,60.,"ADCwidth [ns]");
      tube_ADCwidth_map_h2 = make_h2(Form("%s_ADCwidth_map",chamber),Form("%s ADCwidth Map",chamber),
				  tubes,1.,1+tubes,2*layers+1,1,2*layers+2,"tube number","layer");
      int mezztube[26]  = {};  //to track tube within mezzcard
      int tubenum = 0;
      for( int m=0; m<nml; m++ ) { 
	for( int l=0; l<layers; l++ ) {
	  for( int t=0; t<tubes; t++ ) {
            tubenum++;
	    float data[NDATA] = {-1};
	    int mllt = 1000*m + 100*l + t + 1101;
	    tube_time_h1[m][l][t] = (TH1F *) f->Get(Form("%s_Time_%i",pname,mllt));
	    if( FitTdcHist( tube_time_h1[m][l][t],data,chamdata,tubeplots) ) continue;

	    //	    int tb = MLLT2T(chamber,mllt) + 1;     //MLL2T numbering starts at 0
	    //	    printf("mllt=%i tb=%i tubenum=%i\n",mllt,tb,tubenum);
	    T0_Tube_h1->SetBinContent(tubenum,data[1]);
	    T0_Tube_h1->SetBinError(tubenum,data[2]);

            int mz = MLLT2M(chamber,mllt);         //mezzcard of tube
            mezztube[mz]++;                        //number of tube within mezzcard [1..24]
            int ibin = mz*24 + mezztube[mz];
	    T0_MezzTube_h1->SetBinContent(ibin,data[1]);
	    T0_MezzTube_h1->SetBinError(ibin,data[2]);

	    tube_ADC_h1[m][l][t] = (TH1F *) f->Get(Form("%s_ADC_%i",pname,mllt));
	    FitAdcHist(tube_ADC_h1[m][l][t],data,tubeplots);
            if( data[0] == 0. ) continue;
	    fout << chamber << " TB" << mllt << Form(" %.0f",data[0]);
	    for( int k=1; k<NDATA; k++ ) fout << Form(" %.2f",data[k]); 
	    fout << "\n";
            int tt = t + 1;
            int ll = l + m + m*chlist[i].nly_ml1 + 1;
	    tube_T0_h1->Fill(data[1]);
	    tube_T0slope_h1->Fill(data[3]);
	    tube_Tmax_h1->Fill(data[8]);
	    tube_TmaxSlope_h1->Fill(data[10]);
	    tube_ADCpeak_h1->Fill(data[13]); 
	    tube_ADCwidth_h1->Fill(data[15]);
            tube_T0_map_h2->Fill(tt,ll,data[1]);
	    tube_T0slope_map_h2->Fill(tt,ll,data[3]);
	    tube_Tmax_map_h2->Fill(tt,ll,data[8]);
	    tube_TmaxSlope_map_h2->Fill(tt,ll,data[10]);
	    tube_ADCpeak_map_h2->Fill(tt,ll,data[13]);
	    tube_ADCwidth_map_h2->Fill(tt,ll,data[15]);      
// Fill mezzcard T0 vs tube histogram
	    if( tubenum%mezzstep == 0 ) {
	      ibin = tubenum/mezzstep;
	      T0_Mezz_h1->SetBinContent(ibin,T0_Mezzcard_h1->GetBinContent(mz+1));
	      T0_Mezz_h1->SetBinError(ibin,T0_Mezzcard_h1->GetBinError(mz+1));
	    }
	  }  //end loop over tubes
	}    //end loop over layers
      }      //end loop over ml
      plotmap(tube_T0_h1,tube_T0_map_h2,0);
      plotmap(tube_T0slope_h1,tube_T0slope_map_h2);
      plotmap(tube_Tmax_h1,tube_Tmax_map_h2);
      plotmap(tube_TmaxSlope_h1,tube_TmaxSlope_map_h2);
      plotmap(tube_ADCpeak_h1,tube_ADCpeak_map_h2);
      plotmap(tube_ADCwidth_h1,tube_ADCwidth_map_h2);
      plott0s(tube_T0_h1,T0_Chamber_h1,T0_ML_h1,T0_Mezz_h1,T0_Tube_h1);
      plott0s2(tube_T0_h1, T0_Mezzcard_h1, T0_MezzTube_h1);
    }        //end if region

    //Save fitted histograms to refit ROOT file
    froot->cd();
    if( chamber_time_h1 == 0 ) continue;
    froot->mkdir(chamber);
    froot->cd(chamber);
    // Save misc chamber histograms in original file
    char mischist[4][10] = { "Momentum", "Pt", "Mezz_Hits", "dTime" };
    for( int ih=0; ih<4; ih++ ) {
      TH1F *h1 = (TH1F *) f->Get(Form("%s_%s",pname,mischist[ih]));
      safeWrite( h1 );
    }
    safeWrite( chamber_time_h1 );
    safeWrite( chamber_ADC_h1 );
    safeWrite( T0_Chamber_h1 );
    safeWrite( T0_ML_h1 );
    safeWrite( T0_Mezz_h1 );
    safeWrite( T0_Mezzcard_h1 );
    safeWrite( T0_MezzTube_h1 );
    safeWrite( T0_Tube_h1 );

    for( int m=0; m<nml; m++ ) { 
      safeWrite( ml_time_h1[m] );
      safeWrite( ml_ADC_h1[m] );
    }
    for( int m=0; m<nmezz; m++ ) { 
      safeWrite( mezz_time_h1[m] );
      safeWrite( mezz_ADC_h1[m] );
    }
    if( cham[0] != '\0' ) {
      safeWrite( T0_Tube_h1 );
      safeWrite( tube_T0_h1 );
      safeWrite( tube_T0slope_h1 );
      safeWrite( tube_Tmax_h1 );
      safeWrite( tube_TmaxSlope_h1 );
      safeWrite( tube_ADCpeak_h1 ); 
      safeWrite( tube_ADCwidth_h1 );
      safeWrite( tube_T0_map_h2 );
      safeWrite( tube_T0slope_map_h2 );
      safeWrite( tube_Tmax_map_h2 );
      safeWrite( tube_TmaxSlope_map_h2 );
      safeWrite( tube_ADCpeak_map_h2 );
      safeWrite( tube_ADCwidth_map_h2 );
      for( int m=0; m<nml; m++ ) { 
	for( int l=0; l<layers; l++ ) {
	  for( int t=0; t<tubes; t++ ) {
	    safeWrite( tube_time_h1[m][l][t] );
	    safeWrite( tube_ADC_h1[m][l][t] );
	  }  //end loop over tubes
	}    //end loop over layers
      }      //end loop over ml
    }        //end if region

  }  //end loop over chambers

// Close the ROOT file of refit histograms
  froot->Close();
  f->Close();

  return;
}      //end timefit()

/************************************************************
*  Fit Time spectrum for T0 and Tmax
************************************************************/
int FitTdcHist(TH1F *h, float data[NDATA], float pardata[NDATA], const int print ) {
  TF1 *ftm, *ft0;
  Double_t t0,slope,ampl,xmin,xmax,maxval=130.;
  Double_t tmax,tmslope,tmback=0.,A=3.,B=-0.0015;
  TPaveText *t0PT, *tmPT, *tdPT;
 
  // Protect for null histograms and low statistics time spectra
  if( h == 0 || h->GetEntries()<1000. ) return -1;
  printf("Fitting %s\n",h->GetName());

  gStyle->SetOptStat("e"); 
  data[0] = h->GetEntries();
  
//  default text box locations  
  t0PT = new TPaveText(0.50,0.65,0.98,0.82,"NDC");
  tmPT = new TPaveText(0.50,0.50,0.98,0.65,"NDC");
  tdPT = new TPaveText(0.50,0.42,0.98,0.50,"NDC");

//  Fit T0.  Used pardata for initial values if available, else estimate from histogram data
  maxval = h->GetMaximum();
  if( pardata ) {
    t0    = pardata[1];
    slope = pardata[3];
  } else {
    t0     = h->GetBinCenter(h->FindFirstBinAbove(0.45*maxval));
    slope  = 2.5;
  }
  //  ampl  = h->GetEntries()/4000.;
  ampl  = maxval/1.1;
  xmin  = t0 - 300.;
  if( xmin < -100. ) xmin = -100.;
  xmax = t0 + 70.;
  printf("Estimated parameters t0=%.1f slope=%.1f back=%.1f ampl=%.1f xmin=%.1f xmax=%.1f maxval=%.1f\n",t0,slope,0.,ampl,xmin,xmax,maxval);

  ft0 = new TF1("mt_t0_fermi",mt_t0_fermi,xmin,xmax,4);
  ft0->SetParameters(t0,slope,0.,ampl);
  h->Fit("mt_t0_fermi","r");

// refit with a smaller range if problems: 
//  slope too small refit 
//  fitted T0 very different from estimated T0
//  if( ft0->GetParameter(1) < 1. || TMath::Abs(ft0->GetParameter(0)-t0) > 5. ) {
//    printf("Refit %s\n",h->GetName());
//    refit++;
//    ft0 = new TF1("mt_t0_fermi",mt_t0_fermi,xmin,xmax-35.,4);
//    ft0->SetParameters(t0,2.,0.,ampl);
//    h->Fit("mt_t0_fermi","r");
//  }
// if still slope fix slope=2
//  if( ft0->GetParameter(1) < 1. ) {
//    printf("Refit AGAIN %s\n",h->GetName());
//    refit++;
//    ft0 = new TF1("mt_t0_fermi",mt_t0_fermi,xmin,xmax-20.,4);
//    ft0->SetParameters(t0,2.,0.,ampl);
//    ft0->SetParLimits(2,2.,2.);     //force slope=2
//    h->Fit("mt_t0_fermi","r");
//  }

  t0PT->AddText(Form("T0 %.2lf #pm %.2lf ns",
		     ft0->GetParameter(0),ft0->GetParError(0)));
  t0PT->AddText(Form("Slope %.2lf #pm %.2lf /ns",
		     ft0->GetParameter(1),ft0->GetParError(1)));
  t0PT->SetTextColor(2);
  t0PT->Draw();
  data[1] = ft0->GetParameter(0);
  data[2] = ft0->GetParError(0);
  data[3] = ft0->GetParameter(1);
  data[4] = ft0->GetParError(1);
  data[5] = ft0->GetParameter(2);
  data[6] = ft0->GetParError(2);
  double dof = ft0->GetNDF();
  if( dof ) data[7] = ft0->GetChisquare()/dof;

  // Only do Tmax fit if there are more than 2000 entries
  if( h->GetEntries() >2000. ) {
  // Do Tmax fit, first estimate input parameters
    t0 = data[1];
    if( pardata ) {
      tmax    = pardata[8]+t0;
      tmslope = pardata[10];
    } else {
      tmax    = h->GetBinCenter(h->FindLastBinAbove(0.1*maxval));
      tmslope = 8.;
    }
    xmin = tmax - 100.;
    xmax = tmax + 200.;
    if( xmax > 900. ) xmax=900.;
    printf("Estimated Tmax parameters tmax=%.1f slope=%.1f back=%.1f A=%.1f B=%.5f xmin=%.1f xmax=%.1f\n",
	   tmax,tmslope,tmback,A,B,xmin,xmax);
    ftm = new TF1("mt_tmax_fermi",mt_tmax_fermi,xmin,xmax,6);
    ftm->SetParameters(tmax,tmslope,tmback,A,B,t0);
    ftm->SetParLimits(6,t0,t0);     //Use t0 from ft0 fit
    ftm->SetLineColor(8);
    h->Fit("mt_tmax_fermi","r+");   //the "+" is to add the function to the list
    
    ftm->SetLineColor(8);
    tmPT->AddText(Form("Tmax %.1lf #pm %.1lf ns",
		       ftm->GetParameter(0),ftm->GetParError(0)));
    tmPT->AddText(Form("Slope %.1lf #pm %.1lf /ns",
		       ftm->GetParameter(1),ftm->GetParError(1)));
    tmPT->SetTextColor(8);
    tmPT->Draw();

    // Compute max drift time of time spectra: Tmax-T0
    Double_t dtmax = ftm->GetParameter(0) - ft0->GetParameter(0);
    Double_t dtmaxe = sqrt( ftm->GetParError(0)*ftm->GetParError(0)+
			    ftm->GetParError(0)*ft0->GetParError(0)  );
    tdPT->AddText(Form("DTmax %.1lf #pm %.1lf ns",dtmax,dtmaxe));
    tdPT->SetTextColor(1);
    tdPT->Draw();
    data[8] = dtmax;
    data[9] = dtmaxe;
    data[10] = ftm->GetParameter(1);
    data[11] = ftm->GetParError(1);
    dof = ftm->GetNDF();
    if( dof ) data[12] = ftm->GetChisquare()/dof;
  }

  t0PT->Draw();  //should not be needed, done above, but does not appear if you dont
  if( print ) c1->Print(Form("%s_%s.png",fname,h->GetName()));

  return 0;
}  //end FitTdcHist

/************************************************************
*  Draw ML1+ML2 Time spectrum on same plot
************************************************************/
int TdcMLPlot(TH1F *h[2]) {
  char htitle[80]; 

  // This does not work
  gStyle->SetOptStat(kFALSE);

  // Protect for null histograms
  if( h[0] == NULL ) return -1;
  sprintf(htitle,"%s",h[0]->GetTitle());
  htitle[10] = ' ';  //overwrite ML number
  h[0]->SetTitle(htitle);
  h[0]->SetStats(0);
  if( h[1] != NULL ) {
    h[1]->SetLineColor(8);
    h[1]->SetStats(0);
    //Reset h[0] scale if needed so h[1] does not get cut off
    if( h[0]->GetMaximum() < h[1]->GetMaximum() ) {
      h[0]->SetMaximum(h[1]->GetMaximum()*1.05);
    }
    h[0]->Draw();
    h[1]->Draw("same");
  } else {
    h[0]->Draw();
  }

//  text box locations  
  TPaveText *keyPT[2];
  keyPT[0] = new TPaveText(0.53,0.75,0.66,0.90,"NDC");
  keyPT[0]->AddText(" ");
  keyPT[0]->AddText("Entry");
  keyPT[0]->Draw();
  keyPT[1] = new TPaveText(0.53,0.45,0.66,0.75,"NDC");
  keyPT[1]->AddText("T0");
  keyPT[1]->AddText("Slope");
  keyPT[1]->AddText("Tmax");
  keyPT[1]->AddText("TMslp");

// Grab parameters from histogram fit functions
  TPaveText *enPT[2],*t0PT[2], *tdPT;
  Double_t t0[2]={0.,0.};
  Double_t tmax[2]={0.,0.};
  for( int ml=0; ml<2; ml++ ) {
    if( h[ml] == NULL || h[ml]->GetEntries() == 0. ) continue;
    if( ml==0 ) {
      keyPT[1]->Draw();
      enPT[0] = new TPaveText(0.66,0.75,0.82,0.90,"NDC");
      t0PT[0] = new TPaveText(0.66,0.45,0.82,0.75,"NDC");
      enPT[0]->SetTextColor(4);
      t0PT[0]->SetTextColor(50);
    } else {
      enPT[1] = new TPaveText(0.82,0.75,0.98,0.90,"NDC");
      t0PT[1] = new TPaveText(0.82,0.45,0.98,0.75,"NDC");
      enPT[1]->SetTextColor(8);
      t0PT[1]->SetTextColor(7);
    }
    enPT[ml]->AddText(Form("ML%i",ml+1));
    enPT[ml]->AddText(Form("%.0f",h[ml]->GetEntries()));
    enPT[ml]->Draw();
    //Get T0 fit
    TF1 *ft0 = h[ml]->GetFunction("mt_t0_fermi");
    if( ft0 == NULL ) continue;
    if( ml==0 ) ft0->SetLineColor(50);
    else        ft0->SetLineColor(7);
    t0[ml] = ft0->GetParameter(0);
    t0PT[ml]->AddText(Form("%.2lf",t0[0]));
    t0PT[ml]->AddText(Form("%.2lf",ft0->GetParameter(1)));
    //Get Tmax fit
    TF1 *ftm = h[ml]->GetFunction("mt_tmax_fermi");
    if( ftm == NULL ) {
      t0PT[ml]->AddText(" ");
      t0PT[ml]->AddText(" ");
      t0PT[ml]->Draw();
      continue;
    }
    tmax[ml] = ftm->GetParameter(0)-t0[ml];
    t0PT[ml]->AddText(Form("%.2lf",tmax[ml]));
    t0PT[ml]->AddText(Form("%.2lf",ftm->GetParameter(1)));
    t0PT[ml]->Draw();
    if( ml==0 ) {
      ftm->SetLineColor(50);
    } else {
      ftm->SetLineColor(7);
      if( tmax[0]!=0. && tmax[1]!=0. ) {
        tdPT  = new TPaveText(0.53,0.38,0.98,0.45,"NDC");
	tdPT->AddText(Form("#DeltaTmaxML = %.1lf ns",tmax[0]-tmax[1]));
	if( TMath::Abs(tmax[0]-tmax[1]) > 5. ) tdPT->SetTextColor(2);
	tdPT->Draw();
      }
    }
  }

  htitle[7] = '\0';   //truncate string to chamber name
  c1->Print(Form("%s_%s_TimeML.png",fname,htitle));
  return 0;
}  //end TdcMLPlot

/************************************************************
*  Fit ADC spectrum
************************************************************/
int FitAdcHist(TH1F *h, float data[NDATA], const int print ) {
  TF1 *fadc;
  TPaveText *adcPT;

  // Protect for null histograms
  if( h == 0 || h->GetEntries()<1000. ) return -1;
  printf("Fitting %s\n",h->GetName());

  gStyle->SetOptStat("emr"); 
  adcPT = new TPaveText(0.58,0.315,0.98,0.635,"NDC");

//  Refit ADC
  Double_t n    = h->GetEntries();
  Double_t rms  = h->GetRMS();
  Double_t mean = h->GetMean();
  Double_t ymin = mean-2*rms;
  if( ymin < 60. ) ymin=60.;
  Double_t ymax = mean+1.5*rms;
  fadc = new TF1("adc_fun",skewnormal,ymin,ymax,4);
  fadc->SetParameters(n,mean,rms,1.5);
  fadc->SetParLimits(1,mean-rms,mean+0.5*rms);
  fadc->SetParLimits(2,rms*0.5,rms*1.5);
  fadc->SetParLimits(3,1.,4.);
  h->Fit("adc_fun","r"); 
//  If skew is too high refit with fixed skew
  if( fadc->GetParameter(3) > 2.5 ) {
    printf("REFIT %s\n",h->GetName());
    fadc->SetParameter(3,1.6);
    fadc->SetParLimits(3,1.6,1.6);
    h->Fit("adc_fun","r") ;
  }
// if have enough hits use fit results  
  if( n > MINHITS ) {
    data[13] = fadc->GetParameter(1);
    data[14] = fadc->GetParError(1);
    data[15] = fadc->GetParameter(2);
    data[16] = fadc->GetParError(2);
    data[17] = fadc->GetParameter(3);
    data[18] = fadc->GetParError(3);
    double dof = fadc->GetNDF();
    if( dof > 0. ) data[19] = fadc->GetChisquare()/dof;
    adcPT->AddText(Form("Peak %.1lf #pm %.1lf",data[13],data[14]));
    adcPT->AddText(Form("Width %.1lf #pm %.1lf",data[15],data[16]));
    adcPT->AddText(Form("Skew %.2lf #pm %.2lf",	data[17],data[18]));
    if( dof > 0. ) adcPT->AddText(Form("#Chi^{2}/DoF     %.2lf",data[19]));
    adcPT->SetTextColor(2);
    adcPT->Draw();
// if have too few hits use fit stats instead of fit results  
  } else {
    data[12] = mean;
    data[13] = h->GetMeanError();
    data[14] = rms;
    data[15] = h->GetRMSError();
    data[16] = h->GetSkewness();
  }
  if( print ) c1->Print(Form("%s_%s.png",fname,h->GetName()));
  return 0;
}    //end FitAdcHist

/************************************************************
*  Draw ML1+ML2 ADC spectrum on same plot
************************************************************/
int AdcMLPlot(TH1F *h[2] ) {
  char htitle[80]; 

  // This does not work
  gStyle->SetOptStat(kFALSE);

  // Protect for null histograms
  if( h[0] == NULL ) return -1;
  sprintf(htitle,"%s",h[0]->GetTitle());
  htitle[10] = ' ';  //overwrite ML number
  h[0]->SetTitle(htitle);
  h[0]->SetStats(0);
  if( h[1] != NULL ) {
    h[1]->SetLineColor(8);
    h[1]->SetStats(0);
    //Reset h[0] scale if needed so h[1] does not get cut off
    if( h[0]->GetMaximum() < h[1]->GetMaximum() ) {
      h[0]->SetMaximum(h[1]->GetMaximum()*1.05);
    }
    h[0]->Draw();
    h[1]->Draw("same");
  } else {
    h[0]->Draw();
  }

//  text box locations  
  TPaveText *keyPT[2];
  keyPT[0] = new TPaveText(0.53,0.60,0.66,0.90,"NDC");
  //  keyPT[0]->SetTextAlign(11);
  keyPT[0]->AddText(" ");
  keyPT[0]->AddText("Entry");
  keyPT[0]->AddText("Mean");
  keyPT[0]->AddText("RMS");
  keyPT[0]->Draw();
  keyPT[1] = new TPaveText(0.53,0.375,0.66,0.60,"NDC");
  keyPT[1]->AddText("Peak");
  keyPT[1]->AddText("Width");
  keyPT[1]->AddText("Skew");

// Grab parameters from histogram fit functions
  TPaveText *enPT[2],*fitPT[2];
  for( int ml=0; ml<2; ml++ ) {
    if( h[ml] == NULL || h[ml]->GetEntries() == 0. ) continue;
    if( ml==0 ) {
      keyPT[1]->Draw();
      enPT[0] = new TPaveText(0.66,0.60,0.82,0.90,"NDC");
      fitPT[0] = new TPaveText(0.66,0.375,0.82,0.60,"NDC");
      enPT[0]->SetTextColor(4);
      fitPT[0]->SetTextColor(50);
    } else {
      enPT[1] = new TPaveText(0.82,0.60,0.98,0.90,"NDC");
      fitPT[1] = new TPaveText(0.82,0.375,0.98,0.60,"NDC");
      enPT[1]->SetTextColor(8);
      fitPT[1]->SetTextColor(7);
    }
    enPT[ml]->AddText(Form("ML%i",ml+1));
    enPT[ml]->AddText(Form("%.0f",h[ml]->GetEntries()));
    enPT[ml]->AddText(Form("%.1f",h[ml]->GetMean()));
    enPT[ml]->AddText(Form("%.1f",h[ml]->GetRMS()));
    enPT[ml]->Draw();
    //Get ADC fit
    TF1 *fadc = h[ml]->GetFunction("adc_fun");
    if( fadc == NULL ) continue;
    if( ml==0 ) fadc->SetLineColor(50);
    else        fadc->SetLineColor(7);
    fitPT[ml]->AddText(Form("%.1lf",fadc->GetParameter(1)));
    fitPT[ml]->AddText(Form("%.1lf",fadc->GetParameter(2)));
    fitPT[ml]->AddText(Form("%.2lf",fadc->GetParameter(3)));
    fitPT[ml]->Draw();
  }

  htitle[7] = '\0';   //truncate string to chamber name
  c1->Print(Form("%s_%s_ADCML.png",fname,htitle));
  return 0;
}    //end AdcMLPlot

// Plot maps using range from distribution of data
void plotmap(TH1F *h1, TH2F *h2, const int zerolimit ) {
  if( h1 == 0 || h2 == 0 ) return;
  gStyle->SetOptStat(0);
  Double_t zmax = h1->GetMean() + 4.*h1->GetRMS();
  Double_t zmin = h1->GetMean() - 4.*h1->GetRMS();
  if( zerolimit && zmin < 0. ) zmin = 0.;
  h2->SetMinimum(zmin);
  h2->SetMaximum(zmax);
  h2->Draw("colz");
  c1->Print(Form("%s.png",h2->GetName()));
}

// Plot T0s from chamber, ML, mezzcard, tube on same plot
void plott0s(TH1F *tube_T0_h1, TH1F *Chamber_T0_h1, TH1F *T0_ML_h1, TH1F *T0_Mezz_h1, TH1F *T0_Tube_h1) {
  if( tube_T0_h1==0 || Chamber_T0_h1==0 || T0_ML_h1==0 || T0_Mezz_h1==0 || T0_Tube_h1==0 ) return;
  Double_t zmax = tube_T0_h1->GetMean() + 4.*tube_T0_h1->GetRMS();
  Double_t zmin = tube_T0_h1->GetMean() - 4.*tube_T0_h1->GetRMS();
  gStyle->SetOptStat(0);
  Chamber_T0_h1->SetLineColor(5); 
  T0_ML_h1->SetLineColor(3); 
  T0_Mezz_h1->SetLineColor(2);
  Chamber_T0_h1->SetMinimum(zmin);
  Chamber_T0_h1->SetMaximum(zmax);
  Chamber_T0_h1->Draw();
  T0_ML_h1->Draw("same");
  T0_Mezz_h1->Draw("same");
  T0_Tube_h1->Draw("same");
  c1->Print(Form("%s.png",T0_Tube_h1->GetName()));
}
// Plot T0s from chamber, ML, mezzcard, tube on same plot
void plott0s2(TH1F *tube_T0_h1, TH1F *T0_Mezzcard_h1, TH1F *T0_MezzTube_h1) {
  if( tube_T0_h1==0 || T0_Mezzcard_h1==0 || T0_MezzTube_h1==0 ) return;
  Double_t zmax = tube_T0_h1->GetMean() + 4.*tube_T0_h1->GetRMS();
  Double_t zmin = tube_T0_h1->GetMean() - 4.*tube_T0_h1->GetRMS();
  gStyle->SetOptStat(0);
  T0_Mezzcard_h1->SetLineColor(2);
  T0_Mezzcard_h1->SetMinimum(zmin);
  T0_Mezzcard_h1->SetMaximum(zmax);
  T0_Mezzcard_h1->Draw();
  T0_MezzTube_h1->Draw("same");
  c1->Print(Form("%s.png",T0_Mezzcard_h1->GetName()));
}
