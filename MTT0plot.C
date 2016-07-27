/***********************************************************
* Make plots of T0 and ADC fits in ROOT files from MTT0 calib 
* Note: to refit MTT0 root files use MTT0fit.C (ADC+Time) or MTT0fitADC.C (ADC only)
*
* OPTIONS
*  filelist = list of root files to process (one file name per line)
*             OR name of a root file to process
*  flag     = flag to control various options, character string
*             E or e => Make EPS plots, not PNG
*             W or w => make composite plots for webpage (default)
*             l      => List option - list all plots found
*             S or s => Make single plots of everything
*                Use ATLAS publication style (run in no compile mode)
*             These only apply to single plot option
*             a => Make ADC plots
*             M => Make plots for chamber & multilayers
*             m => Make plots for mezzanines
*             t => Make plots for tubes
*             A => Make plots for all histograms 
*             g => Make double fit of Tmax for step of wire sag chambers
*             C => Make close-up plot of T0 fit
*             B => Special plot for Bing
*  chrun    = Name or run number to put on plots
*  mllt     = mllt of tube to plot
*
*  110106 version to work with Athena 16 ROOT files (plot names have changed)
*         Mezzcards are now listed by mezz channel not geometric step.
*  140523 Discover that mezzcard numbering is screwed up because it does not take
*         into account that mezzcards numbers are swapped between ML1/2 for various
*         A/C L/S chambers.  So fix numbering by hand here.
************************************************************/
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TPaveText.h>
#include <TLatex.h>
#include <TH1.h>
#include <TF1.h>
#include <TFile.h>
#include <TKey.h>
#include <TDirectory.h>
#include "/afs/atlas.umich.edu/home/muoncal/public/root/chamberlist.h"
#include "/afs/atlas.umich.edu/home/muoncal/public/root/muonfixedid.h"

const int NDATA=21; //number of data items written to stats file

void MTT0plot( const char *filelist, const char *flag="A", const char *chrun="", int imllt=0 );
void Init( void );
void MakePlot( const char *fname );
void DrawTimeHist(TH1F *h, float data[NDATA] );
void DrawAdcHist(TH1F *h, float data[NDATA] );
void DrawBing(TH1F *h);
int fix_mezzcard( char *chambername, int mz );

char chplot[34] = ".png",crun[30]="";
int  singleplot=0,mlplot=0,mzplot=0, tubeplot=0, adcplot=0, wiresag=0, list=0, listsum=0, mllt=0, pubplot=0, closeup=0, bing=0; 
//  Canvases for plots
TCanvas *c1,*c2,*c3,*c4;           
TPad    *p1,*p2,*p3,*p4;

std::ofstream fout;

/***********************************************************
* Make plots of MTT0 calibs 
************************************************************/
void MTT0plot( const char *filelist, const char *flag, const char *chrun, int imllt ) {
  char fname[255];

// Parse options
  for( int i=0; flag[i]!='\0'; i++ ) {
    if( flag[i] == 'B' ) {
      bing = 1;
      singleplot = 1;
    }
    if( flag[i] == 'E' || flag[i] == 'e' ) sprintf(chplot,"eps"); 
    if( flag[i] == 'W' || flag[i] == 'w' ) singleplot = 0; 
    if( flag[i] == 'S' || flag[i] == 's' ) singleplot = 1; 
    if( flag[i] == 'M' ) mlplot = 1;
    if( flag[i] == 'm' ) mzplot = 1;
    if( flag[i] == 't' ) tubeplot = 1;
    if( flag[i] == 'g' ) wiresag = 1;
    if( flag[i] == 'A' ) {
      mlplot = 1; 
      mzplot = 1; 
      tubeplot = 1; 
    }
    if( flag[i] == 'a' ) adcplot = 1; 
    if( flag[i] == 'l' ) list = 1; 
    if( flag[i] == 'L' ) listsum = 1; 
    if( flag[i] == 'C' ) closeup = 1; 
  }
  sprintf(crun,"%s",chrun);
  if( crun[0] != '\0' ) sprintf(chplot,"_%s.png",chrun);
  mllt = imllt;

  Init();

//  If file name has "root" in it, assume it is a root file.
//  If not, assume it is a list of root files
  if( strstr(filelist,"root") ) {
    MakePlot( filelist );
  } else {
// Open list of T0 root files
    std::ifstream fin(filelist );
    if( fin.is_open() == 0 )  {
      printf("ERROR: cannot open %s\n",filelist);
      return;
    }

//  Open stats file
    sprintf(fname ,"MTT0stats_%s",filelist);
    fout.open(fname,std::fstream::app );
    if( fout.is_open() == 0 ) {
      printf("ERROR: cannot open stats file %s\n",fname);
      return; 
    } 
    printf("INFO: opened stats file %s\n",fname);
// Print header of stats file
    fout << "chamber type Events T0 T0_Err T0slope T0slope_Err Tmax Tmax_Err TmaxSlope TmaxSlope_Err ADCpeak ADCpeak_Err ADCwidth ADCwidth_Err ADCskew ADCskew_Err\n";
    fout << "units     x     x  [ns]  [ns]    [ns]    [ns]     [ns]    [ns]     [ns]      [ns]         [ns]    [ns]        [ns]       [ns]        x        x\n"; 
    fout << "min       x     0   -10    0      1        0      720       0        1         0          110       0          25          0         1        0\n"; 
    fout << "max       x     0    80    0      7        0      800       0       18         0          180       0          60          0         6        0\n"; 
    
//  loop over files in list & Make T0 plots
    while( fin >> fname && !fin.eof() ) { MakePlot( fname ); }
    fin.close();
    fout.close();
  }     //end if list of files
}     //end of MTT0Plot

/***********************************************************
* Initialize program
************************************************************/
void Init( void ) {
  //  gStyle->SetOptStat(111; 
  //  if( wiresag ) gStyle->SetOptStat(11);  
  if( closeup ) {
    gStyle->SetStatX(0.9);
    gStyle->SetStatW(0.45);
    gStyle->SetStatY(0.40);
  } else {
    //    gStyle->SetStatW(0.48);
    //    gStyle->SetStatX(0.98);
    //    gStyle->SetStatY(1.0);
    gStyle->SetStatW(0.4);
    gStyle->SetStatX(0.98);
    gStyle->SetStatY(0.95);
  }
  gStyle->SetStatH(0.28);
  gStyle->SetTitleH(0.09);
  gStyle->SetTitleX(0.2);
    
  gStyle->SetStatH(0.3);
  gStyle->SetOptStat(1111); 

//  Book the canvas
  if( singleplot ) {
    c1 = new TCanvas("c1","chamberplot",800,600);
    return;
  }

//  Book the canvases for doing multiplots - chamber plot
//  Make subpad for plots (and to leave space for page title)
  c1 = new TCanvas("c1","chamberplot",700,400);
  p1 = new TPad("p1","p1",0.0,0.0,1.0,0.9,-1,-1,0);
  p1->Draw();
  p1->Divide(2,1); 
//  Multilayer plot
  c2 = new TCanvas("c2","mlplot",700,600);
  p2 = new TPad("p2","p2",0.0,0.0,1.0,0.9,-1,-1,0);
  p2->Draw();
  p2->Divide(2,2); 
// Mezzanine plots - get created in MakePlot

}    //end Init()

/***********************************************************
* Make plot of MTT0 calibs
*  fname = name of root file
************************************************************/
void MakePlot( const char *fname ) {
  TH1F *histlist[1000];     //histograms read from ROOT file
  TKey *key;
  float chamdata[NDATA];
  float multdata[2][NDATA];
  float mezzdata[20][NDATA];
  float tubedata[NDATA];     //not really used, but needed to call DrawTimeHist
  int  nhist=0;    //total hists
  int  nmezz=0,nrows=0,ml=0,mz,mldata=0,mzdata=0,ntubes=0;
  char title[80],chambername[20]="",chh[3]="";

//init arrays
  for( int i=0; i<NDATA; i++ ) {
    chamdata[i] = -1;
    multdata[0][i] = -1;
    multdata[1][i] = -1;
    tubedata[i] = -1;
    for( int j=0; j<20; j++ ) mezzdata[j][i] = -1;
  }

// Open ROOT file
  TFile *f = new TFile(fname);
  if( f && f->IsZombie() ) {   
    printf("Error with ROOT file %s\n",fname);
    f->Close();
    return;
  } 
//  printf("Opened ROOT file %s\n",fname);
//  Look for histograms, and if found grab them.
//  Skip the blasted "scrambled" histograms 
//  Skip tube (tb) histograms - not any more
  gDirectory->cd("MT_t0_fitter");
  TIter next(gDirectory->GetListOfKeys());
  while ((key = (TKey*)next())) {
    if( strcmp(key->GetClassName(),"TH1F")==0 &&         
	strstr(key->GetName(),"scram") == NULL ) {
      //        && strstr(key->GetName(),"tb") == NULL ) {
      histlist[nhist] = (TH1F *) gDirectory->Get(key->GetName());
      if( histlist[nhist] ) {
      	if(list) printf("FOUND %i %s\n",nhist+1,histlist[nhist]->GetName()); 
	nhist++;
      }
    }
  }

// If no histograms then move on
  if( nhist == 0 ) {
    printf("%s NO HISTOGRAMS\n",fname);
    f->Close();
    return;
  }

//  Naming scheme of histograms
//  Chamber names
// 01234567890123456789012345678901234567
// t_spec_BIS_etaC8_phi1_num16777216
// adc_spec_BIS_etaC8_phi1_num16777216
//  Mult names
// 01234567890123456789012345678901234567
// t_spec_EML_etaC2_phi5_ml1_num305168384
// adc_spec_EML_etaC2_phi5_ml2_num305168896
//  Mezz names
// 01234567890123456789012345678901234567
// t_spec_EMS_etaC5_phi3_ml2_mez6_num320356
// adc_spec_EMS_etaC5_phi3_ml1_mez5_num320356357
//  Tube names
// 01234567890123456789012345678901234567
// t_spec_EMS_etaA1_phi3_ml1_ly1_tb37_num323509284
// adc_spec_BIL_etaA1_phi1_ml1_ly1_tb2_num4725761_4
  int tdchistos[4] = {0,0,0,0};
  int adchistos[4] = {0,0,0,0};
// Count types of histograms found
  for( int i=0; i<nhist; i++ ) {
    sprintf(title,"%s",histlist[i]->GetName());
//  First determine the chambername
    if( chambername[0]=='\0' ) {
      int mfid=0;
      char *ptr=strtok(title,"_");
//  Split name into strings separated by '_'
      for( int j=0; ptr!=NULL; j++, ptr=strtok(NULL, "_") ) {
	if( j==2 ) {
	  mfid = SetStationID( IChamberType(ptr), mfid);
	} else if( j==3 ) {
	  sprintf(chh,"%c",ptr[4]);
	  if( ptr[3] == 'A' ) mfid = SetStationEta( atoi(chh), mfid); 
	  else                mfid = SetStationEta( -atoi(chh), mfid);	  	  
	} else if( j==4 ) {
	  sprintf(chh,"%c",ptr[3]);
          mfid = SetStationPhi( atoi(chh), mfid);
	  ChamberName(mfid,chambername);
	  nmezz = MDTnmezz(chambername);   //number of mezz cards
	  ntubes = MDTtotalTubes(chambername);
	}
      }
    }
    if( title[0] == 't' ) {    //time histogram
      if( title[7] == 'S' || title[22] == 'n' ) {        //summary hist
	tdchistos[0]++;
      } else if( title[26] == 'n' ) {  //ML
	tdchistos[1]++;
      } else if( title[28] == 'z' ) {  //mezz card
	tdchistos[2]++;
      } else if( title[30] == 't' ) {  //tube
	tdchistos[3]++;
      }
    } else if( title[0] == 'a') {   //adc histogram
      if( title[9] == 'S' || title[24] == 'n' ) {  //Summary histogram
	adchistos[0]++;
      } else if( title[28] == 'n' ) {  //ML
	adchistos[1]++;
      } else if( title[30] == 'z' ) {  //mezz card
	adchistos[2]++;
      } else if( title[32] == 't' ) {  //tube
	adchistos[3]++;
      }
    }  //end if adc hist      
  }    //end loop over histograms
  //  printf("%s MZ=%i Tube=%i Histos TDC Cham=%i ML=%i MZ=%i Tube=%i",chambername,nmezz,ntubes,tdchistos[0],tdchistos[1],tdchistos[2],tdchistos[3]);
  //  printf("  ADC Cham=%i ML=%i MZ=%i Tube=%i\n",adchistos[0],adchistos[1],adchistos[2],adchistos[3]);
  printf("%s MZ=%i Tube=%i TDC=%i/%i/%i/%i",chambername,nmezz,ntubes,tdchistos[0],tdchistos[1],tdchistos[2],tdchistos[3]);
  printf(" ADC=%i/%i/%i/%i",adchistos[0],adchistos[1],adchistos[2],adchistos[3]);
  if( tdchistos[3]>0. && tdchistos[3]!=ntubes ) printf(" TUBE WARNING");
  printf("\n");

// Quit if doing list
  if(list || listsum) {
    f->Close();
    return;
  }
  printf("%s %s %i histograms nmezz=%i tubes=%i\n",fname,chambername,nhist,nmezz,ntubes);

//If doing multiplots put titles on plots
  if( !singleplot ) {
    c1->cd();

    TPaveText *headerPT1 = new TPaveText(0.02,0.92,0.98,0.99);
    headerPT1->AddText(Form("%s %s MTT0 Chamber plots",crun,chambername));
    headerPT1->Draw();

    c2->cd();
    TPaveText *headerPT2 = new TPaveText(0.02,0.92,0.98,0.99);
    headerPT2->AddText(Form("%s %s MTT0 Multilayer",crun,chambername));
    headerPT2->Draw();

    nrows = nmezz/4;
    if( nmezz%4 ) nrows++;

//  Make subpad for plots (and to leave space for page title)
    c3 = new TCanvas("c3","MezzCardTime",1000,220*nrows);
    p3 = new TPad("p3","p3",0.0,0.0,1.0,0.95,-1,-1,0);
    p3->Draw();
    p3->Divide(4,nrows);
    TPaveText *headerPT3 = new TPaveText(0.02,0.955,0.98,0.99);
    headerPT3->AddText(Form("%s %s MTT0 Mezzcard Time",crun,chambername));
    headerPT3->Draw();     

    c4 = new TCanvas("c4","MezzCardADC",1000,220*nrows);
//  Make subpad for plots (and to leave space for page title)
    p4 = new TPad("p4","p4",0.0,0.0,1.0,0.95,-1,-1,0);
    p4->Draw();
    p4->Divide(4,nrows);
    TPaveText *headerPT4 = new TPaveText(0.02,0.955,0.98,0.99);
    headerPT4->AddText(Form("%s %s MTT0 Mezzcard ADC",crun,chambername));
    headerPT4->Draw();     

  }

//  Now go through the histogram list and make plots
  for( int i=0; i<nhist; i++ ) {
    sprintf(title,"%s",histlist[i]->GetName());
    if( title[0]=='t' && adcplot==0) {    //time histogram
      histlist[i]->SetAxisRange(-100.,1000.,"X");
      histlist[i]->GetXaxis()->SetTitle("time [ns]");
      if( title[7] == 'S' || title[22] == 'n' ) {        //summary hist
        histlist[i]->SetName(Form("%s Time",chambername));
	if( singleplot ) {
	  histlist[i]->SetTitle(crun);
          if( bing ) {
  	    DrawBing(histlist[i]);
	    c1->Print(Form("%s_MTT0_CH_Time_Bing%s",chambername,chplot));
	    return;
	  }
	  DrawTimeHist(histlist[i],chamdata);
          if(      wiresag ) c1->Print(Form("%s_MTT0_CH_Time_sag%s",chambername,chplot));
          else if( closeup ) c1->Print(Form("%s_MTT0_CH_Time_closeup%s",chambername,chplot));
	  else c1->Print(Form("%s_MTT0_CH_Time%s",chambername,chplot));
	} else {
	  p1->cd(1);
	  DrawTimeHist(histlist[i],chamdata);
	}
      } else if( title[28] == 'z' ) { //mezz card
	mzdata |= 1;                  //time mezz plots
        sprintf(chh,"%c",title[24]);
        ml = atoi(chh);
//        int ilen = strlen(title);
// if( title[ilen-2] == '0' ) sprintf(chh,"%c",title[ilen-1]);
// else                       sprintf(chh,"%c%c",title[ilen-2],title[ilen-1]);
        if( title[30] == '_' ) sprintf(chh,"%c",title[29]);
        else                   sprintf(chh,"%c%c",title[29],title[30]);
        mz = fix_mezzcard( chambername, atoi(chh));
	//	printf("Checking histogram %s %s %i\n",chambername,title,mz);
//  BIS8 and BEE are listed as having only even numbered mezzcards.  Change to fit on plots and array
//	if( chambername[0] == 'B' && (chambername[1]=='E' || chambername[3]=='8') ) imz = mz/2;
//	else imz = mz;
//        imz = mz + nmezz*(ml-1)/2;
//  Check for bad mezz card numbers
// TEST
// 	if( mz >= nmezz ) {
//	  printf("BAD MEZZCARD NUMBER %s %s %i\n",chambername,title,mz);
//	  continue;
//	}
	histlist[i]->SetName(Form("%s Time ML%i MZ%i",chambername,ml,mz));
	if( singleplot ) {
	  if( mzplot ) {
	    histlist[i]->SetTitle(crun);
	    DrawTimeHist(histlist[i],mezzdata[mz]);
	    printf("%s  ml=%i mz=%i\n",title,ml,mz);
	    printf("Plot hist %s\n",histlist[i]->GetName());

	    if(      wiresag ) c1->Print(Form("%s_MTT0_MZ%i_Time_sag%s",chambername,mz,chplot));
	    else if( closeup ) c1->Print(Form("%s_MTT0_MZ%i_Time_closeup%s",chambername,mz,chplot));
	    else               c1->Print(Form("%s_MTT0_MZ%i_Time%s",chambername,mz,chplot));
	  }
	} else {
	  p3->cd(mz+1);
	  DrawTimeHist(histlist[i],mezzdata[mz]);
	}
      } else if( title[26] == 'n' ) {  //ML
// Do not plot ML plots for BEE,BIS8 only one ML, so same as chamber plot
	if( chambername[0] == 'B' && (chambername[1]=='E' || chambername[3]=='8') ) continue;
	mldata = 1;
        sprintf(chh,"%c",title[24]);
        ml = atoi(chh);
	histlist[i]->SetName(Form("%s Time ML%i",chambername,ml));
	if( singleplot ) {
	  if( mlplot ) {
	    histlist[i]->SetTitle(crun);
	    DrawTimeHist(histlist[i],multdata[ml-1]);
	    if(      wiresag ) c1->Print(Form("%s_MTT0_ML%i_Time_sag%s",chambername,ml,chplot));
	    else if( closeup ) c1->Print(Form("%s_MTT0_ML%i_Time_closeup%s",chambername,ml,chplot));
	    else          c1->Print(Form("%s_MTT0_ML%i_Time%s",chambername,ml,chplot));
	  }
	} else {
	  p2->cd(2*ml-1);
	  DrawTimeHist(histlist[i],multdata[ml-1]);
	}
      } else if( title[30] == 't' && tubeplot == 1 ) {  //tube
        sprintf(chh,"%c",title[24]);
        ml = atoi(chh);
        sprintf(chh,"%c",title[28]);
        int ly = atoi(chh);
        if( title[33] == '_' ) sprintf(chh,"%c",title[32]);
        else sprintf(chh,"%c%c",title[32],title[33]);
        int tb = atoi(chh);
        int imllt = 1000*ml + 100*ly + tb;
        if( mllt > 0 && mllt != imllt ) continue;
	histlist[i]->SetName(Form("%s Time %i-%i-%i",chambername,ml,ly,tb));
        histlist[i]->SetTitle(crun);
	DrawTimeHist(histlist[i],tubedata);
        c1->Print(Form("%s_MTT0_Tube_%i_Time%s",chambername,imllt,chplot));
      }
    } else if( title[0] == 'a') {   //adc histogram
      histlist[i]->SetAxisRange(0.,400.,"X");
      histlist[i]->GetXaxis()->SetTitle("ADC counts");
      if( title[9] == 'S' || title[24] == 'n' ) {  //Summary histogram
        histlist[i]->SetName(Form("%s ADC",chambername));
	if( singleplot ) {
	  histlist[i]->SetTitle(crun);
	  DrawAdcHist(histlist[i],chamdata);
	  if( adcplot ) 
	    c1->Print(Form("%s_MTT0_CH_ADC%s",chambername,chplot));
	} else {
	  p1->cd(2);
	  DrawAdcHist(histlist[i],chamdata);
	}
      } else if( title[30] == 'z' ) { //mezz card
	mzdata |= 2;                  //ADC mezz plots
// int ilen = strlen(title);
// if( title[ilen-2] == '0' ) sprintf(chh,"%c",title[ilen-1]);
// else       sprintf(chh,"%c%c",title[ilen-2],title[ilen-1]);
        if( title[32] == '_' ) sprintf(chh,"%c",title[31]);
        else                   sprintf(chh,"%c%c",title[31],title[32]);
        mz = fix_mezzcard( chambername, atoi(chh));
	//	printf("Checking histogram %s %s %i\n",chambername,title,mz);
//  BIS8 and BEE are listed as having only even numbered mezzcards.  Change to fit on plots and array
//	if( chambername[0] == 'B' && (chambername[1]=='E' || chambername[3]=='8') ) mz = mz/2;
//	else mz = mz;
//        mz = mz + nmezz*(ml-1)/2;
//  Check for bad mezz card numbers
// 	if( mz >= nmezz ) {
//	  printf("BAD MEZZCARD NUMBER %s %s %i\n",chambername,title,mz);
//	  continue;
//	}
        histlist[i]->SetName(Form("%s ADC ML%i MZ%i",chambername,ml,mz));
	if( singleplot ) {
	  if( adcplot && mzplot ) {
	    histlist[i]->SetTitle(crun);
	    DrawAdcHist(histlist[i],mezzdata[mz]);
	    c1->Print(Form("%s_MTT0_MZ%i_ADC%s",chambername,mz,chplot));
	  }
	} else {
	  p4->cd(mz+1);
	  DrawAdcHist(histlist[i],mezzdata[mz]);
	}
      } else if( title[28] == 'n' ) {  //ML
// Do not plot ML plots for BEE,BIS8 only one ML, so same as chamber plot
	if( chambername[0] == 'B' && (chambername[1]=='E' || chambername[3]=='8') ) continue;
	mldata = 1;
        sprintf(chh,"%c",title[26]);
        ml = atoi(chh);
        histlist[i]->SetName(Form("%s ADC ML%i",chambername,ml));
	if( singleplot ) {
	  if( adcplot && mlplot ) {
	    histlist[i]->SetTitle(crun);
	    DrawAdcHist(histlist[i],multdata[ml-1]);
	    c1->Print(Form("%s_MTT0_ML%i_ADC%s",chambername,ml,chplot));
	  }
	} else {
	  p2->cd(2*ml);
	  DrawAdcHist(histlist[i],multdata[ml-1]);
	}
      } else if( title[32] == 't' && tubeplot == 1 ) {  //tube
        sprintf(chh,"%c",title[26]);
        ml = atoi(chh);
        sprintf(chh,"%c",title[30]);
        int ly = atoi(chh);
        if( title[35] == '_' ) sprintf(chh,"%c",title[34]);
        else sprintf(chh,"%c%c",title[34],title[35]);
        int tb = atoi(chh);
        int imllt = 1000*ml + 100*ly + tb;
        if( mllt > 0 && mllt != imllt ) continue;
	histlist[i]->SetName(Form("%s ADC %i-%i-%i",chambername,ml,ly,tb));
        histlist[i]->SetTitle(crun);
	DrawAdcHist(histlist[i],tubedata);
        c1->Print(Form("%s_MTT0_Tube_%i_ADC%s",chambername,imllt,chplot));
      }
    }  //end if adc hist      

  }    //end loop over histograms

//  Quit if no chamber data found
  if( chamdata[0] == 0. ) {
    f->Close();
    return;
  }

//  Save canvas to a file if TDC plot has non-zero hits
  if( !singleplot ) {
    c1->Print(Form("%s_MTT0_CHPlot%s",chambername,chplot));
    if( mldata ) { 
      c2->Print(Form("%s_MTT0_MLPlot%s",chambername,chplot));
    }
    if( mzdata&1 ) { 
      c3->Print(Form("%s_MTT0_TimeMezzPlot%s",chambername,chplot));
      if( mzdata&2 ) c4->Print(Form("%s_MTT0_ADCMezzPlots%s",chambername,chplot));
      else           printf("WARNING: %s NO MEZZCARD ADC PLOTS\n",chambername);
    }
  }
  f->Close();

// Save data to file
  if( fout.is_open() ) {
    int imax=15;
    if( wiresag ) imax=21;
    fout << chambername << " CHM";
    for( int i=0; i<imax; i++ ) fout << " " << chamdata[i]; 
    fout << "\n";
      
    if( mldata && (singleplot==0 || mlplot==1)) {
      for( int i=0; i<2; i++ ) {
	fout << chambername << " ML" << i+1;
	for( int j=0; j<NDATA; j++ ) fout << " " << multdata[i][j]; 
	fout << "\n";
      }
    }
    
    if( mzdata && (singleplot==0 || mzplot==1)) {
      for( int i=0; i<nmezz; i++ ) {
	if( mezzdata[i][0] == -1 ) continue;
	fout << chambername << " MZ" << i;
	for( int j=0; j<NDATA; j++ ) fout << " " << mezzdata[i][j]; 
	fout << "\n";
      }
    }
  }       //end if chamdata[0]>0 

}      //end MakePlot()

/************************************************************
*  Draw fit info on plots
************************************************************/
void DrawTimeHist(TH1F *h, float data[NDATA] ) {
  Double_t dtmax,dtmaxe;
  TF1 *ftm, *ft0;
  TPaveText *t0PT, *tmPT, *tdPT, *tmPT1=0, *tdPT1=0, *tmPT2=0, *tdPT2=0;

  gStyle->SetOptStat(11); 
  h->Draw();

// Change location and number of text boxes for wiresag option.
  if( wiresag ) {
    t0PT = new TPaveText(0.50,0.76,0.98,0.86,"NDC");
    tmPT = new TPaveText(0.50,0.76,0.98,0.86,"NDC");
    tdPT = new TPaveText(0.50,0.71,0.98,0.76,"NDC");
    tmPT1 = new TPaveText(0.50,0.61,0.98,0.71,"NDC");
    tdPT1 = new TPaveText(0.50,0.56,0.98,0.61,"NDC");
    tmPT2 = new TPaveText(0.50,0.46,0.98,0.56,"NDC");
    tdPT2 = new TPaveText(0.50,0.41,0.98,0.46,"NDC");
//  closeup text box locations  
  } else if(closeup) {    
    t0PT = new TPaveText(0.45,0.10,0.9,0.26,"NDC");
// these will be skipped for closeup plots but put here to avoid compiler warning
    tmPT = new TPaveText(0.50,0.17,0.98,0.31,"NDC");
    tdPT = new TPaveText(0.50,0.10,0.98,0.17,"NDC");
//  default text box locations  
  } else {
    t0PT = new TPaveText(0.50,0.64,0.98,0.80,"NDC");
    tmPT = new TPaveText(0.50,0.50,0.98,0.64,"NDC");
    tdPT = new TPaveText(0.50,0.43,0.98,0.50,"NDC");
  }

  h->GetXaxis()->SetTitle("time [ns]");
  data[0] = h->GetEntries();
  ft0 = h->GetFunction("mt_t0_fermi");
  if( ft0 ) {
// For closeup option plot only around +- 100 ns of T0 value
    if( closeup ) {
      double t0 = ft0->GetParameter(0);
      h->SetAxisRange(t0-50.,t0+150.);
      h->Draw();
    }
    // Change range for Run1 data
    if( ft0->GetParameter(0) > 200. ) {
      h->SetAxisRange(200.,1400.,"X");
      h->Draw();
    }
    t0PT->AddText(Form("T0 %.1lf #pm %.2lf ns",
		       ft0->GetParameter(0),ft0->GetParError(0)));
    t0PT->AddText(Form("Slope %.1lf #pm %.2lf /ns",
		       ft0->GetParameter(1),ft0->GetParError(1)));
    t0PT->SetTextColor(2);
    t0PT->Draw();
    data[1] = ft0->GetParameter(0);
    data[2] = ft0->GetParError(0);
    data[3] = ft0->GetParameter(1);
    data[4] = ft0->GetParError(1);
  } else {
    printf("  INFO: %s no function mt_t0_fermi\n",h->GetName());
  }
  if( closeup ) return;  //skip Tmax for closeup plots
  ftm = h->GetFunction("mt_tmax_fermi");
  if( ftm == 0 ) {
    printf("  INFO: %s no function mt_tmax_fermi\n",h->GetName());
    return;
  } else {
    ftm->SetLineColor(8);
    tmPT->AddText(Form("Tmax %.1lf #pm %.1lf ns",
		       ftm->GetParameter(0),ftm->GetParError(0)));
    tmPT->AddText(Form("Slope %.1lf #pm %.1lf /ns",
		       ftm->GetParameter(1),ftm->GetParError(1)));
    tmPT->SetTextColor(8);
    tmPT->Draw();
    dtmax = ftm->GetParameter(0) - ft0->GetParameter(0);
    dtmaxe = sqrt( ftm->GetParError(0)*ftm->GetParError(0)+
		   ftm->GetParError(0)*ft0->GetParError(0)  );
    tdPT->AddText(Form("DTmax %.1lf #pm %.1lf ns",dtmax,dtmaxe));
    tdPT->SetTextColor(1);
    tdPT->Draw();
    data[5] = dtmax;
    data[6] = dtmaxe;
    data[7] = ftm->GetParameter(1);
    data[8] = ftm->GetParError(1);
  }
  if( wiresag ) {
    float tmax = ftm->GetParameter(0);
    TF1 *sfit = new TF1("sfit","[0]+[6]/(1+exp((x-[1])/[2]))+([3]+x*[7])/(1+exp((x-[4])/[5]))",tmax-300.,tmax+300.);
    float amp = h->GetEntries()/20000.;
    sfit->SetParameters(10.,tmax-30.,10.,amp,tmax+30.,10.,amp,-0.1);
    sfit->SetLineColor(3);
    sfit->SetLineWidth(2);
    h->SetAxisRange(tmax-500.,tmax+500.);   //only display Tmax part of time spectrum
    h->Fit(sfit,"R+");
    //    t0PT->Draw();  //must redraw after fit
    tmPT->Draw();
    tdPT->Draw();
// Make sure tmax1 < tmax2
    int im1 = 1;
    int im2 = 4;
    if( sfit->GetParameter(4) < sfit->GetParameter(1) ) {
      im1 = 4;
      im2 = 1;
    }
    data[17] = sfit->GetParameter(im1+1);
    data[18] = sfit->GetParError(im1+1);
    tmPT1->AddText(Form("Tmax1 %.1lf #pm %.1lf ns",sfit->GetParameter(im1),sfit->GetParError(im1)));
    tmPT1->AddText(Form("Slope1 %.1lf #pm %.1lf /ns",data[17],data[18]));
    tmPT1->SetTextColor(8);
    tmPT1->Draw();
    data[13] = sfit->GetParameter(im1) - ft0->GetParameter(0);
    data[14] = sqrt( sfit->GetParError(im1)*sfit->GetParError(im1)+
		     ft0->GetParError(0)*ft0->GetParError(0)  );
    tdPT1->AddText(Form("DTmax1 %.1lf #pm %.1lf ns",data[13],data[14]));
    tdPT1->SetTextColor(1);
    tdPT1->Draw();
    data[19] = sfit->GetParameter(im2+1);
    data[20] = sfit->GetParError(im2+1);
    tmPT2->AddText(Form("Tmax2 %.1lf #pm %.1lf ns",sfit->GetParameter(im2),sfit->GetParError(im2)));
    tmPT2->AddText(Form("Slope2 %.1lf #pm %.1lf /ns",data[19],data[20]));
    tmPT2->SetTextColor(8);
    tmPT2->Draw();
    data[15] = sfit->GetParameter(im2) - ft0->GetParameter(0);
    data[16] = sqrt( sfit->GetParError(im2)*sfit->GetParError(im2)+
		     ft0->GetParError(0)*ft0->GetParError(0)  );
    tdPT2->AddText(Form("DTmax2 %.1lf #pm %.1lf ns",data[15],data[16]));
    tdPT2->SetTextColor(1);
    tdPT2->Draw();
  }  //end if

}    //end DrawTimeHist

/************************************************************
*  Draw fit info on plots
************************************************************/
void DrawBing(TH1F *h) {
  Double_t dtmax,dtmaxe;
  TF1 *ftm, *ft0;
  TPaveText *t0PT, *tdPT;

  gStyle->SetOptStat(0); 
  gStyle->SetTitleX(0.4);
  gStyle->SetTitleX(0.8);

  //  h->SetTitle("EIS1C14 Runs 200987-201269");
  h->SetAxisRange(400.,1400.);
  h->GetXaxis()->SetTitle("time [ns]");
  h->GetYaxis()->SetTitle("counts");
  h->GetXaxis()->SetTitleSize(0.05);
  h->GetYaxis()->SetTitleSize(0.05);
  h->GetXaxis()->SetTitleOffset(0.85);
  h->GetYaxis()->SetTitleOffset(1.1);
  h->Draw();

  TLatex l;
  l.SetTextAlign(23);
  l.SetTextSize(0.08);
  l.DrawLatex(1050,43500,"EIS1C14");
  l.DrawLatex(1050,39000,"Runs 200987-201269");

//  default text box locations  
  t0PT = new TPaveText(0.42,0.6,0.82,0.7,"NDC");
  tdPT = new TPaveText(0.42,0.5,0.82,0.60,"NDC");

  ft0 = h->GetFunction("mt_t0_fermi");
  if( ft0 ) {
    t0PT->AddText(Form("T0 %.1lf #pm %.2lf ns",
		       ft0->GetParameter(0),ft0->GetParError(0)));
    t0PT->SetTextColor(2);
    t0PT->Draw();
  } else {
    printf("  INFO: %s no function mt_t0_fermi\n",h->GetName());
  }
  ftm = h->GetFunction("mt_tmax_fermi");
  if( ftm == 0 ) {
    printf("  INFO: %s no function mt_tmax_fermi\n",h->GetName());
    return;
  } else {
    ftm->SetLineColor(8);
    //    tmPT->AddText(Form("Tmax %.1lf #pm %.1lf ns",
    //		       ftm->GetParameter(0),ftm->GetParError(0)));
    //    tmPT->AddText(Form("Slope %.1lf #pm %.1lf /ns",
    //		       ftm->GetParameter(1),ftm->GetParError(1)));
    //    tmPT->SetTextColor(8);
    //    tmPT->Draw();
    dtmax = ftm->GetParameter(0) - ft0->GetParameter(0);
    dtmaxe = sqrt( ftm->GetParError(0)*ftm->GetParError(0)+
		   ftm->GetParError(0)*ft0->GetParError(0)  );
    tdPT->AddText(Form("Tmax %.1lf #pm %.1lf ns",dtmax,dtmaxe));
    tdPT->SetTextColor(8);
    tdPT->Draw();
  }  //end if

}    //end DrawBing

/************************************************************
*  Draw fit info on plots
************************************************************/
void DrawAdcHist(TH1F *h, float data[NDATA] ) {
  TF1 *fadc;
  TPaveText *adcPT;

  gStyle->SetOptStat(1111); 
  h->Draw();
  adcPT = new TPaveText(0.58,0.33,0.98,0.65,"NDC");

  fadc = h->GetFunction("adc_fun");
  if( fadc ) {
    adcPT->AddText(Form("Peak %.1lf #pm %.1lf",
			fadc->GetParameter(1),fadc->GetParError(1)));
    adcPT->AddText(Form("Width %.1lf #pm %.1lf",
			fadc->GetParameter(2),fadc->GetParError(2)));
    adcPT->AddText(Form("Skew %.2lf #pm %.2lf",
			fadc->GetParameter(3),fadc->GetParError(3)));
    double dof = fadc->GetNDF();
    if( dof > 0. ) adcPT->AddText(Form("#Chi^{2}/dof     %.1lf",fadc->GetChisquare()/dof));
    adcPT->SetTextColor(2);
    adcPT->Draw();
    data[9]  = fadc->GetParameter(1);
    data[10] = fadc->GetParError(1);
    data[11] = fadc->GetParameter(2);
    data[12] = fadc->GetParError(2);
    data[13] = fadc->GetParameter(3);
    data[14] = fadc->GetParError(3);
  } else {
    printf("  INFO: %s no function adc_fun\n",h->GetName());
  }  //end if

}    //end DrawAdcHist
//  140523 Mezzcard numbering in MTT0 files does not take into account
//         that mezzcards numbers are swapped between ML1/2 for
//         various A/C L/S chambers.  Fix numbering by hand here.
int fix_mezzcard( char *chambername, int mz ) {
// 140523 FIX MEZZCARD NUMBERING (BEE, EE not included here)
  int flipped = 0;
  if(        chambername[0] == 'B' ) {  //barrel
    if(      chambername[2] == 'L' && chambername[4] == 'C') flipped=1; 
    else if( chambername[2] == 'S' && chambername[4] == 'A') flipped=1; 
    else if( chambername[2] == 'M' && chambername[2] == 'F' && chambername[4] == 'A') flipped=1; 
    else if( chambername[2] == 'O' && (chambername[2] == 'F' || chambername[2] == 'G') ) flipped=1; 
  } else if( chambername[4] == 'C' ) { //endcap C
    if(      chambername[2] == 'S' ) flipped=1;          
    else if( chambername[1] == 'I' && chambername[2] == 'L' && (chambername[3] == '4' || chambername[5] == '5') ) flipped=1;
  } else if( chambername[2] == 'L' ) { //endcap A
    flipped=1;  
  }
// Flip mezzcard 
  if( flipped ) {
    if( mz%2 == 0 ) mz++;
    else mz--;
  }
  return mz;
}   //end fix_mezzcard
