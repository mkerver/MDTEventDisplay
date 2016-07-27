/**********************************************************
 selector_resi.C

   Make plots of stuff by ML 
    output: ML.root containing histograms


   Edward Diehl 20-Oct-2015 

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
#include "BestSegment.h"

const int   SEGMENT_HIT_CUT=6;    //min segment hits for best segment
const int   ADC_CUT=80;           // ADC cut for raw hits

// Histograms
TList *histlist;

// Chamber rawhit histos
TH1F *raw_tdc_h1[MAXCHAMBERS][2];     //TDC for hits in rawhits
TH1F *raw_adc_h1[MAXCHAMBERS][2];     //ADC for hits in rawhits
TH2F *raw_adctdc_h2[MAXCHAMBERS][2];  //ADC vs TDC for hits in rawhits
// Chamber rawhit histos after ADC cut
TH1F *raw_tdc_cut_h1[MAXCHAMBERS][2]; //TDC for hits in rawhits
TH1F *raw_adc_cut_h1[MAXCHAMBERS][2]; //ADC for hits in rawhits

// Chamber segment histos
TH1F *seg_tdc_h1[MAXCHAMBERS][2];     //TDC for hits in segments
TH1F *seg_adc_h1[MAXCHAMBERS][2];     //ADC for hits in segments
TH2F *seg_adctdc_h2[MAXCHAMBERS][2];  //ADC vs TDC for hits in segments
TH1F *seg_resi_h1[MAXCHAMBERS][2];     
TH1F *seg_driftradius_h1[MAXCHAMBERS][2];     //Drift radius

/**************************************************************/
// Book histos, read event list
/**************************************************************/
void selector::Begin(TTree *) {

  printf("Running selector_ML.C\n");

//  Book histos
  histlist = new TList();

// loop over chambers
  for( int j=0; j<MAXCHAMBERS; j++ ) {   //Chambers
    for( int i=0; i<2; i++ ) {           //ML
      int ml = i + 1;
      raw_tdc_h1[j][i] = make_h1(histlist,Form("%s_ML%i_rawTDC",chlist[j].hardname,ml),
					 Form("%s ML%i raw TDC",chlist[j].hardname,ml),
					 200, 0., 1400., "TDC [ticks]");
      raw_adc_h1[j][i] = make_h1(histlist,Form("%s_ML%i_rawADC",chlist[j].hardname,ml),
					 Form("%s ML%i raw ADC",chlist[j].hardname,ml),
					 100, 0., 400., "ADC [ticks]");
      raw_adctdc_h2[j][i] = make_h2(histlist,Form("%s_ML%i_rawADCTDC",chlist[j].hardname,ml),
					    Form("%s ML%i raw ADC vs TDC",chlist[j].hardname,ml),
					    100,0.,1400.,100,0.,400.,"TDC [ticks]","ADC [ticks]");
      raw_tdc_cut_h1[j][i] = make_h1(histlist,Form("%s_ML%i_rawTDCcut%d",chlist[j].hardname,ml,ADC_CUT),
					     Form("%s ML%i raw TDC (ADC>%d)",chlist[j].hardname,ml,ADC_CUT),
					     200, 0., 1400., "TDC [ticks]");
      raw_adc_cut_h1[j][i] = make_h1(histlist,Form("%s_ML%i_rawADCcut%d",chlist[j].hardname,ml,ADC_CUT),
					   Form("%s ML%i raw ADC (ADC>%d)",chlist[j].hardname,ml,ADC_CUT),
				       100, 0., 400., "ADC [ticks]");
      seg_tdc_h1[j][i] = make_h1(histlist,Form("%s_ML%i_segTDC",chlist[j].hardname,ml),
					 Form("%s ML%i Segment TDC",chlist[j].hardname,ml),
					 200, 0., 1400., "TDC [ticks]");
      seg_adc_h1[j][i] = make_h1(histlist,Form("%s_ML%i_segADC",chlist[j].hardname,ml),
					 Form("%s ML%i segment ADC",chlist[j].hardname,ml),
					 100, 0., 400., "ADC [ticks]");
      seg_adctdc_h2[j][i] = make_h2(histlist,Form("%s_ML%i_segADCTDC",chlist[j].hardname,ml),
					    Form("%s ML%i segment ADC vs TDC",chlist[j].hardname,ml),
					    100,0.,1400.,100,0.,400.,"TDC [ticks]","ADC [ticks]");
      seg_resi_h1[j][i] = make_h1(histlist,Form("%s_ML%i_SegRes",chlist[j].hardname,ml),
				  Form("%s ML%i Segment Residuals",chlist[j].hardname,ml),
				       100, -1000., 1000., "Residuals #mum");
      seg_driftradius_h1[j][i] = make_h1(histlist,Form("%s_ML%i_DriftRadius",chlist[j].hardname,ml),
				  Form("%s ML%i Drift Radius",chlist[j].hardname,ml),
				  200, -17., 17., "mm");
    }     //end loop over ML
  }     //end loop over chambers

}        //end Begin()

/***************************************************************/
Bool_t selector::Process(Long64_t entry) {

// Get event data - this gets the whole ntuple
  fChain->GetTree()->GetEntry(entry);

/**************************************************/
//  fill histograms
/**************************************************/

//  Fill TDCs in raw hits
  for( int ir=0; ir<rawMdt_nRMdt; ir++ ) {
    int idx = MDTindex(rawMdt_id[ir]);
    int ML  = MdtML(rawMdt_id[ir]) - 1;   //subtract 1 because use as index
    float tdc = rawMdt_tdc[ir];
    float adc = rawMdt_adc[ir];
    raw_tdc_h1[idx][ML]->Fill(tdc);
    raw_adc_h1[idx][ML]->Fill(adc);
    raw_adctdc_h2[idx][ML]->Fill(tdc,adc);
    if( adc > ADC_CUT ){
      raw_tdc_cut_h1[idx][ML]->Fill(tdc);
      raw_adc_cut_h1[idx][ML]->Fill(adc);
    }
  }  //end loop over raw hits
 
/**************************************************/
//  find Segments with MDT hits
/**************************************************/
  std::map<Int_t,std::vector<Int_t> > map_seghit;
  std::vector<Int_t> vi_hit;
  std::map<Int_t,std::vector<Int_t> >::iterator it_map;
  HitSegment( this, entry, map_seghit );
  BestSegment( this, entry, map_seghit );

// Loop over map of segments
  for( it_map=map_seghit.begin(); it_map!=map_seghit.end(); it_map++ ) {
    int is = (*it_map).first;
    vi_hit = (*it_map).second;
    if( seg_author[is] != 4 ) continue;    //unknown segments

//  Fill histos using hits on this segment
    for( unsigned int ihit=0; ihit<vi_hit.size(); ihit++ ) {
      int im = vi_hit[ihit];
      int idx = MDTindex(mdt_id[im]);
      int ML  = MdtML(mdt_id[im]) - 1;   //subtract 1 because use as index
      seg_tdc_h1[idx][ML]->Fill(mdt_tdc[im]);
      seg_adc_h1[idx][ML]->Fill(mdt_adc[im]);
      seg_adctdc_h2[idx][ML]->Fill(mdt_tdc[im],mdt_adc[im]);
      seg_resi_h1[idx][ML]->Fill(1000.*mdt_resi[im]);
      seg_driftradius_h1[idx][ML]->Fill(mdt_r[im]);
    }      //end loop over MDTs in segments
  }        //end loop over segments

  return kTRUE;
}

/**************************************************************/
// The Terminate() function is the last function to be called during
// a query. It always runs on the client, it can be used to present
// the results graphically or save the results to file.
/**************************************************************/
void selector::Terminate() { 
// Save histograms to a file
//  Open ROOT file
  TFile *f = new TFile("ML.root","recreate");
  if (f->IsZombie()) {   
    printf("selector_ML ERROR opening ROOT output file\n");
    return;
  }

  for( int j=0; j<MAXCHAMBERS; j++ ) {
    // Skip chambers with no entries
    if( raw_tdc_h1[j][0]->GetEntries() + raw_tdc_h1[j][1]->GetEntries() == 0.) continue;
    f->cd();
    f->mkdir(chlist[j].hardname);
    f->cd(chlist[j].hardname);
    for( int i=0; i<2; i++ ) {           //ML
      raw_tdc_h1[j][i]->Write();
      raw_adc_h1[j][i]->Write();
      raw_adctdc_h2[j][i]->Write();
      raw_tdc_cut_h1[j][i]->Write();
      raw_adc_cut_h1[j][i]->Write();
      seg_tdc_h1[j][i]->Write();   
      seg_adc_h1[j][i]->Write();   
      seg_adctdc_h2[j][i]->Write();
      seg_resi_h1[j][i]->Write();
      seg_driftradius_h1[j][i]->Write();
    }
  }

  f->Close();
}

void selector::SlaveBegin(TTree * ) {}
void selector::SlaveTerminate() {}
