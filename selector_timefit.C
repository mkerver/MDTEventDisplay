/**********************************************************
 selector_timefit.C

   Make time and ADC histograms for calibrations 
    output: time.root containing histograms

   Use only hits which appear on tracks
   Book histograms for chamber, ML, mezzcards
   Use timefit.C to do T0, Tmax, and ADC fits

   Edward Diehl 14-Aug-2014

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
#include "/atlas/data18c/muoncal/root/macros/edplot.h"
#include "/atlas/data18c/muoncal/root/macros/muonfixedid.h"
#include "/atlas/data18c/muoncal/root/macros/chamberlist.h"

int nevents[2] = {0,0};
//const int   SEGMENT_HIT_CUT=6;    //min segment hits for best segment
const int   TRACK_AUTHOR1=200;    //Run2: 206=RAW data; 200=calibstream 
const int   TRACK_AUTHOR2=208;    //3rd chain muon combined track (?)
// Time plot range: -100 to 900 ns  1280 bins make 25/32 ns bin size (TDC resolution)
//const int   TIME_BINS=400;      // number of bins for time plots.  Time range=-100 to 900 ns
const int   TIME_BINS=640;        // number of bins for time plots.  Time range=-100 to 900 ns
const int   ADC_BINS=200;         // number of bins for ADC plots
int invertcut=0;                  // invert P cut

// Histograms
TH1F *chamber_hits_h1;             //hits per chamber    
TH1F *mezz_hits_h1[MAXCHAMBERS];   //hits per mezzcard   
TH1F *segment_hits_h1;             //hits per track segment    
TH1F *segment_hitsC_h1;            //hits per track segment (combine hits in adjacent L+S chambers)    
TH1F *track_hits_h1;               //MDT hits per track
TH1F *track_chambers_h1;           //MDT chambers per track

// Chamber Time histos (T0 is subtracted from time)
TH1F *chamber_time_h1[MAXCHAMBERS];   
TH1F *ml_time_h1[MAXCHAMBERS][2];     
TH1F *mezz_time_h1[MAXCHAMBERS][26];    

TH1F *chamber_dtime_h1[MAXCHAMBERS];   //segment time - track time

// Chamber ADC histos
TH1F *chamber_ADC_h1[MAXCHAMBERS];   
TH1F *ml_ADC_h1[MAXCHAMBERS][2];     
TH1F *mezz_ADC_h1[MAXCHAMBERS][26];    

// Chamber pt histos
TH1F *trk_author_h1;     //track author 
TH1F *trk_tracks_h1;     //number of tracks 
TH1F *trk_p_h1;          //track momentum
TH1F *trk_pt_h1;         //track transverse momentum
TH1F *trk_phi_h1;        //track phi angle 
TH1F *trk_eta_h1;        //psuedo-rapidity 
TH2F *trk_eta_phi_h2;    //eta vs phi
TH2F *trk_p_phi_h2;      //momentum vs phi
TH2F *trk_p_eta_h2;      //momentum vs eta
TH2F *trk_pt_phi_h2;     //momentum vs phi
TH2F *trk_pt_eta_h2;     //momentum vs eta
TH1F *chamber_p_h1[MAXCHAMBERS];   
TH1F *chamber_pt_h1[MAXCHAMBERS];   

/**************************************************************/
// Book histos, read event list
/**************************************************************/
void selector::Begin(TTree *) {
  char copt[80];

  printf("Running selector_timefit.C\n");

  TString option = GetOption();
  sprintf(copt,"%s",option.Data());
  for( int i=0; copt[i]!='\0'; i++ ) {
    if( copt[i] == 'I' ) {
      invertcut = 1;
      printf("INVERTING Track P cut\n");
    } else {
      printf("WARNING: Unknown option %c\n",copt[i]);
    }  
  }    //end loop over options

//  Book histos

  chamber_hits_h1 = make_h1("chamber_hits", "Chamber hits", MAXCHAMBERS, 0., MAXCHAMBERS, "Chamber");
  segment_hits_h1 = make_h1("segment_hits", "Hits per track segment", 20, 0., 20, "hits");
  segment_hitsC_h1 = make_h1("segment_hits_comb", "Hits per track segment (L+S)", 20, 0., 20, "hits");
  track_hits_h1   = make_h1("track_hits", "MDT hits per track", 50, 0., 50, "hits");
  track_chambers_h1 = make_h1("track_chambers", "MDT chambers per track", 10, 0., 10, "chambers");
  trk_tracks_h1   = make_h1("Trk_Tracks","Number of Tracks", 10, 0., 10., "Tracks");
  trk_author_h1   = make_h1("Trk_Author","Track Author", 12, 200., 212., "Track Author");
  trk_p_h1        = make_h1("Trk_Momentum","Track Momentum", 100, 0., 100., "Track Momentum [GeV/c]");
  trk_pt_h1       = make_h1("Trk_Pt","Track Pt", 100, 0., 100., "Track Pt [GeV/c]");
  trk_phi_h1      = make_h1("Trk_Phi","Track #phi", 100, -3.2, 3.2, "radians");
  trk_eta_h1      = make_h1("Trk_Eta","Track #eta", 100, -4., 4., "#eta");
  trk_eta_phi_h2  = make_h2("Trk_Eta_Phi","Track #eta vs #phi",
			   100, -4., 4., 100, -3.2, 3.2, "#eta", "#phi [rad]");
  trk_p_phi_h2 = make_h2("Trk_P_Phi","Track P vs #phi",
			 100, -3.2, 3.2, 100, 0., 100., "#phi [rad]", "Track P [GeV/c]");
  trk_p_eta_h2 = make_h2("Trk_P_Eta","Track P vs #eta",
			   100, -4., 4., 100, 0., 100., "#eta", "Track P [GeV/c]");
  trk_pt_phi_h2 = make_h2("Trk_Pt_Phi","Track Pt vs #phi",
			  100, -3.2, 3.2, 100, 0., 100., "#phi [rad]", "Track Pt [GeV/c]");
  trk_pt_eta_h2 = make_h2("Trk_Pt_Eta","Track Pt vs #eta",
			  100, -4., 4., 100, 0., 100., "#eta", "Track Pt [GeV/c]");

// loop over chambers
  for( int i=0; i<MAXCHAMBERS; i++ ) { 
    int nml   = MDTnML(chlist[i].hardname);
    int nmezz = MDTmaxmezz(chlist[i].hardname);

    chamber_p_h1[i] = make_h1(Form("%s_Momentum",chlist[i].hardname), 
			   Form("%s Momentum",chlist[i].hardname),
			   100, 0., 100., "Track Momentum [GeV/c]");
    chamber_pt_h1[i] = make_h1(Form("%s_Pt",chlist[i].hardname), 
			   Form("%s Pt",chlist[i].hardname),
			   100, 0., 100., "Track Pt [GeV/c]");

    mezz_hits_h1[i] = make_h1(Form("%s_Mezz_Hits",chlist[i].hardname), 
			      Form("%s Mezz hits",chlist[i].hardname),
			      nmezz, 0., nmezz, "Mezz card");

    chamber_dtime_h1[i] = make_h1(Form("%s_dTime",chlist[i].hardname),
				Form("%s Segment-Track time",chlist[i].hardname),
				100, -20., 20., "[ns]");

    chamber_time_h1[i] = make_h1(Form("%s_Time",chlist[i].hardname),
				Form("%s time",chlist[i].hardname),
				TIME_BINS, -100., 900., "Time [ns]");

    chamber_ADC_h1[i] = make_h1(Form("%s_ADC",chlist[i].hardname),
				Form("%s ADC",chlist[i].hardname),
				ADC_BINS, 0., 400., "ADC [ns]");

    for( int j=0; j<nml; j++ ) { 
      ml_time_h1[i][j] = make_h1(Form("%s_Time_ML%i",chlist[i].hardname,j+1),
			      Form("%s ML%i time",chlist[i].hardname,j+1),
			      TIME_BINS, -100., 900., "Time [ns]");
      ml_ADC_h1[i][j] = make_h1(Form("%s_ADC_ML%i",chlist[i].hardname,j+1),
			      Form("%s ML%i ADC",chlist[i].hardname,j+1),
			      ADC_BINS, 0., 400., "ADC [ns]");
    }

    for( int j=0; j<nmezz; j++ ) { 
      mezz_time_h1[i][j] = make_h1(Form("%s_Time_Mezz%i",chlist[i].hardname,j),
				Form("%s Mezz%i time",chlist[i].hardname,j),
				TIME_BINS, -100., 900., "Time [ns]");
      mezz_ADC_h1[i][j] = make_h1(Form("%s_ADC_Mezz%i",chlist[i].hardname,j),
			       Form("%s Mezz%i ADC",chlist[i].hardname,j),
			       ADC_BINS, 0., 400., "ADC [ns]");
    }
  }     //end loop over chambers

}        //end Begin()

/***************************************************************/
Bool_t selector::Process(Long64_t entry) {
  float ntracks=0.;

// Get event data - this gets the whole ntuple
  fChain->GetTree()->GetEntry(entry);
  nevents[0]++;

  //  printf("Process %i\n",nevents[0]);

// Proceed only if there is a track 
  if( trk_nTracks==0 ) return kTRUE;
  nevents[1]++;

// fill track histograms
  for( int it=0; it<trk_nTracks; it++ ) {

    if( trk_author[it] != TRACK_AUTHOR1 && trk_author[it] != TRACK_AUTHOR2 ) continue;
    ntracks++;
    trk_author_h1->Fill(trk_author[it]);
    
    Double_t eta = -TMath::Log(TMath::Tan(trk_theta[it]/2.));
    Double_t phi = trk_phi[it];
    trk_phi_h1->Fill(phi);
    trk_eta_h1->Fill(eta);
    trk_eta_phi_h2->Fill(eta,phi);

//Track momentum in GeV
    Double_t momentum = 0.,pt=0.;
    if( trk_qOverP[it] != 0. ) {
      momentum = TMath::Abs(1./trk_qOverP[it]/1000.);
      pt = momentum*TMath::Abs(TMath::Cos(trk_theta[it]));
    }
    trk_p_h1->Fill(momentum);
    if( momentum == 0. ) continue;
    trk_pt_h1->Fill(pt);
    trk_p_phi_h2->Fill(phi,momentum);
    trk_p_eta_h2->Fill(eta,momentum);
    trk_pt_phi_h2->Fill(phi,pt);
    trk_pt_eta_h2->Fill(eta,pt);

// Make track P cut 
    if( invertcut && momentum >= 1. && momentum <= 100. ) {
      continue;
    } else if( momentum < 1. || momentum > 100. ) {
      continue;
    }

//Loop over track hits
    float trackhits = 0.;
    int nchambers=0;
    int chamberlist[10] = {-1};
    int chamberhits[10] = {0};
    for( int ih=0; ih<trkHit_nHits; ih++ ) {
//  check if hit is on this track and
//  if it is a track hit & an MDT hit 
      if( trkHit_trackIndex[ih]!=it || trkHit_type[ih]!=1 || TechID(trkHit_id[ih])!=0 ) continue;
      int mfid = trkHit_id[ih];
      int idx = MDTindex(mfid);
      if( idx == -1 ) continue;  //skip bad chambers
//  Check if this hit is also on a segment
      for( int im=0; im<mdt_nMdt; im++ ) {
	if( mfid != mdt_id[im] ) continue;
	//	double segtime = mdt_t[im] + mdt_tube_t0[im];   
        trackhits++;
	int ic=0;
        for( ; ic<nchambers; ic++ ) {   //look for chamber in chamberlist
	  if( chamberlist[ic] == idx ) break;
	}
        if( ic == nchambers ) {  //if not found add chamber to list
	  chamberlist[ic] = idx;
	  nchambers++;
        }
        chamberhits[ic]++;

// USE TRACK time!!
	double time = trkHit_driftTime[ih] + mdt_tube_t0[im];   
	double dtime = mdt_t[im] - trkHit_driftTime[ih];   
	double adc  = mdt_adc[im];
	int ml    = MdtML(mfid);
	int layer = MdtLayer(mfid);
	int tube  = MdtTube(mfid);
        int mllt  = 1000*ml + 100*layer + tube;
        int mz    = MLLT2M(chlist[idx].hardname,mllt);
	int nmezz = MDTmaxmezz(chlist[idx].hardname);
	if( mz >= nmezz ) {
	  printf("   Chamber %s-%i-%i-%i mz%i %i\n",chlist[idx].hardname,ml,layer,tube,mz,nmezz);
	  break;
	}
	//	printf("    MDTnLml = %i\n",MDTnLml(chlist[idx].hardname,ml));
	//	int ntl_mez = 24/MDTnLml(chlist[idx].hardname,ml);
	//	printf("    MDTnML(ChName)*((tube-1)/ntl_mez) %i\n",MDTnML(chlist[idx].hardname)*((tube-1)/ntl_mez));
	//	printf("    MDTmlMZ0(ChName)=%i\n",MDTmlMZ0(chlist[idx].hardname));

	chamber_hits_h1->Fill(idx);
	mezz_hits_h1[idx]->Fill(mz);
	chamber_dtime_h1[idx]->Fill(dtime);   
	chamber_time_h1[idx]->Fill(time);   
	ml_time_h1[idx][ml-1]->Fill(time);     
	mezz_time_h1[idx][mz]->Fill(time);    
	chamber_ADC_h1[idx]->Fill(adc);
	ml_ADC_h1[idx][ml-1]->Fill(adc);     
	mezz_ADC_h1[idx][mz]->Fill(adc);    
	chamber_p_h1[idx]->Fill(momentum);
	chamber_pt_h1[idx]->Fill(pt);
	break;   //break so count segment hits only once.
      }  //end loop over segment hits

    }   //end loop over track hits
    track_hits_h1->Fill(trackhits);
    track_chambers_h1->Fill(nchambers);
    for( int ic=0; ic<nchambers; ic++ ) {
      segment_hits_h1->Fill( chamberhits[ic] );
    }
    // Combine hits in adjacent chambers
    for( int ic=0; ic<nchambers; ic++ ) {
      int idc = chamberlist[ic];
      for( int id=ic+1; id<nchambers; id++ ) {
	if( chamberhits[id] == 0 ) continue;
        int idd = chamberlist[id];
        if( chlist[idc].hardname[1] == chlist[idd].hardname[1] && 
	    TMath::Abs(chlist[idc].phi-chlist[idd].phi) < 2 &&
	    TMath::Abs(chlist[idc].eta-chlist[idd].eta) < 2 ) { 
	  //	  printf("%i %s (%i) %s (%i) \n",event_eventNumber,chlist[idc].hardname, chamberhits[ic],chlist[idd].hardname, chamberhits[id]);
	  chamberhits[ic] += chamberhits[id];
	  chamberhits[id] = 0;
	}
      } 
    }
    for( int ic=0; ic<nchambers; ic++ ) {
      if( chamberhits[ic] ) segment_hitsC_h1->Fill( chamberhits[ic] );
    }

  }     //end loop over tracks
  //  printf("Ntracks = %.0f\n",ntracks);
  trk_tracks_h1->Fill(ntracks);

  return kTRUE;
}

/**************************************************************/
// The Terminate() function is the last function to be called during
// a query. It always runs on the client, it can be used to present
// the results graphically or save the results to file.
/**************************************************************/
void selector::Terminate() { 

  printf("Processed %i/%i events\n",nevents[1],nevents[0]);
// Save histograms to a file
//  Open ROOT file
  TFile *f = new TFile("time.root","recreate");
  if (f->IsZombie()) {   
    printf("selector_time ERROR opening ROOT output file\n");
    return;
  }

  f->cd();
  chamber_hits_h1->Write();
  trk_author_h1->Write();
  trk_tracks_h1->Write();
  track_hits_h1->Write();
  track_chambers_h1->Write();
  segment_hits_h1->Write();      
  segment_hitsC_h1->Write();      
  trk_p_h1->Write();
  trk_pt_h1->Write();
  trk_phi_h1->Write();
  trk_eta_h1->Write();
  trk_eta_phi_h2->Write();
  trk_p_phi_h2->Write();
  trk_p_eta_h2->Write();
  trk_pt_phi_h2->Write();
  trk_pt_eta_h2->Write();

// loop over chambers
  for( int i=0; i<MAXCHAMBERS; i++ ) { 

//if there are no hits for this chamber skip it.
    if( chamber_hits_h1->GetBinContent(i+1) == 0. ) continue;

    f->cd();
    f->mkdir(chlist[i].hardname);
    f->cd(chlist[i].hardname);
    chamber_p_h1[i]->Write();   
    chamber_pt_h1[i]->Write();   
    mezz_hits_h1[i]->Write();
    chamber_dtime_h1[i]->Write();
    chamber_time_h1[i]->Write();
    chamber_ADC_h1[i]->Write();
// Only save ML and Mezz histograms which have entries
    int nml   = MDTnML(chlist[i].hardname);
    int nmezz = MDTmaxmezz(chlist[i].hardname);
    for( int j=0; j<nml; j++ ) { 
      if( ml_time_h1[i][j]->GetEntries() ) ml_time_h1[i][j]->Write();
      if( ml_ADC_h1[i][j]->GetEntries() ) ml_ADC_h1[i][j]->Write();
    }
    for( int j=0; j<nmezz; j++ ) { 
      if( mezz_time_h1[i][j]->GetEntries() ) mezz_time_h1[i][j]->Write();
      if( mezz_ADC_h1[i][j]->GetEntries() ) mezz_ADC_h1[i][j]->Write();
    }
  }

  f->Close();
}  //end selector::Terminate

void selector::SlaveBegin(TTree * ) {}
void selector::SlaveTerminate() {}
