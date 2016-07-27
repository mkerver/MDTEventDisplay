//*****************************************************************
//   chamberplotter => Plot data vs chamber by wheel
//  filename = File with data to plot (format given below)
//             OR file with list of files with data to plot.  Data
//                from files will be plotted on same plots.
//  item     = Which item (column) of data to plot
//             0 => all data items plotted
//             N => only column N plotted
//
//  Format of data files
//  row 1: Names of data items (in column, separated by single space)
//  row 2: Units of data items (units must start with [, else ignored)
//         Should have "units" in first column
//  row 3: Minimum range for Y axis for variables
//         Should have "min" in first column
//  row 4: Maximum range for Y axis for variables
//         Should have "max" in first column
//  row 5: Fit Function for profile histogram (only 1 option g=gaus, any other letter=no fit)
//         Should have "fit" in first column
//  row 6+n: Columns of data
//  column 1: Hardware chamber name (e.g. EIL1A01)
//  column 2: Type of data: 
//    CHM=Chamber level, ML1=Multilayer1, MZ1=mezz card1, etc
//  column 2+n: data
//  NOTE: rows 2,3,4 are optional
//*****************************************************************
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TH1F.h>
#include <TH2F.h>

#include "chamberlist.h"
#include "chamberwheel.h"
#include "edplot.h"

using namespace std;

const int MAXDATAFILES=4;    //max number of data files
const int MINEVENTS = 0; //20;

//  Class to hold histograms
const int MAXITEMS=30;
class ChamberPlots {
public:
  char fname[80];
  char items[MAXITEMS][30];
  char units[MAXITEMS][30];
  int  errors[MAXITEMS];             //flag if item is an error for an item
  double hmin[MAXITEMS];             //min range for plot of items
  double hmax[MAXITEMS];             //max range for plot of items
  int   fit[MAXITEMS];               //flag for fit function.  1=gaussian 0=nofit
  TH1F *histentry[12][MAXITEMS];     //histos to record whether or not chamber has entry
  TH1F *histlist[12][MAXITEMS];      //histos for items in data file
  TH2F *bckh2[12][MAXITEMS];         //background histos
  TH1F *histlistp[15][MAXITEMS];     //profile hists per wheel, barrel, endcap, all
  int  nitems;
  ChamberPlots( const char *filename, const int color, const char *gtitle );
};
ChamberPlots::ChamberPlots( const char *filename, const int color, const char *gtitle ) {
  char *ptr,line[500],chamber[20];
  int  igw,ich,icham,iwheel=0;

  int  markers[10] = {0,20,22,20,22,27,29,23,24,33};

  nitems=0;

// Save file name up to "."
  for( int i=0; filename[i]!='\0' && filename[i]!='.'; i++ ) {
    fname[i] = filename[i];
    fname[i+1] = '\0';
  }

// Init arrays
  for( int i=0; i<MAXITEMS; i++ ) {
    sprintf(items[i],"Item%i",i);
    units[i][0] = '\0';
    hmin[i] = 0;
    hmax[i] = 0;
    errors[i] = -1;
    fit[i] = 0;
  }

//  Open input file
  std::fstream fin;
  fin.open(filename,std::fstream::in );
  if( fin.is_open() == 0 )  {
    printf("ERROR: Could not open %s\n",filename);
    return;
  }
//  printf("Opened %s\n",filename);
  
//  Read 1st line to get name of items 
  fin.getline(line,500);
//  printf("Read line %s\n",line);
  ptr=strtok(line," ");     //skip first 2 items 
  ptr=strtok(NULL," ");
  for( ptr=strtok(NULL," "); ptr!=NULL; nitems++, ptr=strtok(NULL," ") ) {
    if( nitems >= MAXITEMS ) printf("ERROR: Too many items - increase MAXITEMS\n");
    sprintf(items[nitems],"%s",ptr);
    //    printf("Found item %s\n",ptr);

//  Determine which items are errors of other items
    for( int j=0; j<nitems-1; j++ ) {
      if( strcmp( items[nitems], Form("%sErr",items[j] ) ) ) continue;
      errors[j] = nitems;
      break;
    }

//  Book histograms for each item for each wheel
    for( int iw=0; iw<12; iw++ ) {
//       histlist[iw][nitems] = new TH1F(Form("h1_%s_%s_%s",fname,chwheel[iw],ptr),
// 				      Form("%s %s %s %s",gtitle,fname,chwheel[iw],ptr),
// 				      wheelmax[iw%6],1,wheelmax[iw%6]+1);
      histlist[iw][nitems] = new TH1F(Form("h1_%s_%s_%s",fname,chwheel[iw],ptr),
				      Form("%s %s %s",gtitle,chwheel[iw],ptr),
				      wheelmax[iw%6],1,wheelmax[iw%6]+1);
      histlist[iw][nitems]->SetLineColor(color);
      histlist[iw][nitems]->SetMarkerColor(color);
      histlist[iw][nitems]->SetMarkerStyle(markers[color]);
      //      histlist[iw][nitems]->SetMarkerStyle(2+color);
//  Histograms to keep track of non-null entries
      histentry[iw][nitems] = new TH1F(Form("h1_%s_%s_%s_entry",fname,chwheel[iw],ptr),
				       Form("%s %s %s",gtitle,chwheel[iw],ptr),
				       wheelmax[iw%6],1,wheelmax[iw%6]+1);
    }
  }

// Loop over rest of file and fill histograms
  int ncham = 0;
  while( fin.getline(line,500) && !fin.eof() ) {
    //    printf("Read line %s\n",line);

    ptr=strtok(line," ");     //get first item in line 

//  Check for units line 2nd line to get units of items 
//  If unit does not begin with "[" it is ignored
    if( strcmp( ptr, "units" ) == 0 ) {
      ptr=strtok(NULL," ");   //skip 2nd item
      ptr=strtok(NULL," ");
      for( int ii=0; ptr!=NULL; ii++, ptr=strtok(NULL," ") ) {
	if( ptr[0] == '[' ) sprintf(units[ii],"%s",ptr);
	else if( ptr[0] == 'X' ) sprintf(units[ii],"X");       //if X then ignore data
	else units[ii][0] = '\0';
	//	printf("  Read units %s\n",ptr);
      }
      continue;
//  Read 3rd line to get minimums for plots
    } else if( strcmp( ptr, "min" ) == 0 ) {
      ptr=strtok(NULL," ");   //skip 2nd item
      ptr=strtok(NULL," ");
      for( int ii=0; ptr!=NULL; ii++, ptr=strtok(NULL," ") ) {
	hmin[ii] = atof(ptr);
      }
      continue;
//  Read 4th line to get maximums for plots
    } else if( strcmp( ptr, "max" ) == 0 ) {
      ptr=strtok(NULL," ");   //skip 2nd item      
      ptr=strtok(NULL," ");
      for( int ii=0; ptr!=NULL; ii++, ptr=strtok(NULL," ") ) {
	hmax[ii] = atof(ptr); 
      }
      continue;
//  Read fit line 
    } else if( strcmp( ptr, "fit" ) == 0 ) {
      ptr=strtok(NULL," ");   //skip 2nd item      
      ptr=strtok(NULL," ");
      for( int ii=0; ptr!=NULL; ii++, ptr=strtok(NULL," ") ) {
	if( strcmp( ptr, "g" ) == 0 ) fit[ii] = 1; 
      }
      continue;
    }  // end checking header lines

    sprintf(chamber,"%s",ptr);
    ptr = strtok(NULL," ");
//only do CHM lines
    if( strcmp(ptr,"CHM") == 0 ) { 
      //      printf("  Doing %s %i\n",chamber,nitems);
      igw = get_chwheel(chamber); //get iwheel, icham, ich for new chamber
      if( igw == -1 ) {
        printf("ERROR: invalid chamber %s\n",line);
        continue;
      }
      ncham++;
      ich    = igw/100000;
      icham  = (igw-ich*100000)/100;
      iwheel = igw - 100*icham - ich*100000; 
//  Fill histograms for each item for each wheel
      ptr=strtok(NULL," ");
      int skip=0;
      for( int ii=0; ii<nitems && ptr!=NULL; ii++, ptr=strtok(NULL," ") ) {
        if( units[ii][0] == 'X') continue;
		printf("  load item %i %i %f\n",ii,icham,atof(ptr));
//  Skip filling values for chambers with very few events.
	if( !skip ) {
	  histlist[iwheel][ii]->SetBinContent(icham,atof(ptr)); 
	  histentry[iwheel][ii]->Fill(icham); 
// If item is has an error add error bars
	  for( int j=0; j<MAXITEMS; j++ ) {
	    if( errors[j] == ii ) 
	      histlist[iwheel][j]->SetBinError(icham,atof(ptr)); 
	  }
	}
	if( ii==0 && atof(ptr) < MINEVENTS) skip=1;
      }  //end loop over items
    }    //end if CHM line
  }      //end loop over file lines
  printf("Read %i items for %i chambers from %s\n",nitems,ncham,filename);

//  Now make background histograms
  for( int ii=0; ii<nitems; ii++ ) {
    if( units[ii][0] == 'X') continue;
//  Set max to max value of A/C side
    Double_t maxes[6],mins[6];
    for( int iw=0; iw<12; iw++ ) {    //loop over wheels
//  If min/max not specified set limits from data itself
      if( iw < 6 ) {
	if( hmin[ii]==0 && hmax[ii]==0 ) {
	  if( histlist[iw][ii]->GetMaximum() > histlist[iw+6][ii]->GetMaximum() ) {
	    maxes[iw] = histlist[iw][ii]->GetMaximum();
	  } else {
	    maxes[iw] = histlist[iw+6][ii]->GetMaximum();
	  }
	  if( histlist[iw][ii]->GetMinimum() < histlist[iw+6][ii]->GetMinimum() ) {
	    mins[iw] = histlist[iw][ii]->GetMinimum();
	  } else {
	    mins[iw] = histlist[iw+6][ii]->GetMinimum();
	  }
	  if( mins[iw] < 0. ) {
	    if( -mins[iw] > maxes[iw] ) {
	      mins[iw] *= 1.05;
	      maxes[iw] = -mins[iw];
	    } else {
	      maxes[iw] *= 1.05;
	      mins[iw] = -maxes[iw];
	    }
	  } else {
	    mins[iw] = 0.;
	    maxes[iw] *= 1.05;
	  }
// Prevent excessively large mins, maxes
// 	  Float_t x = 100.*histlist[iw][ii]->GetMean();
// 	  if( -mins[iw] > x ) {
// 	    printf("WARNING: MIN TOO LARGE %s %s min=%.1lf max=%.1lf RESET min to %lf\n",items[ii],chwheel[iw],mins[iw%6],maxes[iw%6],x);
// 	    mins[iw] = -x;
// 	  }
// 	  if( maxes[iw] > x ) {
// 	    printf("WARNING: MAX TOO LARGE %s %s min=%.1lf max=%.1lf RESET max to %lf\n",items[ii],chwheel[iw],mins[iw%6],maxes[iw%6],x);
// 	    maxes[iw] = x;
// 	  }
	} else {
	  mins[iw]  = hmin[ii];
	  maxes[iw] = hmax[ii];
	}
      }    //end if iw<6
//       printf("%s %s min=%.1lf max=%.1lf\n",items[ii],chwheel[iw],mins[iw%6],maxes[iw%6]);
//       printf("BOOK histo %s %s %s %s\n",gtitle,fname,chwheel[iw],items[ii]);   //DEBUG
//       bckh2[iw][ii] = new TH2F(Form("bckh2_%s_%s_%s",fname,chwheel[iw],items[ii]),
// 			       Form("%s %s %s %s",gtitle,fname,chwheel[iw],items[ii]), 
// 			   wheelbins[iw%6],binedges[iw%6],100,mins[iw%6],maxes[iw%6]);
      bckh2[iw][ii] = new TH2F(Form("bckh2_%s_%s_%s",fname,chwheel[iw],items[ii]),
			       Form("%s %s %s",gtitle,chwheel[iw],items[ii]), 
			   wheelbins[iw%6],binedges[iw%6],100,mins[iw%6],maxes[iw%6]);
      bckh2[iw][ii]->SetStats(0);
      bckh2[iw][ii]->GetXaxis()->SetTitle("chamber (by phi & eta)");
      bckh2[iw][ii]->GetYaxis()->SetTitle(Form("%s %s",items[ii],units[ii]));
      bckh2[iw][ii]->GetXaxis()->SetTickLength(0);  //suppress X ticks
      AddHistBinLabels(bckh2[iw][ii],&binlabels[iw%6][0]);

//  Make profile histograms
//      printf("BOOK histo %s %s %s\n",gtitle,chwheel[iw],items[ii]);   //DEBUG
      histlistp[iw][ii] = new TH1F(Form("%s_%s_%s",fname,chwheel[iw],items[ii]),
				   Form("%s %s %s",gtitle,chwheel[iw],items[ii]), 
			   100,mins[iw%6],maxes[iw%6]);
      histlistp[iw][ii]->SetStats(1110);
      histlistp[iw][ii]->SetLineColor(color);
      histlistp[iw][ii]->GetXaxis()->SetTitle(Form("%s %s",items[ii],units[ii]));
//  first time through book barrel, endcap profile histos
      if( iw == 0 ) {
	for( int eb=12; eb<15; eb++ ) { 
	  char *chnames[] = {"Barrel", "Endcap", "All"};
//	  printf("BOOK %s %s %s\n",gtitle,chnames[eb-12],items[ii]);    //DEBUG
	  histlistp[eb][ii] = new TH1F(Form("%s_%s_%s",fname,chnames[eb-12],items[ii]),
				       Form("%s %s %s",gtitle,chnames[eb-12],items[ii]), 
				       100,mins[iw%6],maxes[iw%6]);
	  histlistp[eb][ii]->SetStats(1110);
	  histlistp[eb][ii]->SetLineColor(color);
	  histlistp[eb][ii]->GetXaxis()->SetTitle(Form("%s %s",items[ii],units[ii]));
	}
      }  //end if iw==0
//  Fill profile histos
      for( int ibin=1; ibin<=wheelmax[iw%6]; ibin++ ) {
//  Check if have entry for this chamber
	if( histentry[iw][ii]->GetBinContent(ibin) ) {
          Float_t x = histlist[iw][ii]->GetBinContent(ibin);
//==0 means no data, so reset these to be off the plot
	  if( x == 0. ) { 
	    //	    printf("ZERO iw=%i ii=%i\n",iw,ii);
	    histlist[iw][ii]->SetBinContent(ibin,-10000.);
	    continue;
	  }
	  histlistp[iw][ii]->Fill(x);
	  histlistp[14][ii]->Fill(x);
	  if( chwheel[iw][0] == 'B' ) histlistp[12][ii]->Fill(x);
	  else                        histlistp[13][ii]->Fill(x);
	} else {
//  Fill unfilled bins  entry of 1000000 so bin will not show up on plots
	  histlist[iw][ii]->SetBinContent(ibin,-1000000.); 
	}

      }
    }  //end loop over wheels (iw)
  }    //end loop over items  (ii)

}     //end ChamberPlots::ChamberPlots

//****************************************************************************
// list.txt = list of files of chamber data 
// item = which item to plot; 
//        0  => ALL
//        -1 => special setting for ATLAS 
// gtitle = Global title added to all histogram names
//****************************************************************************
void chamberplotter( const char *filename="list.txt", const int item=0, const char *gtitle="" ) {
  int nfiles=1,minitem=0,maxitem=1;
  char fname[255];
  ChamberPlots *chplot[MAXDATAFILES];

  gStyle->SetOptFit(1);

//  Open file and check if it is a list of data files or a data file.
//  If first word is "chamber" then assume is a data file.
  std::fstream fin;
  fin.open(filename,std::fstream::in );
  if( fin.is_open() == 0 )  {
    printf("ERROR: Could not open %s\n",filename);
    return;
  }
//  Read 1st line to get name of items 
  fin.getline(fname,10);
  fin.close();

//  If it is a single data file
  if( fname[0]=='c' && fname[1]=='h' && fname[2]=='a' && fname[3]=='m' ) {
    chplot[0] = new ChamberPlots( filename, 1, gtitle );
// Else it is a list of datafiles
  } else {
    fin.open(filename,std::fstream::in );
    nfiles = 0;
    while( fin>>fname && !fin.eof() && nfiles < MAXDATAFILES ) {
      chplot[nfiles] = new ChamberPlots( fname, 1+nfiles, gtitle );
      nfiles++; 
    }
    fin.close();
  }

// Save file name up to "." which will be used for plot names base
  for( int i=0; filename[i]!='\0' && filename[i]!='.'; i++ ) {
    fname[i] = filename[i];
    fname[i+1] = '\0';
  }

//  Open ROOT file to save histograms
  TFile *f = new TFile(Form("%s_hist.root",fname),"recreate");
  if (f->IsZombie()) {   
    printf("ERROR opening ROOT output file\n");
    return;
  }

//  Make colored background histograms
  TH1F *bckgd[6];
  for( int iw=0; iw<6; iw++ ) {
    bckgd[iw] = new TH1F(Form("%s_tzbg",chwheel[iw]),"",
			 wheelbins[iw],binedges[iw]);
  }

  TCanvas *tzcan[12],*tzcanh1[14];
  for( int iw=0; iw<12; iw++ ) {
    tzcan[iw]   = new TCanvas(Form("tzcan%i",iw), chwheel[iw],800,600);
    tzcanh1[iw] = new TCanvas(Form("tzcanh1%i",iw), Form("%s_pr",chwheel[iw]),600,600);
  }
  tzcanh1[12] = new TCanvas("tzcanh1barrel","Barrel",600,600);
  tzcanh1[13] = new TCanvas("tzcanh1endcap","Endcap",600,600);

//  Set range of plots to make
  if( item > 0 ) {
    minitem = item-1;
    maxitem = item;
    printf("Plot item %i %s\n",item,chplot[0]->items[minitem]);
  } else {
    maxitem = chplot[0]->nitems;
  }

  TLegend *legend = new TLegend( 0.7, 0.99-nfiles*0.08, 0.99, 0.99 );
  for( int ff=0; ff<nfiles; ff++ ) {
    for( int ii=minitem; ii<maxitem; ii++ ) {
// Skip errors
      if( chplot[ff]->units[ii][0] == 'X' ) continue;
      legend->AddEntry( chplot[ff]->histlist[0][ii], chplot[ff]->fname );
      break;
    }
  }

//  Loop over items and wheels and make plots
  for( int ii=minitem; ii<maxitem; ii++ ) {
// Skip errors
    if( strstr( chplot[0]->items[ii], "Err" ) != NULL || chplot[0]->units[ii][0] == 'X' ) continue;

//  Draw plots for all wheels
    for( int iw=0; iw<15; iw++ ) {
      char *chnames[] = {"Barrel", "Endcap", "All"};
//  Find which histogram has max range and use that for all histos
      Double_t ymax = chplot[0]->histlistp[iw][ii]->GetMaximum();
      for( int ff=1; ff<nfiles; ff++ ) {
	if( ymax < chplot[ff]->histlistp[iw][ii]->GetMaximum() )
	  ymax = chplot[ff]->histlistp[iw][ii]->GetMaximum();
      }
      ymax *= 1.05;
//  Skip this wheel if it has no data
      if( ymax == 0. ) {
	printf("Item %s Wheel %s No data - skipping\n",chplot[0]->items[ii],chwheel[iw]);
	continue;
      }
// Plot profile histos
      tzcanh1[iw]->cd();
//  Plot histos
      for( int ff=0; ff<nfiles; ff++ ) {
//  Fit a gaussian if desired  
//        if( chplot[ff]->fit[ii] ) chplot[ff]->histlistp[iw][ii]->Fit("gaus","0");
        if( chplot[ff]->fit[ii] ) chplot[ff]->histlistp[iw][ii]->Fit("gaus");
	chplot[ff]->histlistp[iw][ii]->SetAxisRange(0,ymax,"Y");
	if( ff == 0 ) chplot[ff]->histlistp[iw][ii]->Draw("");
	else          chplot[ff]->histlistp[iw][ii]->Draw("sames");
// Save histogram to file    
        chplot[ff]->histlistp[iw][ii]->Write();
      }
      tzcanh1[iw]->Update();
//  Change color of stats box to match line color of corresponding histogram
      TPaveStats *p1;
      float frac = 0.2;
      if( nfiles > 2 ) frac = 1./((float)2*nfiles);
      for( int ff=0; ff<nfiles; ff++ ) {
	p1 = (TPaveStats*)chplot[ff]->histlistp[iw][ii]->GetListOfFunctions()->FindObject("stats");
	chplot[ff]->histlistp[iw][ii]->GetListOfFunctions()->Remove(p1);
	chplot[ff]->histlistp[iw][ii]->SetStats(0);
	p1->SetX1NDC(0.6);
	p1->SetX2NDC(1.0);
	p1->SetY1NDC(1.0-frac*ff-frac);
	p1->SetY2NDC(1.0-frac*ff);
	p1->SetTextColor(chplot[ff]->histlistp[iw][ii]->GetLineColor());
	//	p1->SetName(chplot[ff]->fname);
	p1->Draw();
      }
      if( iw < 12 ) {
	tzcanh1[iw]->Print(Form("%s_%s_%s_pr.png",fname,chwheel[iw],chplot[0]->items[ii]));
      } else {
	tzcanh1[iw]->Print(Form("%s_%s_%s_pr.png",fname,chnames[iw-12],chplot[0]->items[ii]));
	continue;
      }
      tzcan[iw]->cd();
//  Plot background histogram
      Background( tzcan[iw], bckgd[iw%6], chplot[0]->bckh2[iw][ii], 18 ); 
//  Plot data
      for( int ff=0; ff<nfiles; ff++ ) {
//  If item has error bars, plot them
	if( chplot[ff]->errors[ii] != -1 ) chplot[ff]->histlist[iw][ii]->Draw("epsame");
	else             chplot[ff]->histlist[iw][ii]->Draw("SAMEP");
      }
      legend->Draw();
      tzcan[iw]->Print(Form("%s_%s_%s.png",fname,chwheel[iw],chplot[0]->items[ii]));

    }   //end loop over wheels

  }     //end loop over plots

}       //end chamberplotter()
