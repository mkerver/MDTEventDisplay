/**********************************************************
 selector_resi.C

   Make residual plots 
    output: resi.root containing histograms

    Options (catcatente in one word):
      v => set verbose = 1
      V => set verbose = 2
      g => set trackcut = 1
      G => set trackcut = 2
      f/F => set suppressT0Fitcut = 1
      S => set TRACK_AUTHOR1 = 200  (2015 calibstream, default)
      T => set TRACK_AUTHOR1 = 206  (2015 RAW standalone track)
           set TRACK_AUTHOR1 = 208  (2015 RAW combined track)
           set NUM_TRACK_TYPES = 2  (make plots for both 206 and 208 tracks)
      1 -> set TRACK_AUTHOR1 = 0    (Run1 calibstream)

    use plotresi.C to make plots of them.
    then use sortresiplots.sh to sort plots

   Edward Diehl 8-Jan-2010
   Edward Diehl 1-Dec-2014 Update for Run2
   Edward Diehl 26-Feb-2015 Update for M8
      - Remove duplicate Muonboy/Moore segment histos 
      Now only one version of segment histos.
      - Leave provision for up to 2 track types (histos for each)
      Track types controlled by const
      NUM_TRACK_TYPES => How many tracks checked (1 or 2)
      TRACK_AUTHOR1   => Author of first track to check
      TRACK_AUTHOR2   => Author of second track to check
      - Reorganize output histograms into folders in ROOT file
      - Add Claudio best segment finding

**********************************************************/
#define selector_cxx
#include <fstream>
#include <map>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <TH1.h>
#include <TList.h>
#include <TMath.h>
#include <TObject.h>
#include "selector.h"
// hardcoded for condor batch jobs
#include "edplot.h"
#include "muonfixedid.h"
#include "chamberlist.h"
#include "chamberwheel.h"
#include "BestSegment.h"

int nevents[2] = {0,0};
int verbose = 0;           //flag to control printing
int trackcut = 0;          //flag to require segment on track
int suppressT0Fitcut = 0;  //flag to control whether to T0Fit check
const int   Runs[2] = {251367,253395};  //range of runs (M8) for histogram of events per run
const float FITT0CUT=0.1;  //cut for fitted T0s (to not be counted as 0)
const int   SEGMENT_HIT_CUT=6;    //min segment hits for best segment
int         TRACK_AUTHOR1=200;    //Run2: 206=RAW data; 200=calibstream 
int         TRACK_AUTHOR2=10;     //3rd chain muon combined track (?)
int         NUM_TRACK_TYPES=1;    //controls how many track types to check (1 or 2)
const int   MIN_SEGMENT_TYPE=4;   //min segment author type 
const int   ADC_CUT=50;           // ADC cut for raw hits
int  track_types[100];
int  ntrack_types=0;
int  runNumber=0;

//Track types seen in M8 data: 206, 211, 201, 210, 208 
// calibstream track type: 200

// Histograms
TList *histlist;
TH1F *run_h1;                 //run number

// Chamber rawhit histos
TH1F *rawtdc_chamber_h1[MAXCHAMBERS];  //TDC for hits in rawhits
TH1F *rawadc_chamber_h1[MAXCHAMBERS];  //ADC for hits in rawhits
TH2F *rawadctdc_chamber_h2[MAXCHAMBERS];  //ADC vs TDC for hits in rawhits
TH2F *raw_hit_occ_h2[MAXCHAMBERS];     //tube hit occupancy
TH1F *rawtdc_region_h1[15];            //TDC for hits in rawhits for 12 regions + all, barrel, endcap
TH1F *rawadc_region_h1[15];            //ADC for hits in rawhits
TH2F *rawadctdc_region_h2[15];         //ADC vs TDC for hits in rawhits
// Chamber rawhit histos after ADC cut
TH1F *rawtdc_chamber_cut_h1[MAXCHAMBERS];  //TDC for hits in rawhits
TH1F *rawadc_chamber_cut_h1[MAXCHAMBERS];  //ADC for hits in rawhits
// TH2F *rawadctdc_chamber_cut_h2[MAXCHAMBERS];  //ADC vs TDC for hits in rawhits
TH2F *raw_hit_occ_cut_h2[MAXCHAMBERS];     //tube hit occupancy
TH1F *rawtdc_region_cut_h1[15];            //TDC for hits in rawhits for 12 regions + all, barrel, endcap
TH1F *rawadc_region_cut_h1[15];            //ADC for hits in rawhits
TH2F *rawadctdc_region_cut_h2[15];         //ADC vs TDC for hits in rawhits

// Chamber segment histos
TH1F *tdc_chamber_h1[MAXCHAMBERS];  //TDC for hits in segments
TH1F *adc_chamber_h1[MAXCHAMBERS];  //ADC for hits in segments
TH2F *adctdc_chamber_h2[MAXCHAMBERS];  //ADC vs TDC for hits in segments
TH1F *tdc_region_h1[15];            //TDC for hits in segments for 12 regions + all, barrel, endcap
TH1F *adc_region_h1[15];            //ADC for hits in segments
TH2F *adctdc_region_h2[15];         //ADC vs TDC for hits in segments
TH2F *seg_hit_occ_h2[MAXCHAMBERS];  //tube hit occupancy

//  0=all 1=best 2=T0fit,best 3=T0fit,best,5+
TH1F *seg_resi_all_h1[4];        //seg residuals all segments  
TH1F *seg_resi_best_h1[3];       //seg residuals all segments (barrel/endcap/all)
TH1F *seg_resi_region_h1[12];    //0..12=region  
TH2F *seg_resi_r_region_h2[12];  //0..12=region  
TH1F *seg_resi_chamber_h1[MAXCHAMBERS];     
TH1F *seg_pull_region_h1[12];      //0..12=region  
TProfile *seg_pull_region_pr[12];  //0..12=region  
TH1F *t0fit_h1[2];                 //t0fit  (all/best)
TH1F *t0fit_region_h1[12];         //t0fit  0..12=region
TH1F *t0fit_chamber_h1[MAXCHAMBERS];   //t0fit by chamber
TH1F *nsegs_h1[2];       //number of segments (all/best)
TProfile *seg_resi_r_pr; //residuals vs radius
TH2F *seg_resi_r_h2[3];  //residuals vs radius (barrel/endcap/all)
TH1F *seg_hits_h1[4];    //number of hits per segment 0/1=all/best/t0fit/not0fit
TH1F *seg_chi2_h1[2];    //number of hits per segment 0/1=all/best
TH1F *driftime_h1[2];    //Drift time   0/1=best/T0fit
TH1F *driftradius_h1[2]; //Drift radius 0/1=best/T0fit
TProfile *seg_tof_pr[12];   //TOF for wheels
TH2F *seg_xpos_h2[12];      //X position in chamber
TH1F *seg_hits_chamber_h1; //number of segment hits per chamber.
TH1F *seg_second_coord_chamber_h1; //fraction of segment hits with segment 2nd coordinate per chamber.

// Track histograms 0/1=Author1/Author2
TH1F *trk_author_h1;        //Tracks authors
TH1F *trk_ntracks_h1[2];    //number of tracks (will be 0 or 1)
TH1F *trk_ntracks_csc_h1[2]; //number of tracks (which have CSC hits)
TH1F *trk_hits_h1[2];       //track hits (MDTs)
TH1F *trk_hits_csc_h1[2];   //track hits (CSCs)
TH1F *trk_holes_h1[2];      //track holes
TH1F *trk_chambers_h1[2];   //Number of MDT chambers on track
TH1F *trk_chamber_h1[2];      //tracks per chamber
TH1F *trk_chamber_csc_h1[2];  //tracks per chamber, tracks with CSC   
TH1F *trk_chamber_mdt_h1[2];  //tracks per chamber, tracks with MDT only   
TH1F *trk_hit_err_h1[2];    //track hit error
TH1F *trk_hit_pull_h1[2];   //track hit pull
TH1F *trk_hit_r_h1[2];      //track hit drift radius
TH1F *trk_hit_resi_h1[2];   //track hit residuals
TH2F *trk_hit_resi_r_h2[2]; //residuals vs radius
TH1F *trk_phi_h1[2];        //track phi angle 
TH1F *trk_eta_h1[2];        //psuedo-rapidity 
TH2F *trk_eta_phi_h2[2];    //eta vs phi
TH2F *trk_eta_phi_rpc_h2[2];  //eta vs phi tracks with RPC hits
TH2F *trk_eta_phi_tgc_h2[2];  //eta vs phi tracks with TGC hits
TH1F *trk_phi_csc_h1[2];      //track phi angle, tracks with CSC 
TH1F *trk_eta_csc_h1[2];      //psuedo-rapidity, tracks with CSC 
TH2F *trk_eta_phi_csc_h2[2];  //eta vs phi, tracks with CSC
TH1F *trk_phi_mdt_h1[2];      //track phi angle, tracks with MDT only 
TH1F *trk_eta_mdt_h1[2];      //psuedo-rapidity, tracks with MDT only
TH1F *trk_phi_barrel_h1[2];   //track phi angle, tracks with eta < 1 
TH1F *trk_phi_endcap_h1[2];   //track phi angle, tracks with eta > 1 
TH2F *trk_eta_phi_mdt_h2[2];  //eta vs phi, tracks with MDT only
TH1F *trk_dR_Author1Author2_h1[2];   //dR between Author1 and Author2 0/1=all/best
TH1F *trk_dR_Combined_h1;            //dR between Combined tracks
TH1F *trk_deta_Author1Author2_h1[2]; //deta between Author1 and Author2 0/1=all/best
TH1F *trk_deta_Combined_h1;          //deta between Combined tracks
TH1F *trk_dphi_Author1Author2_h1[2]; //dphi between Author1 and Author2 0/1=all/best
TH1F *trk_dphi_Combined_h1;              //dphi between Combined tracks
TH1F *trk_resi_region_h1[2][12];    //0/1=Author1/Author2 0..12=region  
TH2F *trk_resi_r_region_h2[2][12];  //0/1=Author1/Author2 0..12=region  
TH1F *trk_resi_chamber_h1[2][MAXCHAMBERS];  //0/1=Author1/Author2   
TH1F *trk_momentum_h1[2];    //P over Q

TH1F *tgc_nTgc_h1;     //0-300
TH1F *csc_nCsc_h1;     //0-100
TH1F *rpc_nRpc_h1;     //0-100
TH1F *csc_Z_h1;        //csc_gPosZ 8000
TH1F *tgc_Z_h1;        //16000
TH1F *rpc_Z_h1;    //rpcOs_gPosZ 13000
TH2F *csc_YvsX_EIA_h2,*csc_YvsX_EIC_h2;  //2000-2000
TH2F *tgc_YvsX_EMA_h2,*tgc_YvsX_EMC_h2;  //-12000,12000, Z>10000
TH2F *tgc_YvsX_EIA_h2,*tgc_YvsX_EIC_h2;  //-6500,6500, Z<10000
TH2F *csc_trk_YvsX_EIA_h2,*csc_trk_YvsX_EIC_h2;  //2000-2000
TH2F *tgc_trk_YvsX_EMA_h2,*tgc_trk_YvsX_EMC_h2;  //-12000,12000, Z>10000
TH2F *tgc_trk_YvsX_EIA_h2,*tgc_trk_YvsX_EIC_h2;  //-6500,6500, Z<10000

/**************************************************************/
// Book histos, read event list
/**************************************************************/
void selector::Begin(TTree *) {
  char copt[80];
// Change these to match track types found in ntuple
  char cauthor[2][20] = { "", "" };
  char htype  [4][10] = { "all", "best", "T0fit", "noT0fit" };
  char ebtype [3] [7] = { "barrel", "endcap", "all" };

  printf("Running selector_resi.C\n");

  TString option = GetOption();
  sprintf(copt,"%s",option.Data());

//  Print help message to users
  if( option == "h" ) {
    printf("selector_resi: Residual plots\n");
    printf("    option v => Print event information\n");
    printf("           V => Print more event information (crashes)\n");
    printf("           f/F => Suppress T0Fit cut (for data without T0Fit)\n");
    printf("           g => Require segments on tracks\n");
    printf("           G => Require segments on tracks of >20GeV momentum\n");
    printf("           S => calibstream calib ntuple, so use track author 200 (default)\n");
    printf("           T => Tier0 calib ntuple, so use track author 206\n");
    printf("           1 => Run1 calibstream calib ntuple, so use track author 0\n");
    printf("                Default assumes RAW data calib ntuple with track author 206\n");
    printf("    Edward Diehl  1/08/10\n");
    exit(0);    //Abort job
  }
  for( int i=0; i<10; i++ ) {
    if( copt[i] == '\0') break;
    else if( copt[i] == 'v' ) {
      verbose = 1;
    } else if( copt[i] == 'V' ) {
      verbose = 2;
    } else if( copt[i] == 'g' ) {
      trackcut = 1;
    } else if( copt[i] == 'G' ) {
      trackcut = 2;
    } else if( copt[i] == 'f' || copt[i] == 'F' ) {
      suppressT0Fitcut = 1;
    } else if( copt[i] == 'S' ) {  //calibstream
      TRACK_AUTHOR1 = 200;
    } else if( copt[i] == 'T' ) {  //Tier0
      TRACK_AUTHOR1 = 206;
      TRACK_AUTHOR2 = 208;
      NUM_TRACK_TYPES = 2;
    } else if( copt[i] == '1' ) {  //Run1 calibstream
      TRACK_AUTHOR1 = 0;
    } else {
      printf("WARNING: Unknown option %c\n",copt[i]);
    }  
  }    //end loop over options

  sprintf(cauthor[0],"Trk%i",TRACK_AUTHOR1);
  sprintf(cauthor[1],"Trk%i",TRACK_AUTHOR2);

//  Book histos
  histlist = new TList();
  run_h1 = make_h1(histlist,"Run","Run", Runs[1]-Runs[0], Runs[0], Runs[1]);

// Plots combining all chambers
  seg_resi_r_pr = make_prof(histlist,"SegResRpr","Segment Residuals vs Radius",
			    100, 0., 15., -500., 500., 
			    "Tube radius [mm]", "Mean Residual #mum");
  seg_resi_r_h2[2] = make_h2(histlist, "SegResRh2","Segment Residuals vs Radius",
				100, -15., 15., 100, -500., 500., 
				"Tube radius [mm]", "Residuals #mum");
  seg_resi_best_h1[2] = make_h1(histlist,"SegResAll_best4","Segment Residuals",
				200, -1000., 1000., "Residuals #mum");
  seg_hits_chamber_h1 = make_h1(histlist,"SegHitPerChamber",
				"Segment Hits per chamber",
				MAXCHAMBERS, 0., MAXCHAMBERS, "", "hits");
  seg_second_coord_chamber_h1 = make_h1(histlist,"SecondCoordPerChamber",
				"Fraction of Segment Hits with 2nd Coordinate",
				MAXCHAMBERS, 0., MAXCHAMBERS);

// Misc RPC, TGC, CSC
  tgc_nTgc_h1 = make_h1(histlist,"tgc_nTgc","TGC segment hits", 300, 0., 300.);
  csc_nCsc_h1 = make_h1(histlist,"csc_nCsc","CSC segment hits", 100, 0., 100.);
  rpc_nRpc_h1 = make_h1(histlist,"rpc_nRpc","RPC segment hits", 100, 0., 100.);
  tgc_Z_h1 = make_h1(histlist,"tgc_Z","TGC Z", 200, -16000., 16000., "mm");
  csc_Z_h1 = make_h1(histlist,"csc_Z","CSC Z", 200, -16000., 16000., "mm");
  rpc_Z_h1 = make_h1(histlist,"rpc_Z","RPC Z", 200, -16000., 16000., "mm");
  csc_YvsX_EIA_h2 = make_h2(histlist,"csc_YvsX_EIA","CSC Segments Y vs X EIA",200, -2500., 2500.,200, -2500., 2500., "mm", "mm");
  csc_YvsX_EIC_h2 = make_h2(histlist,"csc_YvsX_EIC","CSC Segments Y vs X EIC",200, -2500., 2500.,200, -2500., 2500., "mm", "mm");
  tgc_YvsX_EIA_h2 = make_h2(histlist,"tgc_YvsX_EIA","TGC Segments Y vs X EIA",200, -6500., 6500.,200, -6500., 6500., "mm", "mm");
  tgc_YvsX_EIC_h2 = make_h2(histlist,"tgc_YvsX_EIC","TGC Segments Y vs X EIC",200, -6500., 6500.,200, -6500., 6500., "mm", "mm");
  tgc_YvsX_EMA_h2 = make_h2(histlist,"tgc_YvsX_EMA","TGC Segments Y vs X EMA",200, -12000., 12000.,200, -12000., 12000., "mm", "mm");
  tgc_YvsX_EMC_h2 = make_h2(histlist,"tgc_YvsX_EMC","TGC Segments Y vs X EMC",200, -12000., 12000.,200, -12000., 12000., "mm", "mm");
  csc_trk_YvsX_EIA_h2 = make_h2(histlist,"csc_trk_YvsX_EIA","CSC Tracks Y vs X EIA",200, -2500., 2500.,200, -2500., 2500., "mm", "mm");
  csc_trk_YvsX_EIC_h2 = make_h2(histlist,"csc_trk_YvsX_EIC","CSC Tracks Y vs X EIC",200, -2500., 2500.,200, -2500., 2500., "mm", "mm");
  tgc_trk_YvsX_EIA_h2 = make_h2(histlist,"tgc_trk_YvsX_EIA","TGC Tracks Y vs X EIA",200, -6500., 6500.,200, -6500., 6500., "mm", "mm");
  tgc_trk_YvsX_EIC_h2 = make_h2(histlist,"tgc_trk_YvsX_EIC","TGC Tracks Y vs X EIC",200, -6500., 6500.,200, -6500., 6500., "mm", "mm");
  tgc_trk_YvsX_EMA_h2 = make_h2(histlist,"tgc_trk_YvsX_EMA","TGC Tracks Y vs X EMA",200, -12000., 12000.,200, -12000., 12000., "mm", "mm");
  tgc_trk_YvsX_EMC_h2 = make_h2(histlist,"tgc_trk_YvsX_EMC","TGC Tracks Y vs X EMC",200, -12000., 12000.,200, -12000., 12000., "mm", "mm");

//  These plots only used if there is a 2nd track type in histo
  if( NUM_TRACK_TYPES == 2 ) {
    trk_dR_Combined_h1 = make_h1(histlist,"dRCombined",
			    Form("dR between tracks %i %i",TRACK_AUTHOR1,TRACK_AUTHOR2),
			    100, 0., 8.5, "dR");
    trk_deta_Combined_h1 = make_h1(histlist,"detaCombined",
		      Form("#Delta#eta between tracks %i %i",TRACK_AUTHOR1,TRACK_AUTHOR2),
		      100, -6., 6., "#Delta#eta");
    trk_dphi_Combined_h1 = make_h1(histlist,"dphiCombined",
		      Form("#Delta#phi between tracks %i %i",TRACK_AUTHOR1,TRACK_AUTHOR2),
		      100, -6., 6., "#Delta#phi");
  }

// Loop over histogram types (htype, though on cauthor for some histograms)
  for( int i=0; i<2; i++ ) {

//  Compare tracks if there is a 2nd track type in histo
    if( NUM_TRACK_TYPES == 2 ) {
      trk_dR_Author1Author2_h1[i] = make_h1(histlist,Form("dRAuthor1Author2_%s",htype[i]),
		      Form("dR between  %i %i %s",TRACK_AUTHOR1,TRACK_AUTHOR2,htype[i]),
		      100, 0., 2, "dR");
      trk_deta_Author1Author2_h1[i] = make_h1(histlist,Form("detaAuthor1Author2_%s",htype[i]),
		      Form("#Delta#eta between %i %i %s",TRACK_AUTHOR1,TRACK_AUTHOR2,htype[i]),
		      100, -2., 2., "#Delta#eta");
      trk_dphi_Author1Author2_h1[i] = make_h1(histlist,Form("dphiAuthor1Author2_%s",htype[i]),
		      Form("#Delta#phi between %i %i %s",TRACK_AUTHOR1,TRACK_AUTHOR2,htype[i]),
		      100, -2., 2., "#Delta#phi");
    }     //end if NUM_TRACK_TYPES == 2
  }       //end loop over i

// loop over regions (wheels)
  Double_t wheelX[6] = { 1500., 2000., 2500., 2250., 3000., 3200. };
  for( int j=0; j<12; j++ ) { 
    tdc_region_h1[j] = make_h1(histlist,Form("TDC_%s",chwheel[j]),
			       Form("%s TDC",chwheel[j]),
			       200, 0., 1400., "TDC [ticks]");
    adc_region_h1[j] = make_h1(histlist,Form("ADC_%s",chwheel[j]),
			       Form("%s ADC",chwheel[j]),
			       100, 0., 400., "ADC [ticks]");
    adctdc_region_h2[j] = make_h2(histlist,Form("ADCTDC_%s",chwheel[j]),
				  Form("%s ADC vs TDC",chwheel[j]),
				  100,0.,1400.,100,0.,400.,"TDC [ticks]","ADC [ticks]");
    rawtdc_region_h1[j] = make_h1(histlist,Form("rawTDC_%s",chwheel[j]),
				  Form("%s raw TDC",chwheel[j]),
				  200, 0., 1400., "TDC [ticks]");
    rawadc_region_h1[j] = make_h1(histlist,Form("rawADC_%s",chwheel[j]),
				  Form("%s raw ADC",chwheel[j]),
				  100, 0., 400., "ADC [ticks]");
    rawadctdc_region_h2[j] = make_h2(histlist,Form("rawADCTDC_%s",chwheel[j]),
				     Form("%s raw ADC vs TDC",chwheel[j]),
				     100,0.,1400.,100,0.,400.,"TDC [ticks]","ADC [ticks]");
    rawtdc_region_cut_h1[j] = make_h1(histlist,Form("rawTDCcut%d_%s",ADC_CUT,chwheel[j]),
				      Form("%s raw TDC (ADC>%d)",chwheel[j],ADC_CUT),
				      200, 0., 1400., "TDC [ticks]");
    rawadc_region_cut_h1[j] = make_h1(histlist,Form("rawADCcut%d_%s",ADC_CUT,chwheel[j]),
				      Form("%s raw ADC (ADC>%d)",chwheel[j],ADC_CUT),
				      100, 0., 400., "ADC [ticks]");
    rawadctdc_region_cut_h2[j] = make_h2(histlist,Form("rawADCTDCcut%d_%s",ADC_CUT,chwheel[j]),
					 Form("%s raw ADC vs TDC (ADC>%d)",chwheel[j],ADC_CUT),
					 100,0.,1400.,100,0.,400.,"TDC [ticks]","ADC [ticks]");
    t0fit_region_h1[j] = make_h1(histlist,Form("T0fit_%s",chwheel[j]),
				 Form("T0 refit %s",chwheel[j]),
				 100, -100., 100., "T0 refit [ns]");

    seg_tof_pr[j] = make_prof(histlist, Form("TOFpr_%s",chwheel[j]),
			      Form("%s TOF",chwheel[j]),
			      wheelmax[j%6], 0., wheelmax[j%6], 0., 100., 
			      "chamber","TOF [ns]"); 

    seg_xpos_h2[j] = make_h2(histlist, Form("Xposh2_%s",chwheel[j]),
			     Form("%s ChamberX",chwheel[j]),
			     wheelmax[j%6], 0., wheelmax[j%6], 100, -wheelX[j%6], wheelX[j%6], 
			     "chamber","X pos [mm]"); 
    seg_resi_region_h1[j] = make_h1(histlist,Form("SegRes_%s",chwheel[j]),
				    Form("%s Segment Residuals",chwheel[j]),
				    200, -1000., 1000., "Residuals #mum");
    seg_resi_r_region_h2[j] = make_h2(histlist, Form("SegResRh2_%s",chwheel[j]),
				      Form("%s Segment Residuals vs Radius",chwheel[j]),
				      100, -15., 15., 100, -500., 500., 
				      "Tube radius [mm]", "Residuals #mum");
    seg_pull_region_h1[j] = make_h1(histlist, Form("SegPull_%s",chwheel[j]),
				    Form("%s Segment Pull",chwheel[j]),
				    100, -10., 10.,"Pull");
    seg_pull_region_pr[j] = make_prof(histlist, Form("SegPullPr_%s",chwheel[j]),
				      Form("%s Segment Pull",chwheel[j]),
				      wheelmax[j%6], 0., wheelmax[j%6], -10., 10., "Chamber", "Pull");

// Loop over track types
    for( int i=0; i<NUM_TRACK_TYPES; i++ ) {
      trk_resi_region_h1[i][j] = make_h1(histlist,Form("TrkRes%s_%s",cauthor[i],chwheel[j]),
					 Form("%s %s Track Residuals",cauthor[i],chwheel[j]),
					 200, -1000., 1000., "Residuals #mum");
      trk_resi_r_region_h2[i][j] = make_h2(histlist, Form("TrkResRh2%s_%s",cauthor[i],chwheel[j]),
					   Form("%s %s Track Residuals vs Radius",cauthor[i],chwheel[j]),
					   100, -15., 15., 100, -500., 500., 
					   "Tube radius [mm]", "Residuals #mum");
    }     //end loop over track types
  }       //end loop over regions

// loop over chambers
  for( int j=0; j<MAXCHAMBERS; j++ ) { 
    tdc_chamber_h1[j] = make_h1(histlist,Form("TDC_%s",chlist[j].hardname),
				Form("%s segment TDC",chlist[j].hardname),
				200, 0., 1400., "TDC [ticks]");
    adc_chamber_h1[j] = make_h1(histlist,Form("ADC_%s",chlist[j].hardname),
				Form("%s segment ADC",chlist[j].hardname),
				100, 0., 400., "ADC [ticks]");
    adctdc_chamber_h2[j] = make_h2(histlist,Form("ADCTDC_%s",chlist[j].hardname),
				   Form("%s segment ADC vs TDC",chlist[j].hardname),
				   100,0.,1400.,100,0.,400.,"TDC [ticks]","ADC [ticks]");
    int tubes = chlist[j].ntl_ml1;
    if( chlist[j].ntl_ml2 > chlist[j].ntl_ml1 ) tubes = chlist[j].ntl_ml2;
    int layers = 7;
    if( chlist[j].nly_ml1==4 || chlist[j].nly_ml2==4 ) layers = 9;
    seg_hit_occ_h2[j] = make_h2(histlist,Form("SegHitOcc_%s",chlist[j].hardname),
				Form("%s Seg Hit Occupancy",chlist[j].hardname),
				tubes,1.,1+tubes,layers,1,1+layers,"tube number","layer");
    rawtdc_chamber_h1[j] = make_h1(histlist,Form("rawTDC_%s",chlist[j].hardname),
				   Form("%s raw TDC",chlist[j].hardname),
				   200, 0., 1400., "TDC [ticks]");
    rawadc_chamber_h1[j] = make_h1(histlist,Form("rawADC_%s",chlist[j].hardname),
				   Form("%s raw ADC",chlist[j].hardname),
				   100, 0., 400., "ADC [ticks]");
    rawadctdc_chamber_h2[j] = make_h2(histlist,Form("rawADCTDC_%s",chlist[j].hardname),
				      Form("%s raw ADC vs TDC",chlist[j].hardname),
				      100,0.,1400.,100,0.,400.,"TDC [ticks]","ADC [ticks]");
    raw_hit_occ_h2[j] = make_h2(histlist,Form("RawHitOcc_%s",chlist[j].hardname),
				Form("%s Raw Hit Occupancy",chlist[j].hardname),
				tubes,1.,1+tubes,layers,1,1+layers,"tube number","layer");

    rawtdc_chamber_cut_h1[j] = make_h1(histlist,Form("rawTDCcut%d_%s",ADC_CUT,chlist[j].hardname),
				       Form("%s raw TDC (ADC>%d)",chlist[j].hardname, ADC_CUT),
				       200, 0., 1400., "TDC [ticks]");
    rawadc_chamber_cut_h1[j] = make_h1(histlist,Form("rawADCcut%d_%s",ADC_CUT,chlist[j].hardname),
				       Form("%s raw ADC (ADC>%d)",chlist[j].hardname,ADC_CUT),
				       100, 0., 400., "ADC [ticks]");
    //    rawadctdc_chamber_cut_h2[j] = make_h2(histlist,Form("rawADCTDCcut%d_%s",ADC_CUT,chlist[j].hardname),
    //					  Form("%s raw ADC vs TDC (ADC>%d)",chlist[j].hardname,ADC_CUT),
    //					  100,0.,1400.,100,0.,400.,"TDC [ticks]","ADC [ticks]");
    raw_hit_occ_cut_h2[j] = make_h2(histlist,Form("RawHitOcccut%d_%s",ADC_CUT,chlist[j].hardname),
				    Form("%s Raw Hit Occupancy (ADC>%d)",chlist[j].hardname,ADC_CUT),
				    tubes,1.,1+tubes,layers,1,1+layers,"tube number","layer");
    
    seg_resi_chamber_h1[j] = make_h1(histlist,Form("SegRes_%s",chlist[j].hardname),
				     Form("%s Segment Residuals",chlist[j].hardname),
				     100, -1000., 1000., "Residuals #mum");
    t0fit_chamber_h1[j] = make_h1(histlist,Form("T0fit_%s",chlist[j].hardname),
				  Form("%s T0 refit",chlist[j].hardname),
				  100, -100., 100., "T0 refit [ns]");
// Loop over track types
    for( int i=0; i<NUM_TRACK_TYPES; i++ ) {
//  Suppress 2nd track type histo if not used
      if( i < NUM_TRACK_TYPES ) {
	trk_resi_chamber_h1[i][j] = make_h1(histlist,Form("TrkHitResi%s_%s",cauthor[i],chlist[j].hardname),
					    Form("%s Track Hit Residuals %s",cauthor[i],chlist[j].hardname),
					    100, -1000., 1000., "#mum");
      }   //end if i < NUM_TRACK_TYPES
    }     //end loop over track types
  }     //end loop over chambers

// loop over types "all", "best", "T0fit", "noT0fit"
  for( int j=0; j<4; j++ ) { 
//  Suppress 2nd track type histo if not used
    seg_resi_all_h1[j] = make_h1(histlist,Form("SegResAll_%s",htype[j]),
				 Form("Segment Residuals %s",htype[j]),
				 200, -1000., 1000., "Residuals #mum");
    seg_hits_h1[j] = make_h1(histlist,Form("SegHits_%s",htype[j]),
			     Form("Segment Hits %s",htype[j]),
			     20, 0., 20., "number of MDT hits");
    if( j<2 ) {
      t0fit_h1[j] = make_h1(histlist,Form("T0fit_%s",htype[j]),
			    Form("T0 refit %s",htype[j]),
			    100, -100., 100., "T0 refit [ns]");
      nsegs_h1[j] = make_h1(histlist,Form("NSegs_%s",htype[j]),
			    Form("Segments %s",htype[j]),
			    20, 0., 20., "number of segments");
      seg_chi2_h1[j] = make_h1(histlist,Form("SegChi2_%s",htype[j]),
			       Form("Segment #Chi^{2} %s",htype[j]),
			       100, 0., 20., "Segment #Chi^{2}");
      driftime_h1[j] = make_h1(histlist,Form("DriftTime_%s",htype[j+1]),
			       Form("Drift time %s",htype[j+1]),
			       220, -100., 1000., "ns");
      driftradius_h1[j] = make_h1(histlist,Form("DriftRadius_%s",htype[j+1]),
				  Form("Drift Radius %s",htype[j+1]),
				  200, -20., 20., "mm");
      seg_resi_r_h2[j] = make_h2(histlist, Form("SegResRh2_%s",ebtype[j]),
				 Form("Segment Residuals vs Radius %s",ebtype[j]),
				 100, -15., 15., 100, -500., 500., 
				 "Tube radius [mm]", "Residuals #mum");
      seg_resi_best_h1[j] = make_h1(histlist,Form("SegResAll_best4_%s",ebtype[j]),
				    Form("Segment Residuals %s",ebtype[j]),
				    200, -1000., 1000., "Residuals [#mum]");
    }  //end if j<2
    if( j < 3 ) {   //book barrel, endcap, all histos
      tdc_region_h1[12+j] = make_h1(histlist,Form("TDC_%s",ebtype[j]),
				    Form("%s TDC",ebtype[j]),
				    200, 0., 1400., "TDC [ticks]");
      adc_region_h1[12+j] = make_h1(histlist,Form("ADC_%s",ebtype[j]),
				    Form("%s ADC",ebtype[j]),
				    100, 0., 400., "ADC [ticks]");
      adctdc_region_h2[12+j] = make_h2(histlist,Form("ADCTDC_%s",ebtype[j]),Form("%s ADC vs TDC",ebtype[j]),
				       100,0.,1400.,100,0.,400.,"TDC [ticks]","ADC [ticks]");
      rawtdc_region_h1[12+j] = make_h1(histlist,Form("rawTDC_%s",ebtype[j]),Form("%s raw TDC",ebtype[j]),
				       200, 0., 1400., "TDC [ticks]");
      rawadc_region_h1[12+j] = make_h1(histlist,Form("rawADC_%s",ebtype[j]),Form("%s raw ADC",ebtype[j]),
				       100, 0., 400., "ADC [ticks]");
      rawadctdc_region_h2[12+j] = make_h2(histlist,Form("rawADCTDC_%s",ebtype[j]),Form("%s ADC vs TDC",ebtype[j]),
					  100,0.,1400.,100,0.,400.,"TDC [ticks]","ADC [ticks]");
      rawtdc_region_cut_h1[12+j] = make_h1(histlist,Form("rawTDCcut%d_%s",ADC_CUT,ebtype[j]),
					   Form("%s raw TDC (ADC>%d)",ebtype[j],ADC_CUT),
					   200, 0., 1400., "TDC [ticks]");
      rawadc_region_cut_h1[12+j] = make_h1(histlist,Form("rawADCcat%d_%s",ADC_CUT,ebtype[j]),
					   Form("%s raw ADC (ADC>%d)",ebtype[j],ADC_CUT),
					   100, 0., 400., "ADC [ticks]");
      rawadctdc_region_cut_h2[12+j] = make_h2(histlist,Form("rawADCTDCcut%d_%s",ADC_CUT,ebtype[j]),
					      Form("%s ADC vs TDC (ADC>%d)",ebtype[j],ADC_CUT),
					      100,0.,1400.,100,0.,400.,"TDC [ticks]","ADC [ticks]");
    }  //end if j<3

  }    //end loop over j (segment types)

  trk_author_h1 = make_h1(histlist,"TrkAuthors","Track Authors",20, 200., 220., "Author");
// Loop over track types
  for( int i=0; i<NUM_TRACK_TYPES; i++ ) {
    trk_ntracks_h1[i] = make_h1(histlist,Form("TrkNtracks%s",cauthor[i]),
				Form("Number of Tracks %s",cauthor[i]),
				10, 0., 10., "tracks");
    trk_ntracks_csc_h1[i] = make_h1(histlist,Form("CSCTrkNtracks%s",cauthor[i]),
				Form("Number of CSC Tracks %s",cauthor[i]),
				10, 0., 10., "tracks");
    trk_hits_h1[i] = make_h1(histlist,Form("TrkHits%s",cauthor[i]),
			     Form("MDT hits per track %s",cauthor[i]),
			     100, 0., 100., "MDT hits");
    trk_hits_csc_h1[i] = make_h1(histlist,Form("CSCTrkHits%s",cauthor[i]),
			     Form("CSC hits per track %s",cauthor[i]),
			     100, 0., 100., "CSC hits");
    trk_chambers_h1[i] = make_h1(histlist,Form("TrkChambers%s",cauthor[i]),
			     Form("MDT chambers per track %s",cauthor[i]),
			     10, 0., 10., "MDT Chambers");
    trk_holes_h1[i] = make_h1(histlist,Form("TrkHitHoles%s",cauthor[i]),
			      Form("MDT Holes per track %s",cauthor[i]),
			      100, 0., 100., "MDT holes");
    trk_hit_err_h1[i] = make_h1(histlist,Form("TrkHitError%s",cauthor[i]),
				Form("Track Hit Error %s",cauthor[i]),
				100, 0., 100., "");
    trk_hit_pull_h1[i] = make_h1(histlist,Form("TrkHitPull%s",cauthor[i]),
				 Form("Track Hit Pull %s",cauthor[i]),
				 100, 0., 20., "");
    trk_hit_r_h1[i] = make_h1(histlist,Form("TrkHitDriftRadius%s",cauthor[i]),
			      Form("Track Hit Drift Radius %s",cauthor[i]),
			      100, -15., 15., "mm");
    trk_hit_resi_h1[i] = make_h1(histlist,Form("TrkHitResi%s",cauthor[i]),
				 Form("Track Hit Residuals %s",cauthor[i]),
				 100, -1000., 1000., "#mum");
    trk_phi_h1[i] = make_h1(histlist,Form("TrkPhi%s",cauthor[i]),
			    Form("Track #phi %s",cauthor[i]),
			    100, -3.2, 3.2, "radians");
    trk_phi_barrel_h1[i] = make_h1(histlist,Form("TrkPhiBarrel%s",cauthor[i]),
			    Form("Track #phi Barrel %s",cauthor[i]),
			    100, -3.2, 3.2, "radians");
    trk_phi_endcap_h1[i] = make_h1(histlist,Form("TrkPhiEndcap%s",cauthor[i]),
			    Form("Track #phi Endcap %s",cauthor[i]),
			    100, -3.2, 3.2, "radians");
    trk_eta_h1[i] = make_h1(histlist,Form("TrkEta%s",cauthor[i]),
			    Form("Track #eta %s",cauthor[i]),
			    100, -4., 4., "#eta");
    trk_phi_csc_h1[i] = make_h1(histlist,Form("TrkPhiCSC%s",cauthor[i]),
			    Form("MDT+CSC Track #phi %s",cauthor[i]),
			    100, -3.2, 3.2, "radians");
    trk_eta_csc_h1[i] = make_h1(histlist,Form("TrkEtaCSC%s",cauthor[i]),
			    Form("MDT+CSC Track #eta %s",cauthor[i]),
			    100, -4., 4., "#eta");
    trk_phi_mdt_h1[i] = make_h1(histlist,Form("TrkPhiMDT%s",cauthor[i]),
			    Form("MDT only Track #phi %s",cauthor[i]),
			    100, -3.2, 3.2, "radians");
    trk_eta_mdt_h1[i] = make_h1(histlist,Form("TrkEtaMDT%s",cauthor[i]),
			    Form("MDT only Track #eta %s",cauthor[i]),
			    100, -4., 4., "#eta");
    trk_chamber_h1[i] = make_h1(histlist,Form("TrkPerChamber%s",cauthor[i]),
				Form("Tracks/chamber %s",cauthor[i]),
				MAXCHAMBERS, 0., MAXCHAMBERS, "", "tracks");
    trk_chamber_csc_h1[i] = make_h1(histlist,Form("CSCTrkPerChamber%s",cauthor[i]),
				Form("MDT+CSC Tracks/chamber %s",cauthor[i]),
				MAXCHAMBERS, 0., MAXCHAMBERS, "", "tracks");
    trk_chamber_mdt_h1[i] = make_h1(histlist,Form("MDTTrkPerChamber%s",cauthor[i]),
				Form("MDT only Tracks/chamber %s",cauthor[i]),
				MAXCHAMBERS, 0., MAXCHAMBERS, "", "tracks");

    trk_hit_resi_r_h2[i] = make_h2(histlist, Form("TrkResRh2%s",cauthor[i]),
				   Form("Track Residuals vs Radius %s",cauthor[i]),
				   100, -15., 15., 100, -1000., 1000., 
				   "Tube radius [mm]", "Residuals [#mum]");
    trk_eta_phi_h2[i] = make_h2(histlist, Form("TrkEtaPhih2%s",cauthor[i]),
				Form("Track #eta vs #phi %s",cauthor[i]),
				100, -4., 4., 100, -3.2, 3.2, 
				"#eta", "#phi [rad]");
    trk_eta_phi_csc_h2[i] = make_h2(histlist, Form("TrkEtaPhiCSCh2%s",cauthor[i]),
				Form("MDT+CSC Track #eta vs #phi %s",cauthor[i]),
				100, -4., 4., 100, -3.2, 3.2, 
				"#eta", "#phi [rad]");
    trk_eta_phi_mdt_h2[i] = make_h2(histlist, Form("TrkEtaPhiMDTh2%s",cauthor[i]),
				Form("MDT only Track #eta vs #phi %s",cauthor[i]),
				100, -4., 4., 100, -3.2, 3.2, 
				"#eta", "#phi [rad]");
    trk_eta_phi_tgc_h2[i] = make_h2(histlist, Form("TrkEtaPhiTGCh2%s",cauthor[i]),
				Form("TGC Track #eta vs #phi %s",cauthor[i]),
				100, -4., 4., 100, -3.2, 3.2, 
				"#eta", "#phi [rad]");
    trk_eta_phi_rpc_h2[i] = make_h2(histlist, Form("TrkEtaPhiRPCh2%s",cauthor[i]),
				Form("RPC Track #eta vs #phi %s",cauthor[i]),
				100, -4., 4., 100, -3.2, 3.2, 
				"#eta", "#phi [rad]");
    trk_momentum_h1[i] = make_h1(histlist,Form("TrkMom%s",cauthor[i]),
		      Form("%s Track Momentum",cauthor[i]),
		      200, -100., 100., "Track Momentum [GeV/c]");
  }      //end loop over track types

}        //end Begin()

/***************************************************************/
Bool_t selector::Process(Long64_t entry) {
  char cname[20],cauthor[20];
  int  print=0,print_seg=-1;
  int  trk_types[2] = {TRACK_AUTHOR1,TRACK_AUTHOR2};

// Get event data - this gets the whole ntuple
  fChain->GetTree()->GetEntry(entry);
  nevents[0]++;

// Proceed on if event on list or if no list, if there is a track 
//  if( (ievents && !found) || trk_nTracks==0 ) return kTRUE;
  nevents[1]++;

  //  printf("Run %i Event %i nevents=%i\n",event_runNumber,event_eventNumber,nevents[0]);
  if( runNumber != event_runNumber ) {
    runNumber = event_runNumber;
    printf("****** Run %i\n",runNumber);
  }
  run_h1->Fill(event_runNumber);

/**************************************************/
//  find Segments with MDT hits
/**************************************************/
  std::map<Int_t,std::vector<Int_t> > map_seghit;
  HitSegment( this, entry, map_seghit );
  nsegs_h1[0]->Fill( (int) map_seghit.size() );

  Int_t is, im;
  std::vector<Int_t> vi_hit;
  std::map<Int_t,std::vector<Int_t> >::iterator it_map;
  for( it_map=map_seghit.begin(); it_map!=map_seghit.end(); it_map++ ) {
    is = (*it_map).first;
    vi_hit = (*it_map).second;
    if( seg_author[is] != 4 ) continue;    //unknown segments
    seg_hits_h1[0]->Fill(seg_nMdtHits[is]);
    seg_chi2_h1[0]->Fill(seg_chi2[is]);
//  Fill histos using hits on this segment
    for( unsigned int ihit=0; ihit<vi_hit.size(); ihit++ ) {
      im = vi_hit[ihit];
      seg_resi_all_h1[0]->Fill(1000.*mdt_resi[im]);   //residuals for all segments
    }
  }  //end loop over all segments


/**************************************************/
//  find the "best" Segment for each chamber
/**************************************************/
  BestSegment( this, entry, map_seghit );
  nsegs_h1[1]->Fill((int)map_seghit.size());

/**************************************************/
//  fill histograms
/**************************************************/

// Loop over CSC segments
  csc_nCsc_h1->Fill(csc_nCsc);
  for( int i=0; i<csc_nCsc; i++ ) {
    csc_Z_h1->Fill(csc_gPosZ[i]);
    if( csc_gPosZ[i] > 0. ) {
      csc_YvsX_EIA_h2->Fill(csc_gPosY[i],csc_gPosX[i]);
    } else {
      csc_YvsX_EIC_h2->Fill(csc_gPosY[i],csc_gPosX[i]);
    }
  }

// Loop over TGC segments
  tgc_nTgc_h1->Fill(tgc_nTgc);
  for( int i=0; i<tgc_nTgc; i++ ) {
    tgc_Z_h1->Fill(tgc_gPosZ[i]);
    if( tgc_gPosZ[i] > 7000. ) {
      tgc_YvsX_EMA_h2->Fill(tgc_gPosY[i],tgc_gPosX[i]);
    } else if( tgc_gPosZ[i] > 0. ) {
      tgc_YvsX_EIA_h2->Fill(tgc_gPosY[i],tgc_gPosX[i]);
    } else if( tgc_gPosZ[i] > -7000. ) {
       tgc_YvsX_EIC_h2->Fill(tgc_gPosY[i],tgc_gPosX[i]);
    } else {
      tgc_YvsX_EMC_h2->Fill(tgc_gPosY[i],tgc_gPosX[i]);
    }
  }

// Loop over RPC segments
  rpc_nRpc_h1->Fill(rpcOs_nRpcHits);
  for( int i=0; i<rpcOs_nRpcHits; i++ ) {
    rpc_Z_h1->Fill(rpcOs_gPosZ[i]);
  }

// Loop over segments
// Loop over map of segments
  for( it_map=map_seghit.begin(); it_map!=map_seghit.end(); it_map++ ) {
    is = (*it_map).first;
    vi_hit = (*it_map).second;
    if( seg_author[is] != 4 ) continue;    //unknown segments
    int eb = 0;                  //flag for barrel=0; endcap=1
//  Check if this is a best segment and determine the chamber,wheel of segment
    ChamberName( mdt_id[vi_hit[0]], cname);
//Count Exx, BEE, EE, BIS7/8 as endcap.
    eb = (cname[0]=='E'||cname[1]=='E'|| cname[3]=='7'||cname[3]=='8');
    wheeldata wd = get_chwheel(cname);
    //	printf("event %i seg %i bestchamber=%s wd.iwheel=%i\n", event_eventNumber,is,cname[ib],wd.iwheel);
    if( wd.iwheel < 0 || wd.iwheel > 11 ) {
      printf("   selector_resi ERROR event %i seg %i wd.iwheel=%i\n", event_eventNumber,is,wd.iwheel);
      continue;
    }

// Check for valid T0fit.  Normally if there is no T0Fit seg_fittedT0=-99999
//  Notice that some 3hit segments have non-zero (but small) t0fit
//  This should not be possible.
    Float_t fittedT0 = seg_fittedT0[is];
    int valid_T0fit = ((TMath::Abs(fittedT0) > FITT0CUT) && (fittedT0 > -1000. )) || suppressT0Fitcut;
    if( valid_T0fit ) t0fit_h1[0]->Fill(fittedT0);

// If doing track cut require best segment to be on track
//    printf("ibest=%i trckcut=%i\n",ibest, trackcut);
    Int_t ibest=1;      //At this point all segments are best
    if( trackcut ) {    //if doing track cut, remove segments not on track
//  Find which track this segment is on
      int it = MatchTrack(trk_types[0],wd.idx);
//       printf("  MatchTrack=%i chamber=%s alg=%i",it,chlist[wd.idx].hardname,trk_types[0]);
//       if( it != -1 ) printf(" q/P=%lf",trk_qOverP[it]);
//  Set ibest=0 if segment not on track or track does not pass P cut
//  Note 0.00005 = 1/(20000 MeV) = inverse of P cut
      if( it == -1 || 
	( trackcut==2 && (trk_qOverP[it]==0. || TMath::Abs(trk_qOverP[it]) > 0.00005 )) ) ibest=0;
//      printf(" ibest=%i\n",ibest);
    }

// If this is a best segment
    if( ibest ) {
      seg_hits_h1[1]->Fill(seg_nMdtHits[is]);
      seg_chi2_h1[1]->Fill(seg_chi2[is]);

//  Fill fittedT0 histo if fittedt0 <> 0
      if( valid_T0fit ) {
	seg_hits_h1[2]->Fill(seg_nMdtHits[is]);
	t0fit_h1[1]->Fill(fittedT0);
	t0fit_region_h1[wd.iwheel]->Fill(fittedT0);
	t0fit_chamber_h1[wd.idx]->Fill(fittedT0);
      } else {
	seg_hits_h1[3]->Fill(seg_nMdtHits[is]);
      }
    }   //end if best segment

//  Fill histos using hits on this segment
    for( unsigned int ihit=0; ihit<vi_hit.size(); ihit++ ) {
      im = vi_hit[ihit];
      Float_t resi = 1000.*mdt_resi[im];
      seg_resi_all_h1[1]->Fill(resi);
      driftime_h1[0]->Fill(mdt_t[im]);
      driftradius_h1[0]->Fill(mdt_r[im]);

      seg_hits_chamber_h1->Fill(wd.idx);
      //check if 2nd coordinate has been used to make a wire propagation correction
      if( TMath::Abs(mdt_propTime[im]) > 0.01 ) {
	seg_second_coord_chamber_h1->Fill(wd.idx);
      }

      if( valid_T0fit ) {
	seg_resi_all_h1[2]->Fill(resi);
	driftime_h1[1]->Fill(mdt_t[im]);
	driftradius_h1[1]->Fill(mdt_r[im]);
// Check if segment has enought hits to be a "best" segment
	if( seg_nMdtHits[is] >= SEGMENT_HIT_CUT ) {
//DEBUG  residuals which equal zero
// 	    if( TMath::Abs(resi) == 0.) {
//	    ChamberName(mdt_id[im],cname); 
// 	      segment_author(seg_author[is],cauthor);
// 	      printf("ZERO RESIDUAL %s %s seg=%i hits=%i %.5lf\n",cname,cauthor,is,seg_nMdtHits[is],resi);
// 	      print_seg = is;
// 	    }
	  seg_resi_region_h1[wd.iwheel]->Fill(resi);
	  seg_resi_r_region_h2[wd.iwheel]->Fill(mdt_r[im],resi);
	  seg_resi_best_h1[2]->Fill(resi);
	  seg_resi_best_h1[eb]->Fill(resi);
	  seg_resi_chamber_h1[wd.idx]->Fill(resi);
	  seg_resi_r_pr->Fill(TMath::Abs(mdt_r[im]),resi);
	  seg_resi_r_h2[2]->Fill(mdt_r[im],resi);
	  seg_resi_r_h2[eb]->Fill(mdt_r[im],resi);
	  Double_t fcham = wd.icham;
	  seg_tof_pr[wd.iwheel]->Fill(fcham,mdt_tof[im]);
	  seg_xpos_h2[wd.iwheel]->Fill(fcham,mdt_posX[im]);
	  if( mdt_dr[im] > 0. ) {
//  drTrk is always ZERO
//	      Double_t pull = mdt_resi[im]/TMath::Sqrt(mdt_dr[im]*mdt_dr[im]+mdt_drTrk[im]*mdt_drTrk[im]);
	    Double_t pull = mdt_resi[im]/mdt_dr[im];
	    seg_pull_region_h1[wd.iwheel]->Fill(pull);
	    seg_pull_region_pr[wd.iwheel]->Fill(fcham,pull);
	  }  
//             if( TMath::Abs(resi) < 0.1 ) {
// 	      print=1;
// 	      printf("SMALLRESIDUAL seg=%i hit=%i resi=%f\n",is,im,resi);
// 	    }
	}
      } else {  //else not valid T0fit
	seg_resi_all_h1[3]->Fill(resi);
      }
    }      //end loop over MDTs in segments

  }        //end loop over segments

//  Fill TDCs in raw hits and segments.  Loop over raw hits and find matching segment fit (if any)
//  Fill histo if find match.   Do not just loop over segment hits because hits may be duplicated
  for( int ir=0; ir<rawMdt_nRMdt; ir++ ) {
    int mfid = rawMdt_id[ir]; 
    ChamberName(mfid,cname); 
    int ichamber = checkchamber(cname,1,1,1);
    if( ichamber == -1 ) {
      printf("selector_resi ERROR: Bad raw chamber name run %i event %i hit %i mfid %i\n",
	     event_runNumber,event_eventNumber,ir,mfid);
      continue;
    }
    int eb = 12 + (cname[0]=='E'||cname[1]=='E'||cname[3]=='7'||cname[3]=='8');  //Count Exx, BEE, EE, BIS7/8 as endcap.
    wheeldata wd = get_chwheel(cname);
    float tdc = rawMdt_tdc[ir];
    float adc = rawMdt_adc[ir];
    int tube  = MdtTube(mfid);
    int layer = (MdtML(mfid)-1)*(MDTnLml(cname,1)+1) + MdtLayer(mfid);  //add 1 for ML2 hits to account for gap in histogram
    rawtdc_chamber_h1[ichamber]->Fill(tdc);
    rawadc_chamber_h1[ichamber]->Fill(adc);
    rawadctdc_chamber_h2[ichamber]->Fill(tdc,adc);
    raw_hit_occ_h2[ichamber]->Fill(tube,layer);
    rawtdc_region_h1[wd.iwheel]->Fill(tdc);
    rawadc_region_h1[wd.iwheel]->Fill(adc);
    rawadctdc_region_h2[wd.iwheel]->Fill(tdc,adc);
    rawtdc_region_h1[eb]->Fill(tdc);
    rawadc_region_h1[eb]->Fill(adc);
    rawadctdc_region_h2[eb]->Fill(tdc,adc);
    rawtdc_region_h1[14]->Fill(tdc);
    rawadc_region_h1[14]->Fill(adc);
    rawadctdc_region_h2[14]->Fill(tdc,adc);

    if( adc > ADC_CUT ){
      rawtdc_chamber_cut_h1[ichamber]->Fill(tdc);
      rawadc_chamber_cut_h1[ichamber]->Fill(adc);
      //      rawadctdc_chamber_cut_h2[ichamber]->Fill(tdc,adc);
      raw_hit_occ_cut_h2[ichamber]->Fill(tube,layer);
      rawtdc_region_cut_h1[wd.iwheel]->Fill(tdc);
      rawadc_region_cut_h1[wd.iwheel]->Fill(adc);
      rawadctdc_region_cut_h2[wd.iwheel]->Fill(tdc,adc);
      rawtdc_region_cut_h1[eb]->Fill(tdc);
      rawadc_region_cut_h1[eb]->Fill(adc);
      rawadctdc_region_cut_h2[eb]->Fill(tdc,adc);
      rawtdc_region_cut_h1[14]->Fill(tdc);
      rawadc_region_cut_h1[14]->Fill(adc);
      rawadctdc_region_cut_h2[14]->Fill(tdc,adc);
    }

//  Check if this hit is also on a segment
    for( im=0; im<mdt_nMdt; im++ ) {
      if( mfid == mdt_id[im] ) {
	//	tdc = mdt_tdc[im];   //since it is same as raw hit, ADC/TDC will be the same
	//	adc = mdt_adc[im];
 	tdc_chamber_h1[ichamber]->Fill(tdc);
 	adc_chamber_h1[ichamber]->Fill(adc);
 	adctdc_chamber_h2[ichamber]->Fill(tdc,adc);
	seg_hit_occ_h2[ichamber]->Fill(tube,layer);
 	tdc_region_h1[wd.iwheel]->Fill(tdc);
 	adc_region_h1[wd.iwheel]->Fill(adc);
 	adctdc_region_h2[wd.iwheel]->Fill(tdc,adc);
 	tdc_region_h1[eb]->Fill(tdc);
 	adc_region_h1[eb]->Fill(adc);
 	adctdc_region_h2[eb]->Fill(tdc,adc);
 	tdc_region_h1[14]->Fill(tdc);
 	adc_region_h1[14]->Fill(adc);
 	adctdc_region_h2[14]->Fill(tdc,adc);
 	break;   //break so count segment hits only once.
      }  //end if rawhit found in segment hit
    }  //end loop over segment hits
  }  //end loop over raw hits

// fill track histograms
  Double_t drbest = 100000.;        //best dR between Author1 and Author2
  Double_t detabest = 100000.;      //best deta between Author1 and Author2
  Double_t dphibest = 100000.;      //best dphi between Author1 and Author2
  Float_t ntracks[2] = { 0.,0. };   //number of tracks Author1/Author2
  Float_t ntracks_csc[2] = { 0.,0. }; //number of CSC tracks Author1/Author2
  for( int it=0; it<trk_nTracks; it++ ) {

// Keep track of all the track authors (type) found in ntuple. 
// In Run1 there were numerous track types put in calib ntuple
    int found=0;
    trk_author_h1->Fill(trk_author[it]);
    for( int itt=0; itt<ntrack_types; itt++ ) {
      if( track_types[itt] == trk_author[it] ) {
	found=1;
	break;
      }
    }
    if( !found ) {
      if( ntrack_types < 100 ) {
	track_types[ntrack_types] = trk_author[it];
	//	printf("ntracks=%i it=%4i Track author=%i Ntype=%i\n",trk_nTracks,it,trk_author[it],ntrack_types);
	ntrack_types++;
      } else {
	printf("selector_resi ERROR: TOO many track types! Track author=%i types=%i\n",trk_author[it],ntrack_types);
      }
    }

    int itype = 0;
    if(      trk_author[it] == TRACK_AUTHOR1 ) itype=0;
    else if( trk_author[it] == TRACK_AUTHOR2 && NUM_TRACK_TYPES==2 ) itype=1;
    else continue;

//Track momentum in GeV
    if( trk_qOverP[it] != 0. ) trk_momentum_h1[itype]->Fill(1./trk_qOverP[it]/1000.);
    else                       trk_momentum_h1[itype]->Fill(0.);

// Make track P cut if required
    if( trackcut==2 && (trk_qOverP[it]==0. || TMath::Abs(trk_qOverP[it]) > 0.00005 )) continue;

//Loop over track hits
    Float_t cschits = 0;           //number of CSC hits in track
    Float_t rpchits = 0;           //number of RPC hits in track
    Float_t tgchits = 0;           //number of TGC hits in track
    int ncham_in_trk = 0;          //number of MDT chambers in track
    int chambers_in_trk[20] = {};  //list of MDT chambers in track
    Float_t nhits[2] = {0.,0.};  //mdt hits/holes
    for( int ih=0; ih<trkHit_nHits; ih++ ) {
//  check if hit is on this track and
//  if it is a track hit & an MDT hit OR
//  if it is a track hole & an MDT hit
      if( trkHit_trackIndex[ih]!=it ) continue;
      if( trkHit_type[ih]==1 ) {
	if( TechID(trkHit_id[ih])==0 ) {  //MDT hits
	  nhits[0]++;
	  trk_hit_err_h1[itype]->Fill(trkHit_error[ih]);
	  trk_hit_pull_h1[itype]->Fill(trkHit_pull[ih]);
	  trk_hit_r_h1[itype]->Fill(trkHit_driftRadius[ih]);
	  Double_t resi = 1000.*trkHit_resi[ih];
	  trk_hit_resi_h1[itype]->Fill(resi);
	  ChamberName(trkHit_id[ih],cname);
	  int ichamber = MDTindex(cname);
	  if( ichamber == -1 ) continue;  //skip bad chambers
	  //   Make list of chambers on this track
	  int ich=0;
	  for( ; ich<ncham_in_trk; ich++ ) {
	    if( ichamber == chambers_in_trk[ich] ) break;
	  }
          if( ich == ncham_in_trk && ich < 20) {
	    chambers_in_trk[ich] = ichamber;
	    ncham_in_trk++;
	  }
	  trk_resi_chamber_h1[itype][ichamber]->Fill(resi);
	  trk_hit_resi_r_h2[itype]->Fill(trkHit_driftRadius[ih],resi);
	  wheeldata wd = get_chwheel( cname );
	  trk_resi_region_h1[itype][wd.iwheel]->Fill(resi);
	  trk_resi_r_region_h2[itype][wd.iwheel]->Fill(trkHit_driftRadius[ih],resi);
	} else if( TechID(trkHit_id[ih])==1 ) {  //CSC hits
          cschits++;
	  if( trkHit_posZ[ih] > 0. ) {
	    csc_trk_YvsX_EIA_h2->Fill(trkHit_posY[ih],trkHit_posX[ih]);
	  } else {
	    csc_trk_YvsX_EIC_h2->Fill(trkHit_posY[ih],trkHit_posX[ih]);
	  }

	} else if( TechID(trkHit_id[ih])==2 ) {  //TGC hits
          tgchits++;
	  if( trkHit_posZ[ih] > 7000. ) {
	    tgc_trk_YvsX_EMA_h2->Fill(trkHit_posY[ih],trkHit_posX[ih]);
	  } else if( trkHit_posZ[ih] > 0. ) {
	    tgc_trk_YvsX_EIA_h2->Fill(trkHit_posY[ih],trkHit_posX[ih]);
	  } else if( trkHit_posZ[ih] > -7000. ) {
	    tgc_trk_YvsX_EIC_h2->Fill(trkHit_posY[ih],trkHit_posX[ih]);
	  } else {
	    tgc_trk_YvsX_EMC_h2->Fill(trkHit_posY[ih],trkHit_posX[ih]);
	  }
	} else if( TechID(trkHit_id[ih])==3 ) {  //RPC hits
          rpchits++;
	}
      } else if( trkHit_type[ih]==6 && TechID(trkHit_id[ih])==0 ) {
        nhits[1]++;
      }
    }   //end loop over track hits
    trk_hits_h1[itype]->Fill(nhits[0]);
    trk_holes_h1[itype]->Fill(nhits[1]);
    trk_chambers_h1[itype]->Fill((float)ncham_in_trk);
       
    ntracks[itype]++;
    Double_t eta = -TMath::Log(TMath::Tan(trk_theta[it]/2.));
    trk_phi_h1[itype]->Fill(trk_phi[it]);
    if( TMath::Abs(eta)<1.02 ) trk_phi_barrel_h1[itype]->Fill(trk_phi[it]);
    else                       trk_phi_endcap_h1[itype]->Fill(trk_phi[it]);

    trk_eta_h1[itype]->Fill(eta);
    trk_eta_phi_h2[itype]->Fill(eta,trk_phi[it]);
    for( int ich=0; ich<ncham_in_trk; ich++ ) {
      trk_chamber_h1[itype]->Fill(chambers_in_trk[ich]);
    }
    if( cschits == 0. ) {
      trk_phi_mdt_h1[itype]->Fill(trk_phi[it]);
      trk_eta_mdt_h1[itype]->Fill(eta);
      trk_eta_phi_mdt_h2[itype]->Fill(eta,trk_phi[it]);
      for( int ich=0; ich<ncham_in_trk; ich++ ) {
	trk_chamber_mdt_h1[itype]->Fill(chambers_in_trk[ich]);
      }
    } else {
      trk_hits_csc_h1[itype]->Fill(cschits);
      ntracks_csc[itype]++;
      trk_phi_csc_h1[itype]->Fill(trk_phi[it]);
      trk_eta_csc_h1[itype]->Fill(eta);
      trk_eta_phi_csc_h2[itype]->Fill(eta,trk_phi[it]);
      for( int ich=0; ich<ncham_in_trk; ich++ ) {
	trk_chamber_csc_h1[itype]->Fill(chambers_in_trk[ich]);
      }
    }
    if( tgchits ) trk_eta_phi_tgc_h2[itype]->Fill(eta,trk_phi[it]);
    if( rpchits ) trk_eta_phi_rpc_h2[itype]->Fill(eta,trk_phi[it]);

//  Compare tracks if there is a 2nd track type in histo
    if( NUM_TRACK_TYPES == 2 ) {
      for( int it2=it+1; it2<trk_nTracks; it2++ ) {
	int itype2 = 0;
	if(      trk_author[it2] == TRACK_AUTHOR1 ) itype2=0;
	else if( trk_author[it2] == TRACK_AUTHOR2 && NUM_TRACK_TYPES==2 ) itype2=1;
	else continue;
	Double_t deta = -TMath::Log(TMath::Tan(trk_theta[it2]/2.)) - eta;
	Double_t dphi = trk_phi[it2] - trk_phi[it];
	Double_t dr   = TMath::Sqrt( deta*deta + dphi*dphi );
	if( itype == 1 ) {
	  deta = -deta;
	  dphi = -dphi;
	}
	if( itype != itype2 ) {
	  trk_dR_Author1Author2_h1[0]->Fill(dr);
	  trk_deta_Author1Author2_h1[0]->Fill(deta);
	  trk_dphi_Author1Author2_h1[0]->Fill(dphi);
	  if( dr < drbest) drbest = dr;
	  if( deta < detabest) detabest = deta;
	  if( dphi < dphibest) dphibest = dphi;
	} else if( itype==itype2 && itype==1 ) {
	  trk_dR_Combined_h1->Fill(dr);
	  trk_deta_Combined_h1->Fill(deta);
	  trk_dphi_Combined_h1->Fill(dphi);
	}
      }   //end secondary loop over tracks
    }     //end if NUM_TRACK_TYPES == 2
  }     //end loop over tracks
  for( int i=0; i<NUM_TRACK_TYPES; i++ ) {
    trk_ntracks_h1[i]->Fill(ntracks[i]);
    trk_ntracks_csc_h1[i]->Fill(ntracks_csc[i]);
  }  

//  Compare tracks if there is a 2nd track type in histo
  if( NUM_TRACK_TYPES == 2 && drbest < 100000. ) {
    trk_dR_Author1Author2_h1[1]->Fill(drbest);
    trk_deta_Author1Author2_h1[1]->Fill(detabest);
    trk_dphi_Author1Author2_h1[1]->Fill(dphibest);
  }
//  if( ntracks[1] > 1 ) print=1;

// print_seg is set by debug code above
  if( print_seg != -1 ) PrintSeg(print_seg);

// Return if not doing verbose option
  if( verbose==1 || print==1 ) {

  printf("************ Run %i Event %i Trks=%i TrkHits=%i TrkSegs=%i Segs=%i MDT/RPC/TGC/CSC=%i/%i/%i/%i\n",
	 event_runNumber,event_eventNumber,trk_nTracks,trkHit_nHits,trkSeg_nTrkSegs,
         seg_nSegments,mdt_nMdt,rpc_fchan,tgc_nTgc,csc_nCsc);

//  Count all the crap in ALL tracks
  int nhits=0,hit=0,out=0,hole=0,RIO=0,Com=0,delta=0,phisc=0,thesc=0,unk=0,pseudo=0;
  for( int ih=0; ih<trkHit_nHits; ih++ ) { 
    nhits++;
    if(      trkHit_type[ih]==1 ) hit++;
    else if( trkHit_type[ih]==4 ) out++;
    else if( trkHit_type[ih]==6 ) hole++;
    else if( trkHit_type[ih]==11 ) phisc++;
    else if( trkHit_type[ih]==12 ) thesc++;
    else if( trkHit_type[ih]==-2 ) RIO++;
    else if( trkHit_type[ih]==2 ) Com++;
    else if( trkHit_type[ih]==3 ) pseudo++;
    else if( trkHit_type[ih]==5 ) delta++;
    else unk++;
  }  //end loop over track hits

// Print one-line track info for all tracks
  printf("    Track Hit Summary NHits=%i HitOnTrack=%i Outlier=%i Hole=%i SCphi=%i SCtheta=%i RIO=%i Com=%i DeltaE=%i Pseudo=%i UnknownHit=%i\n",
	 nhits,hit,out,hole,phisc,thesc,RIO,Com,delta,pseudo,unk);

// Loop over tracks
  for( int it=0; it<trk_nTracks; it++ ) {
 
    track_author(trk_author[it],cauthor);

//  Count all the crap in tracks
    nhits=0,
    hit=0,
    out=0,
    hole=0,RIO=0,Com=0,delta=0,phisc=0,thesc=0,unk=0,pseudo=0;
    for( int ih=0; ih<trkHit_nHits; ih++ ) { 
//  skip types: 1=hit 4=outlier 6=hole
      if( trkHit_trackIndex[ih]!=it ) continue;
      nhits++;
      if(      trkHit_type[ih]==1 ) hit++;
      else if( trkHit_type[ih]==4 ) out++;
      else if( trkHit_type[ih]==6 ) hole++;
      else if( trkHit_type[ih]==11 ) phisc++;
      else if( trkHit_type[ih]==12 ) thesc++;
      else if( trkHit_type[ih]==-2 ) RIO++;
      else if( trkHit_type[ih]==2 ) Com++;
      else if( trkHit_type[ih]==3 ) pseudo++;
      else if( trkHit_type[ih]==5 ) delta++;
      else unk++;
    }  //end loop over track hits

    Float_t p = 0.;
    if( trk_qOverP[it] != 0. ) p = 1./trk_qOverP[it]/1000.;   //convert to GeV
 
// Print one-line track info for all tracks
    printf("  ---Track %i %s(%i) NHits=%i HitOnTrack=%i Outlier=%i Hole=%i SCphi=%i SCtheta=%i RIO=%i Com=%i DeltaE=%i Pseudo=%i UnknownHit=%i q/p=%8.1f\n",
	   it,cauthor,trk_author[it],nhits,hit,out,hole,phisc,thesc,RIO,Com,delta,pseudo,unk,p);
// Print segment info for Author1 and Author2 tracks
//    if( !(trk_author[it] == TRACK_AUTHOR2 || trk_author[it]==TRACK_AUTHOR2) ) continue;
//  Print track hits, holes, outliers for real chambers
    int ihit=0;
    for( int ih=0; ih<trkHit_nHits; ih++ ) {
      if( trkHit_trackIndex[ih]!=it ) continue;
      if(ihit==0) printf("        hit Chamber-ML-L-T  rTrk    resi   pull  ADC  TDC\n"); //    t      r    resi\n");
      ihit++;
      track_hittype(trkHit_type[ih],cauthor);
      int id = trkHit_id[ih];
//  If hit is on track and is MDT chamber hit.
//  type: 1=hit 4=outlier 6=hole
      if( trkHit_type[ih]==1 || trkHit_type[ih]==4 || trkHit_type[ih]==6 ) {;
        StationName(id,cname);
        if( cname[0] != 'X' ) {  
	  printf("     %3i %c%c%c %-14s %6.2f %6.2f %6.2f",ihit,cauthor[0],cauthor[1],cauthor[2],
	       cname, trkHit_driftRadius[ih], 1000.*trkHit_resi[ih], trkHit_pull[ih]);
        } else {   //unknown muonfixedid
	  cTechID(id,cname);
  	  printf("     %3i %c%c%c %s ID=%14i %6.2f %6.2f %6.2f",ihit,cauthor[0],cauthor[1],cauthor[2],
		 cname, id, trkHit_driftRadius[ih], 1000.*trkHit_resi[ih], trkHit_pull[ih]);
	}
// if hit or outlier find MDT raw hit info
	if( TechID(id)==0 && trkHit_type[ih]!=6 ) {
	  for( int ir=0; ir<rawMdt_nRMdt; ir++ ) {     //loop over rawMDT hits
	    if( id != rawMdt_id[ir] ) continue;
	    printf(" %3i %4i",rawMdt_adc[ir],rawMdt_tdc[ir]);
	    break;
// OLD // Feb, 2012 - Can't do this any more - no more rawMdt_t, rawMdt_r 
// 	  printf(" %3i %4i %6.2f %6.2f %7.3f",
// 		 rawMdt_adc[ir],rawMdt_tdc[ir],rawMdt_t[ir],rawMdt_r[ir],
// 		 rawMdt_r[ir]-trkHit_driftRadius[ih] );
// 		 TMath::Abs(rawMdt_r[ir])-TMath::Abs(trkHit_driftRadius[ih]));
	  }  //end loop over rawMDT hits
	}
	printf("\n");
//  Else Print all the other crap in tracks
      } else {    //else some other kind of hit
        printf("     %3i %-18s %6.2f %6.2f %6.2f\n",ihit,cauthor,
	       trkHit_driftRadius[ih], 1000.*trkHit_resi[ih], trkHit_pull[ih]);
      } 
    }  //end loop over track hits

//  Print all the other crap in tracks
//    for( int ih=0; ih<trkHit_nHits; ih++ ) { 
//  skip types: 1=hit 4=outlier 6=hole
//      if( trkHit_trackIndex[ih]!=it || 
//	  (trkHit_type[ih]==1 || trkHit_type[ih]==4 || trkHit_type[ih]==6) ) continue;
//      if(ih==0) printf("         TrackObject     rTrk    resi   pull\n");
//      track_hittype(trkHit_type[ih],cauthor);
//      Chamber-ML-L-T   rTrk Trkresi TrkPull  
//  xxxxxxxxxxxxxxxxxx  fff.ff fff.ff fff.ff  
//      printf("     %-18s %6.2f %6.2f %6.2f\n",cauthor,
//	     trkHit_driftRadius[ih], trkHit_resi[ih], trkHit_pull[ih]);
//    }  //end loop over track hits

//  Segments connected to tracks
    for( int its=0; its<trkSeg_nTrkSegs; its++ ) {    //loop over TrkSegs
      if( trkSeg_trkIndex[its] != it ) continue;
      is = trkSeg_segIndex[its];
      PrintSeg(is,it);
    }     //end loop over TrkSegs

  }    //end loop over tracks

//  Segment information
//  for( int is=0; is<seg_nSegments; is++ ) {
  for( is=0; is<0; is++ ) {    //turn off for now
    if( seg_author[is]!=3 && seg_author[is]!=4 ) continue;    //unknown segments
    segment_author(seg_author[is],cauthor);
//  Check if is one of the best segments - put a "*" in front of best segments
    char cbest[3] = " *";
    int  ibest = 0;
    for( it_map=map_seghit.begin(); it_map!=map_seghit.end(); it_map++ ) {
      if( (*it_map).first == is ) {
	ibest = 1;
	break;
      }
    }
    printf(" %cSegment %i %s Hits=%i/%i Qual=%i Chi2=%6.3f FitT0=%6.2f\n",
	   cbest[ibest],is,cauthor,seg_nMdtHits[is],seg_nHits[is],
	   seg_quality[is],seg_chi2[is],seg_fittedT0[is]);
//  Loop MDT hits in segments
    for( im=0; im<mdt_nMdt; im++ ) {
      if(im==0) printf("  MDT Chamber-ML-L-T ADC  TDC  T0   dT0    t      r     rTrk   resi\n");
      if( mdt_segIndex[im] != is ) continue;  //skip hit if not in this segment
      TubeName(mdt_id[im],cname);
//  MDT Chamber-ML-L-T ADC  TDC  T0   dT0    t      r     rTrk   resi
//  MDT EML3A07-1-1-40 120  275 500 fff.ff ffff.f fff.ff fff.ff fffff
      printf("  MDT %-14s %3i %4i %3.0f %6.2f %6.1f %6.2f %6.2f %5.0f\n",
	     cname, mdt_adc[im], mdt_tdc[im], mdt_tube_t0[im],
	     mdt_tof[im],mdt_t[im],mdt_r[im],mdt_rTrk[im],
	     1000.*mdt_resi[im]);
//       Float_t t = mdt_tdc[im]*0.78125 - mdt_tube_t0[im];
//       t -= mdt_slewTime[im];
//       t -= mdt_lorTime[im];
//       t -= mdt_propTime[im];
//       t -= mdt_tof[im];
// Event Chamber         hit seg   t     tcheck    r     rcheck  dr
//444639 EIL4C13-1-1-23    8  1  482.17 1177.77  11.674  11.473 -200.88
//       int imm = seg_author[ mdt_segIndex[im] ] - 3;
//       printf("%6i %s  %3i %2i %i %7.2lf %7.2lf\n",
// 	     event_eventNumber,cname,im,mdt_segIndex[im],imm,mdt_t[im],t);
//       printf("      %3i %4i %5.1f %5.2f %5.2f %5.2f %5.2f\n",
// 	     mdt_adc[im], mdt_tdc[im], mdt_tube_t0[im],mdt_slewTime[im],
// 	     mdt_lorTime[im],mdt_propTime[im],mdt_tof[im]);
    }   //end loop over MDT hits
  }     //end loop over segments

// Return if not doing verbose option  } else if( verbose == 2 ) {

  printf("************ Run %i Event %i Trks=%i TrkHits=%i Segs=%i MDT/RPC/TGC/CSC=%i/%i/%i/%i\n",
	 event_runNumber,event_eventNumber,trk_nTracks,trkHit_nHits,
         seg_nSegments,mdt_nMdt,rpc_fchan,tgc_nTgc,csc_nCsc);

//  Segment information 
  for( it_map=map_seghit.begin(); it_map!=map_seghit.end(); it_map++ ) {
    is = (*it_map).first;
    vi_hit = (*it_map).second;
    if( seg_author[is] != 4 ) continue;    //only Moore segments 

//  Find a chamber segment
    Int_t mfid = mdt_id[ map_seghit[is][0] ];
    ChamberName( mfid,cname);

    printf(" Segment %s %i Hits=%i MDT=%i TGC=%i RPC=%i Chi2=%6.3f FitT0=%6.2f\n",
	   cname,is,seg_nHits[is],seg_nMdtHits[is],seg_nTgcHits[is],seg_nRpcHits[is],
	   seg_chi2[is],seg_fittedT0[is]);
// Print hit data
    printf("  MDT Chamber-ML-L-T ADC  TDC  T0   dT0    t      r     rTrk   resi\n");
//  MDT Chamber-ML-L-T ADC  TDC  T0       t            r           rTrk          resi
//  MDT EML3A07-1-1-40 120  275 500 ffff.f/fff.f fff.ff/fff.ff fff.ff/fff.ff fffff/fffff
    for( unsigned int i=0; i<vi_hit.size(); i++ ) {
      im = vi_hit[i];
      TubeName(mdt_id[im],cname);
      printf("  MDT %-14s %3i %4i %3.0f %6.1f %6.2f %6.2f %5.0f\n",
	     cname, mdt_adc[im], mdt_tdc[im], mdt_tube_t0[im],
	     mdt_t[im],mdt_r[im],mdt_rTrk[im],1000.*mdt_resi[im]);
    }  //end loop over segment hits
    
  }   //end loop over MDT hits
  }
//     }     //end loop over Standalone segments
// }       //end loop over Combined segments
 
  return kTRUE;
}

/**************************************************************/
// The Terminate() function is the last function to be called during
// a query. It always runs on the client, it can be used to present
// the results graphically or save the results to file.
/**************************************************************/
void selector::Terminate() { 
  char cdir[80] = "";

  printf("Processed %i/%i events\n",nevents[1],nevents[0]);
// Save histograms to a file
//  Open ROOT file
  TFile *f = new TFile("resi.root","recreate");
  if (f->IsZombie()) {   
    printf("selector_resi ERROR opening ROOT output file\n");
    return;
  }
//  Save all histgrams in histlist
//  TIter next(histlist);
//  TObject *obj;
//  while( (obj=next()) ) obj->Write();

// General histograms
  run_h1->Write();

// Save things in different directories
  f->cd();
  f->mkdir("Raw");
  f->cd("Raw");
  for( int i=0; i<15; i++ ) {
    rawtdc_region_h1[i]->Write();
    rawadc_region_h1[i]->Write();            
    rawadctdc_region_h2[i]->Write();         
    rawtdc_region_cut_h1[i]->Write();        
    rawadc_region_cut_h1[i]->Write();        
    rawadctdc_region_cut_h2[i]->Write();     
  }

  f->cd();
  f->mkdir("Segments");
  f->cd("Segments");
  seg_resi_r_pr->Write(); 
  //  seg_second_coord_chamber_h1->Divide(seg_hits_chamber_h1);
  seg_second_coord_chamber_h1->Write();
  seg_hits_chamber_h1->Write();
  for( int i=0; i<15; i++ ) {
    if( i<2 ) {
      t0fit_h1[i]->Write();
      nsegs_h1[i]->Write();
      seg_chi2_h1[i]->Write();    
      driftime_h1[i]->Write();    
      driftradius_h1[i]->Write(); 
    }
    if( i<3 ) {
      seg_resi_best_h1[i]->Write();
      seg_resi_r_h2[i]->Write();  
    }
    if( i<4 ) {
      seg_resi_all_h1[i]->Write();
      seg_hits_h1[i]->Write();
    }
    if( i<12 ) {
      seg_resi_region_h1[i]->Write();  
      seg_resi_r_region_h2[i]->Write();
      seg_pull_region_h1[i]->Write();  
      seg_pull_region_pr[i]->Write();  
      t0fit_region_h1[i]->Write();
      seg_tof_pr[i]->Write();
      seg_xpos_h2[i]->Write();
    }
    tdc_region_h1[i]->Write();
    adc_region_h1[i]->Write();   
    adctdc_region_h2[i]->Write();
  }

  f->cd();
  f->mkdir("Tracks");
  f->cd("Tracks");
  trk_author_h1->Write();
  for( int i=0; i<NUM_TRACK_TYPES; i++ ) {
    trk_ntracks_h1[i]->Write();
    trk_ntracks_csc_h1[i]->Write();
    trk_hits_h1[i]->Write();
    trk_hits_csc_h1[i]->Write();
    trk_chambers_h1[i]->Write();
    trk_chamber_h1[i]->Write();
    trk_chamber_csc_h1[i]->Write();
    trk_chamber_mdt_h1[i]->Write();
    trk_holes_h1[i]->Write();
    trk_hit_err_h1[i]->Write();
    trk_hit_pull_h1[i]->Write();
    trk_hit_r_h1[i]->Write();
    trk_hit_resi_h1[i]->Write();
    trk_hit_resi_r_h2[i]->Write();
    trk_phi_h1[i]->Write();
    trk_phi_barrel_h1[i]->Write();
    trk_phi_endcap_h1[i]->Write();
    trk_phi_csc_h1[i]->Write();
    trk_phi_mdt_h1[i]->Write();
    trk_eta_h1[i]->Write();
    trk_eta_csc_h1[i]->Write();
    trk_eta_mdt_h1[i]->Write();
    trk_eta_phi_h2[i]->Write();
    trk_eta_phi_csc_h2[i]->Write();
    trk_eta_phi_mdt_h2[i]->Write();
    trk_eta_phi_rpc_h2[i]->Write();
    trk_eta_phi_tgc_h2[i]->Write();
    trk_momentum_h1[i]->Write();
    if( i ) {
      trk_dR_Combined_h1->Write();
      trk_deta_Combined_h1->Write();
      trk_dphi_Combined_h1->Write();
      trk_dR_Author1Author2_h1[i]->Write();
      trk_deta_Author1Author2_h1[i]->Write();
      trk_dphi_Author1Author2_h1[i]->Write();
    }
    for( int j=0; j<12; j++ ) {
      trk_resi_region_h1[i][j]->Write();  
      trk_resi_r_region_h2[i][j]->Write();
    }
  } 

  f->cd();
  f->mkdir("Chamber");
  for( int i=0; i<MAXCHAMBERS; i++ ) {
    sprintf(cdir,"Chamber/%s",chlist[i].hardname);
    f->cd();
    f->mkdir(cdir);
    f->cd(cdir);
    rawtdc_chamber_h1[i]->Write();
    rawadc_chamber_h1[i]->Write();
    rawadctdc_chamber_h2[i]->Write();
    raw_hit_occ_h2[i]->Write();     
    rawtdc_chamber_cut_h1[i]->Write();
    rawadc_chamber_cut_h1[i]->Write();
    //    rawadctdc_chamber_cut_h2[i]->Write();
    raw_hit_occ_cut_h2[i]->Write();
    tdc_chamber_h1[i]->Write();   
    adc_chamber_h1[i]->Write();   
    adctdc_chamber_h2[i]->Write();
    seg_hit_occ_h2[i]->Write();
    seg_resi_chamber_h1[i]->Write();
    t0fit_chamber_h1[i]->Write();
    for( int j=0; j<NUM_TRACK_TYPES; j++ ) {
      trk_resi_chamber_h1[j][i]->Write();   
    }
  }

  f->cd();
  f->mkdir("Misc");
  f->cd("Misc");
  tgc_nTgc_h1->Write();
  csc_nCsc_h1->Write();
  rpc_nRpc_h1->Write();
  csc_Z_h1->Write();
  tgc_Z_h1->Write();
  rpc_Z_h1->Write();
  csc_YvsX_EIA_h2->Write();
  csc_YvsX_EIC_h2->Write(); 
  tgc_YvsX_EIA_h2->Write();
  tgc_YvsX_EIC_h2->Write();
  tgc_YvsX_EMA_h2->Write();
  tgc_YvsX_EMC_h2->Write();
  csc_trk_YvsX_EIA_h2->Write();
  csc_trk_YvsX_EIC_h2->Write(); 
  tgc_trk_YvsX_EIA_h2->Write();
  tgc_trk_YvsX_EIC_h2->Write();
  tgc_trk_YvsX_EMA_h2->Write();
  tgc_trk_YvsX_EMC_h2->Write();

  f->Close();
  
  char cauthor[100];
  printf(" ******** List of track types found Total Types=%i\n",ntrack_types);
  for( int itt=0; itt<ntrack_types; itt++ ) {
    track_author(track_types[itt], cauthor);
    printf("%3i Type=%4i %s\n",itt,track_types[itt], cauthor);
  }

}

//  Determine if chamber is on a track of algorthm alg.
//  Returns index of matched track in trk_
//  Returns  -1 if no track found.
//  Does not consider the case of a chamber being included in 2 tracks.
//  which should not happen much.
int selector::MatchTrack(int alg, int ichamber) {
  char cname[10];
  for( int it=0; it<trk_nTracks; it++ ) {
    if( trk_author[it] != alg ) continue;
//Loop over track hits
    for( int ih=0; ih<trkHit_nHits; ih++ ) {
//  check if hit is an MDT hit on current track
      if( trkHit_trackIndex[ih]!=it || 
	  trkHit_type[ih]!=1 || 
	  TechID(trkHit_id[ih])!=0 ) continue;
      ChamberName(trkHit_id[ih],cname);
      if( ichamber == checkchamber(cname,1,1,1) ) return it;
    }   //end loop over track hits
  }     //end loop over tracks
  return -1;
}       //end MatchTrack

//  Print 1 or more segments from event
//    iseg = segment index in seg_XXX
//    itrk = track index in trk_XXX
void selector::PrintSeg(int iseg, int itrk) { 
  int iseg1,iseg2;
  char cname[20],cauth[20];

  //  printf("************ Run %i Event %i Trks=%i TrkHits=%i Segs=%i MDT/RPC/TGC/CSC=%i/%i/%i/%i\n",
  //	 event_runNumber,event_eventNumber,trk_nTracks,trkHit_nHits,
  //         seg_nSegments,mdt_nMdt,rpc_fchan,tgc_nTgc,csc_nCsc);

  if( iseg == -1 ) {
    iseg1 = 0;
    iseg2 = seg_nSegments;
  } else {
    iseg1 = iseg;
    iseg2 = iseg+1;
  }
  
//  Segment information
  for( int is=iseg1; is<iseg2; is++ ) {
    if( seg_author[is]!=3 && seg_author[is]!=4 ) continue;    //unknown segments
    segment_author(seg_author[is],cauth);
//  Check if is one of the best segments - put a "*" in front of best segments
    printf(" Segment %i %s Hits=%i/%i Qual=%i Chi2=%6.3f FitT0=%6.2f\n",
	   is,cauth,seg_nMdtHits[is],seg_nHits[is],
	   seg_quality[is],seg_chi2[is],seg_fittedT0[is]);
//  Loop MDT hits in segments
    for( int im=0; im<mdt_nMdt; im++ ) {
      if(im==0) printf("  MDT Chamber-ML-L-T ADC  TDC  T0   dT0    t    rDrift  rSeg  SegRes trkRes  trkR trkPull\n");
      if( mdt_segIndex[im] != is ) continue;  //skip hit if not in this segment
//  check if segment hit is also a track hit
      int itrkhit = -1;
      char ctrk = ' ';
//if track index is supplied
      if( itrk > -1 ) {         
	for( int ih=0; ih<trkHit_nHits; ih++ ) {
	  if( trkHit_trackIndex[ih]==itrk && trkHit_id[ih] == (unsigned int) mdt_id[im]) {
	    itrkhit = ih;
	    ctrk = '*';
	    break;
	  } 
	}   //end loop over track index
      }     //end if track index supplied

//  MDT Chamber-ML-L-T ADC  TDC  T0   dT0    t    rDrift  rSeg  SegRes trkRes  trkR trkPull 
//  MDT EML3A07-1-1-40 120  275 500 fff.ff ffff.f fff.ff fff.ff ffffff ffffff fff.ff ttt.ff
      TubeName(mdt_id[im],cname);
      printf(" %cMDT %-14s %3i %4i %3.0f %6.2f %6.1f %6.2f %6.2f %6.0f",
	     ctrk,cname, mdt_adc[im], mdt_tdc[im], mdt_tube_t0[im],
	     mdt_tof[im],mdt_t[im],mdt_r[im],mdt_rTrk[im],
	     1000.*mdt_resi[im]);
      if( itrk > -1 && itrkhit > -1 ) {
	printf(" %6.0f %6.2f %6.2f",1000*trkHit_resi[itrkhit],trkHit_driftRadius[itrkhit],trkHit_pull[itrkhit]); 
      }
      printf("\n");
    }   //end loop over MDT hits
  }     //end loop over segments

}   //end selector::Print

void selector::SlaveBegin(TTree * ) {}
void selector::SlaveTerminate() {}
