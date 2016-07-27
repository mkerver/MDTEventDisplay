//*****************************************************************
//   dateplotter => Plot data vs day of year + make profile histograms of data.
//
//  filename = File with data to plot (format given below)
//             OR 
//             A file with list of files with data to plot.  Data
//             from multiple files will be plotted on same plots.
//             Filenames should end in ".txt".  The data are labelled in the
//             legend using the filename of the data, with the ".txt" removed.
//  item     = Which item (column) of data to plot
//             0 => all data items plotted (default)
//             N => only column N plotted
//  legend   = 0 no legend
//           = 1 put legend top right    (default)
//           = 2 put legend bottom right
//           = 3 put legend top left
//           = 4 put legend bottom left
//  ititle   = 1 plot title 
//           = 0 no title                (default)
//  ylab     = Y axis label for profile plot
//
//  Format of data files
//  row 1: Names of data items (in column, separated by single space)
//         If name ends in "Err", e.g. of form <name>Err then column
//         will be used for error bars of item named <name>
//         Can have up to MAXITEMS=30 items.  
//         These names will be used for the Y axis labels and names of plots.
//         Names can contain Latex commands as understood by ROOT.  
//         The first column must always be named "date".  
//         The dates in column 1 are used for the X axis.
//         The data in other columns is plotted versus date.
//         The Xaxis is scaled to the range of months present in the data.
//         At this time the script can only do a maximum of 1 calender year of data.
//         
//  Optional rows to specify plotting instructions:
//  row 2: Units of data items (units should be put in [], else ignored)
//         Should have "units" in first column
//         If the item has no units put an "x" as a placeholder.
//         IF you do not want the item to be plotted put X as unit
//  row 3: Minimum range for Y axis for variables
//         Should have "min" in first column
//  row 4: Maximum range for Y axis for variables
//         Should have "max" in first column
//         If both min and max are 0, the range is calculated from the data itself (default)
//  row 5: Fit Function for profile histogram (only 1 option g=gaus, any other letter=no fit)
//         Should have "fit" in first column
//  row 6: Normalization option for data vs day plot
//         Should have "norm" in first column
//         0 = No normalization (default)
//         1 = Normalize to first data point
//         2 = Normalize to mean of profile histogram
//  row 7+n: Columns of data
//  column 1: Date in format YYYYMMDD, YYYY-MM-DD, or DD-MMM-YYYY (MMM=Jan,Feb, etc), MUST be ascending dates in order.
//            At this time only 1 value per date is allowed.  Could easily add HH:MM decoding, however, to allow for
//            multiple data in a single day (but plot may be messy).
//  column 2+n: data
//  NOTE: rows 2-6 are optional, and can come in any order
//  All lines MUST have the same number of columns.
// SAMPLE FILE FORMAT
// date       T0      T0Err
// units    [ns]       [ns]
// min      400         0
// max      700        10
// norm      2          0
// marker    22        22
// 2011-03-30 456.454 0.512
// 2011-04-01 453.445 0.615
//
//  Outputs for each data item.
//  <item_name>.png      => Plot of data by day for data item <item_name>
//  <item_name>_pr.png   => Profile histogram of data item <item_name>
//  If a list of files has been used these names the the list file name will be prepended to these names.
//
//  If your item_name contain crufty Latex names like "#deltaT0" then your file names will also have these
//  funky characters.  You can clean up the file names with this script:
//  /afs/atlas.umich.edu/home/muoncal/bin/cleanfilenames.sh
// 
//  A convenient way to run this script is:
//  /afs/atlas.umich.edu/home/muoncal/bin/runroot.sh dateplotter.C <datafile>
//  "runroot.sh" will run any root script from the command line, and command line arguments as parameters.
//
//*****************************************************************
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TGraphErrors.h>
#include <TMath.h>

#include "/afs/atlas.umich.edu/home/muoncal/public/root/dateplot.h"
#include "/afs/atlas.umich.edu/home/muoncal/public/root/edplot.h"

using namespace std;

const int MAXDATAFILES=10;    //max number of data files
const int MINEVENTS = 100;

//  Class to hold histograms
const int MAXITEMS=30;
class DatePlots {
public:
  char fname[80];
  int  nitems;                       //number of items (data columns) in file
  char items[MAXITEMS][30];          //names of items
  char units[MAXITEMS][30];          //units of items
  int  norm[MAXITEMS];               //flag controling data normalization
  double hmin[MAXITEMS];             //min range for plot of items
  double hmax[MAXITEMS];             //max range for plot of items
  int   fit[MAXITEMS];               //flag for fit function.  1=gaussian 0=nofit
  int npts;                          //number of points in TGraph plots
  double yearday[365];               //X array for TGraph - day of the year
  double data[MAXITEMS][365];        //Y array for TGraph
  TGraphErrors *grlist[MAXITEMS];    //graphs for items in data file
  TH1F *histlistp[MAXITEMS];         //profile hists of data
  TH1F *bckgd;                       //striped background histogram
  TH2F *bckh2[MAXITEMS];             //background histos - set X,Y scale for plots
  int  minmonth;                     //minimum month in data (Jan=0)
  int  maxmonth;                     //maximum month in data
  int  nmonths;                      //number of months in data
  DatePlots( const char *filename, const int color, const char *gtitle );
};
DatePlots::DatePlots( const char *filename, const int color, const char *gtitle ) {
  char *ptr,line[500];
  int  markers[10] = {0,20,22,20,22,27,29,23,24,33};

// Save file name up to "."
  for( int i=0; filename[i]!='\0' && filename[i]!='.'; i++ ) {
    fname[i] = filename[i];
    fname[i+1] = '\0';
  }

// Init arrays
  npts = 0;
  minmonth = -1;
  maxmonth = -1;
  for( int i=0; i<MAXITEMS; i++ ) {
    sprintf(items[i],"Item%i",i);
    units[i][0] = '\0';
    hmin[i] = 0;
    hmax[i] = 0;
    norm[i] = 0;
    fit[i] = 0;
    for( int j=0; j<365; j++ ) {
      yearday[j] = 0.;          
      data[i][j] = 0.; 
    }
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
  nitems=0;
  ptr=strtok(line," ");
  for( ptr=strtok(NULL," ") ; ptr!=NULL; nitems++, ptr=strtok(NULL," ") ) {
    if( nitems >= MAXITEMS ) printf("ERROR: Too many items - increase MAXITEMS\n");
    sprintf(items[nitems],"%s",ptr);
    //    printf("Found item %i %s\n",nitems,ptr);
  }

// Loop over rest of file and save data
  while( fin.getline(line,500) && !fin.eof() ) {
    //            printf("Read line %s\n",line);     //debug
    ptr=strtok(line," ");     //get first item in line 

//  Check for units line 2nd line to get units of items 
//  If unit does not begin with "[" it is ignored
    if( strcmp( ptr, "units" ) == 0 ) {
      ptr=strtok(NULL," ");   //get 2nd item
      for( int ii=0; ptr!=NULL; ii++, ptr=strtok(NULL," ") ) {
	if( ptr[0] == '[' ) sprintf(units[ii],"%s",ptr);
	else if( ptr[0] == 'X' ) sprintf(units[ii],"X");       //if X then ignore data
	else units[ii][0] = '\0';
      }
      continue;
//  Read 3rd line to get minimums for plots
    } else if( strcmp( ptr, "min" ) == 0 ) {
      ptr=strtok(NULL," ");   //get 2nd item
      for( int ii=0; ptr!=NULL; ii++, ptr=strtok(NULL," ") ) {
	hmin[ii] = atof(ptr);
      }
      continue;
//  Read 4th line to get maximums for plots
    } else if( strcmp( ptr, "max" ) == 0 ) {
      ptr=strtok(NULL," ");   //get 2nd item      
      for( int ii=0; ptr!=NULL; ii++, ptr=strtok(NULL," ") ) {
	hmax[ii] = atof(ptr); 
      }
      continue;
//  Read fit line 
    } else if( strcmp( ptr, "fit" ) == 0 ) {
      ptr=strtok(NULL," ");   //get 2nd item      
      for( int ii=0; ptr!=NULL; ii++, ptr=strtok(NULL," ") ) {
	if( strcmp( ptr, "g" ) == 0 ) fit[ii] = 1; 
      }
      continue;
//  Read line to get type of data value (value or error) for plots
//  If item is an "error" it will be used to make error bars on plots
    } else if( strcmp( ptr, "norm" ) == 0 ) {
      ptr=strtok(NULL," ");   //get 2nd item      
      for( int ii=0; ptr!=NULL; ii++, ptr=strtok(NULL," ") ) {
        norm[ii] = atoi(ptr); 
      }
      continue;
    } else if( strcmp( ptr, "marker" ) == 0 ) {
      ptr=strtok(NULL," ");   //get 2nd item      
      for( int ii=0; ptr!=NULL; ii++, ptr=strtok(NULL," ") ) {
        markers[ii] = atoi(ptr); 
      }
      continue;
    }

// Else must be a data line.  First item should be a date
//    printf("Find yearday for date %s point %i\n",ptr,npts);  //debug

    yearday[npts] = getday(ptr);
    if( minmonth == -1 ) minmonth = (int) getday(ptr,1);
    else                 maxmonth = (int) getday(ptr,1);
 //  Check that dates are in ascending order 
    if( npts && yearday[npts] < yearday[npts-1] ) {
      npts=0;   //set to zero to prevent plotting of this data
      printf("ERROR: Data not in ascending order by date! Bad date is %s file %s\n",ptr,filename);
      return;
    }
    //    printf("This is a date %s %.0lf point %i\n",ptr,yearday[npts],npts);//debug

    ptr = strtok(NULL," ");
    for( int ii=0; ii<nitems && ptr!=NULL; ii++, ptr=strtok(NULL," ") ) {
      if( units[ii][0] != 'X') data[ii][npts] = atof(ptr);
    }
    npts++;

  }      //end loop over file lines
  printf("Read %i datapoints for %i items from %s\n",npts,nitems,filename);//debug

//  Make colored background histogram - set range to months used in data
//  You actually only need 1 of these histograms, but its easier just to make one per file.
  nmonths = maxmonth - minmonth + 1;
  bckgd = new TH1F(Form("bckgd%i",color),"background",nmonths,&monthedges[minmonth]);

//  Now make TGraphs, background histograms, profile histograms
  for( int ii=0; ii<nitems; ii++ ) {
    if( units[ii][0] == 'X') continue;   //data item not for plotting 
//  Determine range for profile histograms from data
    double pmax = TMath::MaxElement(npts, data[ii]);
    double pmin = TMath::MinElement(npts, data[ii]);
    double range = pmax - pmin;
    pmax += 0.05*range;
    pmin -= 0.05*range;

//  Make profile histograms.  Do first in case normalizing with them
    histlistp[ii] = new TH1F(Form("%s_%s",fname,items[ii]),
			     Form("%s %s",gtitle,items[ii]), 
			     100,pmin,pmax);
    histlistp[ii]->SetStats(1110);
    histlistp[ii]->SetLineColor(color);
    histlistp[ii]->GetXaxis()->SetTitle(Form("%s %s",items[ii],units[ii]));
    histlistp[ii]->GetYaxis()->SetTitleOffset(1.2);

//  Fill profile histos
    for( int ipt=0; ipt<npts; ipt++ ) histlistp[ii]->Fill(data[ii][ipt]);

// Normalize data if needed.
    double xmean = 0;
    if(      norm[ii] == 1 ) xmean = data[ii][0];               //normalize to first data point
    else if( norm[ii] == 2 ) xmean = histlistp[ii]->GetMean();  //normalize to mean of profile histogram
    if( xmean != 0. ) 
      for( int ipt=0; ipt<npts; ipt++ ) data[ii][ipt] -= xmean;

//  If min/max not specified set limits from data itself
//  Make plotting range 10% bigger than data range so points are not at edges of plot
    if( hmin[ii]==0 && hmax[ii]==0 ) {
      hmin[ii] = pmin;
      hmax[ii] = pmax;
// If doing normalization, make range symmetric about zero
      if( norm[ii] ) {
        if( TMath::Abs(hmax[ii]) > TMath::Abs(hmin[ii]) ) hmin[ii] = -hmax[ii]; 
	else hmax[ii] = -hmin[ii];
      }
    }
    //    printf("Min, Max for %s %.3lf %.3lf\n",items[ii],hmin[ii],hmax[ii]);   //debug

// Book the background histogram used to make XY scale for plots
    bckh2[ii] = new TH2F(Form("bckh2_%s_%s",fname,items[ii]),
			 Form("%s %s",gtitle,items[ii]), 
			 nmonths,&monthedges[minmonth],100,hmin[ii],hmax[ii]);
    bckh2[ii]->SetStats(0);
    bckh2[ii]->GetXaxis()->SetTitle("month");
    bckh2[ii]->GetYaxis()->SetTitle(Form("%s %s",items[ii],units[ii]));
    bckh2[ii]->GetXaxis()->SetTickLength(0);    //suppress X ticks
    AddHistBinLabels(bckh2[ii],&monthlabels[minmonth],nmonths);
    //    printf("Book TGraph for %s\n",items[ii]);
// Make TGraphs of data
// Check if the item has an error, if so add error bars
    double nullerrors[365] = { 365*0. };     //, *errorptr=NULL;
    double *errorptr = nullerrors;
    for( int j=ii+1; j<nitems; j++ ) {
      if( strcmp( items[j], Form("%sErr",items[ii] ) ) ) continue;
      errorptr = data[j];
      break;
    }
    grlist[ii] = new TGraphErrors( npts, yearday, data[ii], nullerrors, errorptr );
    grlist[ii]->SetMarkerColor(color);
    grlist[ii]->SetMarkerStyle(markers[color]);
    grlist[ii]->SetMarkerSize(1);

  }    //end loop over items  (ii)

}     //end DatePlots::DatePlots

//****************************************************************************
// list.txt = list of files of chamber data 
// item = which item to plot; 
//        0  => ALL
//        -1 => special setting for ATLAS 
//****************************************************************************
void dateplotter( const char *filename="list.txt", const int item=0, const int ilegend=1, 
		  const int ititle=1, const char *ytitle="" ) {
  int nfiles=1,minitem=0,maxitem=1;
  char fname[255],line[255];
  DatePlots *dateplot[MAXDATAFILES];
  int colors[] = { 1,2,3,4,6,7,8,9 };   //skip 5=yellow

  gStyle->SetOptFit(1);
  gStyle->SetTitleH(0.09);  
  gStyle->SetTitleX(10);  
  /*  int icol=0;   //use white background
  gStyle->SetFrameBorderMode(icol);
  gStyle->SetFrameFillColor(icol);
  gStyle->SetCanvasBorderMode(icol);
  gStyle->SetCanvasColor(icol);
  gStyle->SetPadBorderMode(icol);
  gStyle->SetPadColor(icol);
  gStyle->SetStatColor(icol);
  */
//  Open file and check if it is a list of data files or a data file.
//  If first word is "date" then assume is a data file.
  std::fstream fin;
  fin.open(filename,std::fstream::in );
  if( fin.is_open() == 0 )  {
    printf("ERROR: Could not open %s\n",filename);
    return;
  }

// Save file name up to "." which will be used for plot names base
  for( int i=0; filename[i]!='\0' && filename[i]!='.'; i++ ) {
    fname[i] = filename[i];
    fname[i+1] = '\0';
  }

//  Read 1st line to get name of items 
  fin.getline(line,10);
  fin.close();

//  If it is a single data file
  if( line[0]=='d' && line[1]=='a' && line[2]=='t' && line[3]=='e' ) {
    dateplot[0] = new DatePlots( filename, 1, fname );
    if( dateplot[0]->npts == 0 ) return;   //quit if error reading file
// Else it is a list of datafiles
  } else {
    fin.open(filename,std::fstream::in );
    nfiles = 0;
    while( fin>>line && !fin.eof() && nfiles < MAXDATAFILES ) {
      dateplot[nfiles] = new DatePlots( line, colors[nfiles], fname );
      if( dateplot[nfiles]->npts == 0 ) return;   //quit if error reading file
      nfiles++; 
    }
    fin.close();
  }

//  Open ROOT file to save histograms
  TFile *f = new TFile(Form("%s_dateplots.root",fname),"recreate");
  if (f->IsZombie()) {   
    printf("ERROR opening ROOT output file\n");
    return;
  }

  TCanvas *tzcan   = new TCanvas("tzcan", "dateplot",1000,600);
  TCanvas *tzcanh1 = new TCanvas("tzcanh1", "dateploth1",600,600);

//  Set range of plots to make
  if( item > 0 ) {
    minitem = item-1;
    maxitem = item;
  } else {
    maxitem = dateplot[0]->nitems;
  }

  int nchar = strlen(dateplot[0]->fname);
  //  TLegend *legend = new TLegend( 0.98-0.015*nchar, 0.99-nfiles*0.05, 0.99, 0.99 );
  TLegend *legend;
  if( ilegend==3 )      legend = new TLegend( 0.11, 0.11+nfiles*0.08, 0.11+0.025*nchar, 0.11 );
  else if( ilegend==3 ) legend = new TLegend( 0.12, 0.89-nfiles*0.08, 0.12+0.025*nchar, 0.89 );
  else if( ilegend==2 ) legend = new TLegend( 0.98-0.025*nchar, 0.1+nfiles*0.08, 0.99, 0.1 );
  else                  legend = new TLegend( 0.98-0.025*nchar, 0.99-nfiles*0.08, 0.99, 0.99 );
  for( int ff=0; ff<nfiles; ff++ ) {
    for( int ii=minitem; ii<maxitem; ii++ ) {
      if( dateplot[ff]->units[ii][0] != 'X' ) {
	printf("Make legend for file=%i item=%i %s\n",ff,ii,dateplot[ff]->fname);
        legend->AddEntry( dateplot[ff]->grlist[ii], dateplot[ff]->fname );
	break;
      }
    }
  }

//  Loop over items and wheels and make plots
  for( int ii=minitem; ii<maxitem; ii++ ) {
// Skip errors and items not to be plotted
    if( strstr( dateplot[0]->items[ii], "Err" ) != NULL || dateplot[0]->units[ii][0] == 'X' ) continue;

// Plot profile histos
    tzcanh1->cd();

//  Plot profile histos
    for( int ff=0; ff<nfiles; ff++ ) {
//  Fit a gaussian if desired  
      if( dateplot[ff]->fit[ii] ) dateplot[ff]->histlistp[ii]->Fit("gaus");
      //      dateplot[ff]->histlistp[ii]->SetAxisRange(0,ymax,"Y");
      dateplot[ff]->histlistp[ii]->GetYaxis()->SetTitle(ytitle);
      if( ititle == 0 ) {
	//	dateplot[ff]->histlistp[ii]->SetOptStats(1110);
	gStyle->SetOptStat(1110);
	dateplot[ff]->histlistp[ii]->SetBit(TH1::kNoTitle);   //suppress the histogram title
      }
      if( ff == 0 ) dateplot[ff]->histlistp[ii]->Draw("");
      else          dateplot[ff]->histlistp[ii]->Draw("sames");
// Save histogram to file    
      dateplot[ff]->histlistp[ii]->Write();
    }
    tzcanh1->Update();

//  Change color of stats box to match line color of corresponding histogram
    TPaveStats *p1;
    float frac = 0.3;
    if( nfiles > 2 ) frac = 0.6/((float)nfiles);
    for( int ff=0; ff<nfiles; ff++ ) {
      p1 = (TPaveStats*)dateplot[ff]->histlistp[ii]->GetListOfFunctions()->FindObject("stats");
      dateplot[ff]->histlistp[ii]->GetListOfFunctions()->Remove(p1);
      dateplot[ff]->histlistp[ii]->SetStats(0);
      p1->SetX1NDC(0.75);
      p1->SetX2NDC(1.0);
      p1->SetY1NDC(1.0-frac*ff-frac);
      p1->SetY2NDC(1.0-frac*ff);
      p1->SetTextColor(dateplot[ff]->histlistp[ii]->GetLineColor());
      //	p1->SetName(dateplot[ff]->fname);
      p1->Draw();
    }
    tzcanh1->Print(Form("%s_%s_pr.png",fname,dateplot[0]->items[ii]));

//  Make data vs day plots
//  Plot background histogram
    tzcan->cd();
    Background( tzcan, dateplot[0]->bckgd, dateplot[0]->bckh2[ii], 18 ); 
//  Plot data
    for( int ff=0; ff<nfiles; ff++ ) {
      dateplot[ff]->grlist[ii]->Draw("P");
    }
    if( nfiles > 1 && ilegend ) legend->Draw();
    tzcan->Print(Form("%s_%s.png",fname,dateplot[0]->items[ii]));

  }     //end loop over items

}       //end dateplotter()
