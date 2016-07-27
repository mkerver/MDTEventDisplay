#ifndef edplot_h
#define edplot_h
/*****************************************************
*   Histogram Plotting routines for ROOT
*     Edward Diehl 25-Mar-2009 
*****************************************************/
#include <vector>
#include <stdio.h>
#include <TH1.h>   
#include <TH2.h>   
#include <THStack.h>   
#include <TProfile.h>   
#include <TCanvas.h>   
#include <TPad.h>   
#include <TList.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TString.h>
#include <TPaveStats.h>   
#include <TPaveText.h>
#include <TGaxis.h>
/*****************************************************
* List of routines
*****************************************************/
//  Books histogram and adds it a TList or not depending on version
TH1F *make_h1( TList *l, const char *name, const char *title, 
               Int_t nx, Double_t x1, Double_t x2, 
               const char *xtit="", const char *ytit="");
TH1F *make_h1( const char *name, const char *title, 
               Int_t nx, Double_t x1, Double_t x2, 
               const char *xtit="", const char *ytit="");
TH2F *make_h2( TList *l, const char *name, const char *title, 
               Int_t nx, Double_t x1, Double_t x2, 
               Int_t ny, Double_t y1, Double_t y2,
               const char *xtit="", const char *ytit="");
TH2F *make_h2( const char *name, const char *title, 
               Int_t nx, Double_t x1, Double_t x2, 
               Int_t ny, Double_t y1, Double_t y2,
               const char *xtit="", const char *ytit="");
TProfile *make_prof( TList *l, const char *name, const char *title, 
               Int_t nx, Double_t x1, Double_t x2, 
               Double_t y1, Double_t y2,
               const char *xtit="", const char *ytit="");
TProfile *make_prof( const char *name, const char *title, 
               Int_t nx, Double_t x1, Double_t x2, 
               Double_t y1, Double_t y2,
               const char *xtit="", const char *ytit="");
//  Print a plot to a PNG file
void PrintPlot( TH1 *h, const char* gstat="emr", const char *opt="", const Int_t icolor=1, const Int_t ifill=0);
//  Normalize a histogram (to 100%) 
void PercentagePlot( TH1F *h, TH1F *ph, Int_t iflag=0 );
//  Plot multiple histograms on same plot 
void MultiPlot( int nplots, TH1F *h[], int inorm=0, const char *title="" );
//  Plot 2 histograms on same plot with scale of 2nd histo on right
void TwoPlot( TH1 *h[], int inorm=0, const char *title="" );
//  Print histogram summary
void PrintSummary( TH1F *h, int iopt=0 );
//  Add bin labels to histogram.
void AddHistBinLabels( TH1 *h, const char *labels[], int nlab=0 );
void AddHistBinLabels( TH2 *h, const char *labels[], int nlab=0 );
//  Print bin contents with bin labels.
void PrintHistBins( TH1 *h, int iopt=0 );
// Utility routine to set axis parameters
void ConfigureAxis( TAxis *axis, char *atitle = NULL, 
                    double tsize = -1., double toffset = -1., 
                    double lsize = -1., double loffset = -1. );
//  Draw a background histogram to overlay another histogram on
int Background( TCanvas *can, TH1F *hbck, TH2F *hxy=NULL, 
		int icolor=18, int titlemask=1 );
// Check that a histogram exists before writing (to open ROOT file)
void safeWrite( TH1* h1, const int check_entries=1 );
int ROOTcolors[] = {2,5,3,4,6,7,8,9,1};   //my favorite ROOT colors

//*******************************************************************
//  Class to plot histograms with a colored background showing different regions
//  on which to plot something else like a TGraph of points.  Create an instance of
//  the class and then call methods DrawHis, DrawXY, DrawStack to draw plots
//  superimposed on a striped colored background.
//  DrawHis() => Draw a histogram with colored background.
//  DrawXY()  => Draw an XY scale with colored background for use with plotting TGraph, etc
//  DrawStack() => Draw a THStack of histograms on colored background
//  DrawHists() => Draw several histograms on colored background (overlaid, not stacked)
//  xbins   = Number of x bins
//  xedges  = array of bin edges
//  xlabels = labels for bins
//  icolor  = color of background stripes  
//
//  DrawHis can be called with a histogram pointer OR a histogram name 
//  in which case the histogram is read from the current directory
// 
//*******************************************************************
class BGHist {
 public:
  BGHist( const Int_t xbins, const Double_t *xedges, const char **xlabels, const int icolor=18 );
  ~BGHist();
  void DrawHis(  TH1 *hplot, Double_t ymin=0., Double_t ymax=0., TCanvas *can=NULL, const char *fname="", int logy=0);
  void DrawHis( char *hname, Double_t ymin=0., Double_t ymax=0., TCanvas *can=NULL, const char *fname="", int logy=0);
  void DrawXY( const Double_t ymin, const Double_t ymax, const char *title="", const char *xlab="", const char *ylab="", int logy=0 );
  void DrawStack( const int nhist, TH1F **hlist, const char **labels, const char *htitle, TCanvas *can, const char *fname, int logy=0, const Double_t **lpos=NULL);
  void DrawHists( const int nhist, TH1F **hlist, const char **labels, const char *htitle, TCanvas *can, const char *fname, int logy=0, const Double_t **lpos=NULL );

 private:
  void DrawHis_( TH1 *hplot, Double_t ymin, Double_t ymax, TCanvas *can, const char *fname, int logy);
  TH1F *m_hbck;             //Histogram with striped background of regions
  const Int_t m_xbins;      //Number of bins of striped regions
  const Double_t *m_xedges; //bin edges of striped regions
  const char **m_xlabels;   //bin labels for regions
  const int m_icolor;       //color of background stripes 
  int m_np;                 //number of plots
//  Lists for TH2F histos for bin labels and XY scale and transparent TPads
  std::vector<TH2F*> m_hxy;
  std::vector<TPad*> m_overlay;
};

//#############################################################################
// Create and optionally draw stacked histograms 
// hlist  = list of histograms to stack
// labels = list of labels for legend.  If NULL then do not make legend.
// htitle = Title for stacked histogram
// can    = Canvas, if supplied canvas stack is plotted to canvas
// fname  = File name to print canvas to, if supplied
// lpos   = pointer to array with [x1,y1,x2,y2] position for legend
//#############################################################################
class HStack {
 public:
  HStack( const int nhist, TH1F **hlist, const char **labels=NULL, const char *htitle="", TCanvas *can=NULL, const char *fname="", const Double_t *lpos[]=NULL);
  THStack *stack;
  TLegend *legend;
};

/*****************************************************
* Routine to create 1D/2D histograms and add it to list
* (or not add to a list if no list supplied.
* Returns a pointer to the histogram
*****************************************************/
TH1F *make_h1( TList *l, const char *name, const char *title, 
	       Int_t nx, Double_t x1, Double_t x2,
               const char *xtit, const char *ytit ) {
  TH1F *h = new TH1F(name, title, nx, x1, x2);
  if( xtit[0] != '\0' ) h->GetXaxis()->SetTitle(xtit);
  if( ytit[0] != '\0' ) h->GetYaxis()->SetTitle(ytit);
  l->Add(h);
  return h;
}
TH1F *make_h1( const char *name, const char *title, 
	       Int_t nx, Double_t x1, Double_t x2,
               const char *xtit, const char *ytit ) {
  TH1F *h = new TH1F(name, title, nx, x1, x2);
  if( xtit[0] != '\0' ) h->GetXaxis()->SetTitle(xtit);
  if( ytit[0] != '\0' ) h->GetYaxis()->SetTitle(ytit);
  return h;
}
TH2F *make_h2( TList *l, const char *name, const char *title, 
              Int_t nx, Double_t x1, Double_t x2, 
              Int_t ny, Double_t y1, Double_t y2,
              const char *xtit, const char *ytit ) {
  TH2F *h = new TH2F(name, title, nx, x1, x2, ny, y1, y2);
  if( xtit[0] != '\0' ) h->GetXaxis()->SetTitle(xtit);
  if( ytit[0] != '\0' ) h->GetYaxis()->SetTitle(ytit);
  l->Add(h);
  return h;
}
TH2F *make_h2( const char *name, const char *title, 
              Int_t nx, Double_t x1, Double_t x2, 
              Int_t ny, Double_t y1, Double_t y2,
              const char *xtit, const char *ytit ) {
  TH2F *h = new TH2F(name, title, nx, x1, x2, ny, y1, y2);
  if( xtit[0] != '\0' ) h->GetXaxis()->SetTitle(xtit);
  if( ytit[0] != '\0' ) h->GetYaxis()->SetTitle(ytit);
  return h;
}
TProfile *make_prof( TList *l, const char *name, const char *title, 
              Int_t nx, Double_t x1, Double_t x2, 
              Double_t y1, Double_t y2,
              const char *xtit, const char *ytit ) {
  TProfile *h = new TProfile(name, title, nx, x1, x2, y1, y2);
  if( xtit[0] != '\0' ) h->GetXaxis()->SetTitle(xtit);
  if( ytit[0] != '\0' ) h->GetYaxis()->SetTitle(ytit);
  l->Add(h);
  return h;
}
TProfile *make_prof( const char *name, const char *title, 
              Int_t nx, Double_t x1, Double_t x2, 
              Double_t y1, Double_t y2,
              const char *xtit, const char *ytit ) {
  TProfile *h = new TProfile(name, title, nx, x1, x2, y1, y2);
  if( xtit[0] != '\0' ) h->GetXaxis()->SetTitle(xtit);
  if( ytit[0] != '\0' ) h->GetYaxis()->SetTitle(ytit);
  return h;
}

/*****************************************************
*  Print histogram details for 1 histogram
*****************************************************/
void PrintSummary( TH1F *h, int iopt ) {
  if( iopt == 0 ) {
    printf("  Histo %-21s Entries %-6.0lf Mean %-15lf RMS %-15lf\n",h->GetName(),
	   h->GetEntries(),h->GetMean(),h->GetRMS());
  } else {
    printf("  %-21s %6.0lf %15lf %15lf\n",h->GetName(),
	   h->GetEntries(),h->GetMean(),h->GetRMS());
  }
}

/********************************************************************
*  Utility routine to set axis parameters
*     axis    = Pointer to axis to be changed
*     atitle  = title for axis
*     tsize   = title size   (-1 => do not change )
*     toffset = title offset (-1 => do not change )
*     lsize   = label size   (-1 => do not change )
*     loffset = label offset (-1 => do not change )
*********************************************************************/
void ConfigureAxis( TAxis *axis, char *atitle, double tsize, 
                    double toffset, double lsize, double loffset ) {
  if( !axis ) return;
  if( atitle != NULL ) axis->SetTitle(atitle);
  if( tsize   != -1. ) axis->SetTitleSize(tsize);
  if( toffset != -1. ) axis->SetTitleOffset(toffset);
  if( lsize   != -1. ) axis->SetLabelSize(lsize);
  if( loffset != -1. ) axis->SetLabelOffset(loffset);
}
/********************************************************************
// Print a histogram in a png file
// Output file name is histogram name + ".png"
// Resizes stats box and Histogram title (unless "x" option choose)
// gstat = char flag for SetOptStats 
//         x=SPECIAL Flag if first character.  
//           do not resize statsbox and histogram title
//         n=name e=entries m=mean M=mean+error r=RMS R=RMS+error 
//         u=underflows o=overflows i=integral of bins
// icolor = color for histogram line and/or fill
// ifill  = fill pattern
********************************************************************/
void PrintPlot( TH1 *h, const char* gstat, const char *opt, const Int_t icolor, const Int_t ifill) {
//  Do not set default title sizes if first char of gstat is x 
  /* if( gstat[0] != 'x' ) { */
  /*   gStyle->SetOptStat(gstat); */
  /*   gStyle->SetStatH(0.25); */
  /*   gStyle->SetTitleH(0.09); */
  /*   gPad->SetTicks(0,1); */
  /* } else if( gstat[1] != '\0' ) { */
  /*   gStyle->SetOptStat(&gstat[1]); */
  /* } */
  if( icolor ) {
    h->SetLineColor(icolor);
    h->SetLineWidth(2);
    h->SetFillColor(icolor);
    h->SetFillStyle(ifill);
  }
  gStyle->SetOptStat(gstat);
  h->Draw(opt);
  gPad->Print(Form("%s.png",h->GetName()));
}    //End PrintPlot

/********************************************************************
//  Convert histogram (in TH1F *h) to percentage (in TH1F *ph)
//  iflag = 1 => Normalize to first bin of the histogram
//       <> 1 => Normalize to number of entries
********************************************************************/
void PercentagePlot( TH1F *h, TH1F *ph, Int_t iflag ) {
  Double_t norm=1.;
  if( iflag == 1 ) {
    norm = h->GetBinContent(1);
  } else {
    norm = h->GetEntries();
  }   
  if( norm == 0. ) norm = 1;
  Int_t nbins = h->GetNbinsX();
  for( Int_t i=0; i<nbins; i++ ) {
    Double_t x = h->GetBinContent(i+1)/norm*100.;
    ph->SetBinContent(i+1,x); 
  }
}   //End PercentagePlot
/********************************************************************
//  Plot multiple histograms on same plot 
//  h = array of histograms to plot.  Scale of first is used for all.
//  inorm = 0 => Do not scale histograms
//          1 => Scale historgrams to number of entries in first one
********************************************************************/
void MultiPlot( int nplots, TH1F *h[], int inorm, const char *title ) {
  gStyle->SetOptStat(0);
  gPad->SetTicks(0,1);

// Make a legend in our own time using names of histograms
  TLegend *legend = new TLegend( 0.84, 0.99-nplots*0.08, 0.99, 0.99 );
  for( int i=0; i<nplots; i++ ) {
    h[i]->SetLineColor(i+1);
    h[i]->SetLineWidth(2);
    h[i]->SetFillColor(i+1);
    h[i]->SetFillStyle(i+3004);
    legend->AddEntry( h[i], h[i]->GetName() );
  }

//  Draw the histograms, scaling if needed
  h[0]->Draw();
  for( int i=1; i<nplots; i++ ) {
    if( inorm == 1 && h[i]->GetEntries()>0) {
      h[i]->Scale(h[0]->GetEntries()/h[i]->GetEntries());
    }
    h[i]->Draw("SAME");
  }
  legend->Draw();

//  Make a title if supplied
  if( title[0] != '\0' ) {
    h[0]->SetBit(TH1::kNoTitle);   //suppress the histogram title
    TPaveText *titlept = new TPaveText(0.02,0.92,0.68,0.99,"NDC");
    titlept->AddText(title);                        
    titlept->Draw();     
  }

//  Print to a file
  gPad->Print(Form("%s_MP.png",h[0]->GetName()));
  h[0]->ResetBit(TH1::kNoTitle);   //enable the histogram title
}    //End MultiPlot
/********************************************************************
//  Plot Two histograms on same plot 
//  h = array of histograms to plot.  
//  scale of 2nd histogram is printed on the right
//  inorm = 0 => Do not scale histograms
//          1 => Scale historgrams to number of entries in first one
//          2 => Scale histograms to max of first.
********************************************************************/
void TwoPlot( TH1 *h[], int inorm, const char *title ) {
  //  printf("TowPlot inorm = %i \n",inorm);
  //  printf("TowPlot segfault now histo %s\n",h[0]->GetName());
  gStyle->SetOptStat(0);
  gPad->SetTicks(0,0);

  if( h[0] == 0 ) {
    printf("ERROR null pointer to h[0]\n");
    return;
  }
  h[0]->Draw();

  gPad->Update();   //I don't know what this does but if you don't do it the axis will not work
  Float_t rightmax = 1.1*h[0]->GetMaximum();
  if( inorm == 1 && h[1]->GetEntries()>0) {
    printf("inorm = 1\n");
    rightmax *= h[0]->GetEntries()/h[1]->GetEntries();
    h[1]->Scale(h[0]->GetEntries()/h[1]->GetEntries());
  } else if (inorm == 2 && h[1]->GetMaximum()>0) {
    printf("inorm = 2\n");
    rightmax = 1.1*h[1]->GetMaximum();
    h[1]->Scale(gPad->GetUymax()/rightmax);
  }
//draw an axis on the right side
  h[1]->Draw("SAME");
//  Using NDC coordinates, as per ROOT manual example, but which does not work
  TGaxis *axis = new TGaxis(gPad->GetUxmax(),gPad->GetUymin(),gPad->GetUxmax(),
			    gPad->GetUymax(),0,rightmax,510,"NDC+L");
  axis->Draw();

//  Make a title from the titles of the 2 histograms else use the one supplied
  h[0]->SetBit(TH1::kNoTitle);   //suppress the original histogram title
  TPaveText *titlept = new TPaveText(0.02,0.92,0.75,0.99,"NDC");
  if( title[0] != '\0' ) {
    titlept->AddText(title);                        
  } else {
    titlept->AddText(Form("%s (L) & %s (R)",h[0]->GetTitle(),h[1]->GetTitle()));
  }
  titlept->Draw();     

//  Print to a file
  gPad->Print(Form("%s_%s_TP.png",h[0]->GetName(),h[1]->GetName()));
  h[0]->ResetBit(TH1::kNoTitle);   //enable the histogram title
}     //End TwoPlot
/********************************************************************
//  Print out bin contents of histograms using axis labels.
//  Mainly this is useful for histogram where each bin is labelled.
//  If a second historgram is supplied its contents are written in 
//  another column
//   iopt=0  Print histogram bin label + contents
//   iopt=1  Print histogram bin label + contents + % of first column
********************************************************************/
void PrintHistBins( TH1 *h, int iopt ) {
  double fnorm=1.;
  h->LabelsDeflate();   //Suppress bins with no labels.

  if( iopt == 0 ) {
    printf("                   %12s\n",h->GetName());
  }  else if ( iopt == 1 ) {
    fnorm = h->GetBinContent(1);
    printf("                   %12s            %%\n",h->GetName());
  }

//  Histogram normalization factor
  if( fnorm == 0.) fnorm=1.;

//  Loop over bins printing contents
//  Note bin0= underflow
  for( int i=1; i<=h->GetNbinsX(); i++ ) {
    if( iopt == 0 ) {
      printf("%-18s %12.2lf\n",h->GetXaxis()->GetBinLabel(i),
	     h->GetBinContent(i));
    } else if ( iopt == 1 ) {
      printf("%-18s %12.0lf %14.4lf\n",h->GetXaxis()->GetBinLabel(i),
	     h->GetBinContent(i),100.*h->GetBinContent(i)/fnorm);
    }
  }
}   //End PrintHistBins

/********************************************************************
//  Add bin labels to histogram.
//  IMPORTANT: Include an empty label at the end to signal end of labels
//  If encounter an empty label stop and suppress bins with no labels
//  or specify number of labels with nlab.
********************************************************************/
void AddHistBinLabels( TH1 *h, const char *labels[], int nlab ) {
  int labmax = ( nlab==0 ) ? h->GetNbinsX() : nlab;
  for( int i=0; i<labmax; i++ ) {
    if( !nlab && labels[i][0] == '\0' ) break;
    h->GetXaxis()->SetBinLabel(i+1,labels[i]);
  }
  if( !nlab ) h->LabelsDeflate();   //Suppress bins with no labels.
}   //End AddHistBinLabels
/********************************************************************
//  Add bin labels to histogram.
//  If encounter an empty label stop and suppress bins with no labels
*******************************************************************/
void AddHistBinLabels( TH2 *h, const char *labels[], int nlab ) {
  int labmax = ( nlab==0 ) ? h->GetNbinsX() : nlab;
  for( int i=0; i<labmax; i++ ) {
    if( !nlab && labels[i][0] == '\0' ) break;
    h->GetXaxis()->SetBinLabel(i+1,labels[i]);
  }
  if( !nlab ) h->LabelsDeflate();   //Suppress bins with no labels.
}   //End AddHistBinLabels

/********************************************************************/
//  Plot a background histogram which is used to provide a colored background
//  on which to plot something else like a TGraph of points.  Call this routine
//  first, then draw the foreground histogram with the "same" option to plot on top of all this
// *can      => Canvas for to draw on
// *hbck     => background histogram
// *hxy      => Optional, TH2F to set XY scale for forground plot (e.g. when plotting TGraph)
// icolor    => color index for background
// titlemask => 1/0 draw/don't draw hxy histogram title
/********************************************************************/
int Background( TCanvas *can, TH1F *hbck, TH2F *hxy, int icolor, int titlemask ) {

// Assume background histogram is booked, but not filled yet.
// Configure it to have alternating colored bands by bin
// User books it so as to pick number & size of bins.
  hbck->SetStats(0);
  hbck->SetFillColor(icolor);
  hbck->SetLineColor(icolor);
  int ibmax = hbck->GetNbinsX()+2;
  for( int ib=2; ib<ibmax; ib+=2 ) {
    hbck->SetBinContent(ib, 1.0);
  }
// Set max Y range to create alternating colored/uncolored bands
  hbck->SetMaximum(1.);     
  hbck->SetBit(TH2::kNoTitle);

//  Create normal pad for background histogram
  TPad *pad = new TPad("pad","",0,0,1,1);
  pad->Draw();
  pad->cd();   
//plot background histo
  hbck->Draw("AH");   

//  Create transparent pad for foreground histograms
//  Foreground histo should be Drawn after this routine
//  They go into this transparent pad
  can->cd();
  TPad *overlay = new TPad("overlay","",0,0,1,1);
  overlay->SetFillStyle(4000);
  overlay->SetFillColor(0);
  overlay->SetFrameFillStyle(4000);
  overlay->SetGrid(0,1);   //grid only on Y axis
  overlay->Draw();
  overlay->cd();

//  Optionally plot an 2D histogram to set an XY scale for subsequent plots.
//  This used, for example when plotting a TGraph which needs a scale.
//  If non-null, plot XY scale histogram
  if( hxy == NULL ) return -1;

  if( titlemask == 0 ) hxy->SetBit(TH2::kNoTitle);
  hxy->Draw();

  return 0;
}

HStack::HStack(const int nhist, TH1F **hlist, const char **labels, const char *htitle, TCanvas *can, const char *fname, const Double_t **lpos) : legend(NULL) {
  stack  = new THStack("stack",htitle);

//  Set fill colors
  for( int i=0; i<nhist; i++ ) {
    hlist[i]->SetFillColor(ROOTcolors[i]);
    hlist[i]->SetLineColor(ROOTcolors[i]);
    stack->Add(hlist[i]);
  }

//  Set fill colors and fill stack
  for( int i=0; i<nhist; i++ ) {
    hlist[i]->SetFillColor(ROOTcolors[i]);
    hlist[i]->SetLineColor(ROOTcolors[i]);
    stack->Add(hlist[i]);
  }

// Add labels to legend if doing that
  if( labels != NULL ) {
    if( lpos == NULL ) legend = new TLegend(0.9, 0.5, 0.99, 0.9);
    else               legend = new TLegend((*lpos)[0],(*lpos)[1],(*lpos)[2],(*lpos)[3]);
    for( int i=0; i<nhist; i++ ) {
      legend->AddEntry(hlist[i],labels[i]);
    }
  }

//  Optionally print to canvas and save to file
  if( can != NULL ) {
    stack->Draw();
    if( legend ) legend->Draw();
    if( fname[0] != '\0' ) can->Print(Form("%s.png",fname));
  }
  return;
}    //end HSTtack::HSTtack

BGHist::BGHist( const Int_t xbins, const Double_t *xedges, const char **xlabels, const int icolor ):
                m_xbins(xbins), m_icolor(icolor), m_np(0) {
  m_xlabels = xlabels;
  m_xedges  = xedges;
//  Histogram to provide the striped background of regions
  m_hbck = new TH1F("hbck","",m_xbins,m_xedges);
  m_hbck->SetStats(0);
  m_hbck->SetFillColor(m_icolor);
  m_hbck->SetLineColor(m_icolor);
  int ibmax = m_hbck->GetNbinsX() + 2;
  for( int ib=2; ib<ibmax; ib+=2) m_hbck->SetBinContent(ib, 1.0);
  m_hbck->SetMaximum(1.);
}  //end BGHist::BGHist

// Seems not to be really be needed, but does get called
BGHist::~BGHist() {
  //  m_overlay->back()->Delete();
}

//*******************************************************************
//  Draw a 1D or 2D histogram over a background
//  hplot => Histogram to be plotted
//           Title and x, y axis units taken from hplot
//  ymin  => minimum Y value
//  ymax  => maximum Y value
//           If ymin=ymax=0 then limits of hplot are used
//  can   => TCanvas assumed to be open.  If supplied
//           hplot is plotted on background and image
//           file "hplot.GetName()".png is created
//  fname => Name for PNG file. default: hplot.GetName()
//  logy  => if =1 use log scale on Y axis [default=0]
//*******************************************************************
void BGHist::DrawHis( TH1 *hplot, Double_t ymin, Double_t ymax, TCanvas *can, const char *fname, int logy) {

  DrawHis_( hplot, ymin, ymax, can, fname, logy);
}     //end BGHist::DrawHis version 1
void BGHist::DrawHis( char *hname, Double_t ymin, Double_t ymax, TCanvas *can, const char *fname, int logy) {

  TH1 *hplot = (TH1 *) gDirectory->Get(hname);
  if( hplot == NULL ) {
    printf("BGHist::DrawHis ERROR: no histogram %s\n",hname); 
    return;
  }
  DrawHis_( hplot, ymin, ymax, can, fname, logy);
}     //end BGHist::DrawHis version 1

// This is the version that really does the work.
void BGHist::DrawHis_( TH1 *hplot, Double_t ymin, Double_t ymax, TCanvas *can, const char *fname, int logy) {

  if( can != NULL ) can->cd();
  
//  If ymin,ymax not supplied use the one with hplot
  if( ymin == 0. and ymax == 0. ) {
    ymin = hplot->GetMinimum();
    ymax = hplot->GetMaximum();
//  Check that y range is nonzero
    if( ymin == ymax ) {
      printf("WARNING: BGHist.DrawHis: %s Y range ZERO, skipping\n",hplot->GetName());
      return; 
    }
  }
//  Draw colored background to plot histogram on
  DrawXY(ymin,ymax,hplot->GetTitle(),hplot->GetXaxis()->GetTitle(),hplot->GetYaxis()->GetTitle(),logy);

// draw hplot 
  hplot->Draw("same");
  if( can != NULL ) { 
    if( fname[0] == '\0' ) can->Print(Form("%s.png",hplot->GetName()));
    else                   can->Print(Form("%s.png",fname));
  }
}     //end BGHist::DrawHis_

//#########################################################     
//  Draw an xy axis over a background
//  Histograms(s) or TGraphs can then be plotted on this xy axis
//  ymin  => minimum y value 
//  ymax  => maximum y value
//  title => title for plot 
//  xlab  => x axis label for plot
//  ylab  => y axis label for plot
//  logy  => 1 for log scale
//########################################################
void BGHist::DrawXY( const Double_t ymin, const Double_t ymax, const char *title, const char *xlab, const char *ylab, int logy ) {
  //  Overlay(ymin,ymax,title,xlab,ylab);
  //  Overlay(hplot->GetTitle(),ymin,ymax,hplot->GetXaxis()->GetTitle(),hplot->GetYaxis()->GetTitle(),logy);

//plot background histo
  m_hbck->Draw("AH");

//  Create transparent pad for foreground histograms
//  Foreground histo should be Drawn after this routine
//  They go into this transparent pad
  TPad *tpad = new TPad(Form("overlay%i",m_np),"",0,0,1,1);
  m_overlay.push_back(tpad);
  m_overlay.back()->SetFillStyle(4000);
  m_overlay.back()->SetFillColor(0);
  m_overlay.back()->SetFrameFillStyle(4000);
  m_overlay.back()->SetGrid(0,1);   //grid only on Y axis
  m_overlay.back()->SetLogy(logy);
  m_overlay.back()->Draw();
  m_overlay.back()->cd();
  m_np++;

  TH2F *hxy = new TH2F(Form("hxy%i",m_np),title,m_xbins,m_xedges,100,ymin,ymax);
  m_hxy.push_back(hxy);
  AddHistBinLabels(m_hxy.back(),m_xlabels,m_xbins);
  TAxis *xaxis = m_hxy.back()->GetXaxis();
  TAxis *yaxis = m_hxy.back()->GetYaxis();
  xaxis->SetTitle( xlab );
  xaxis->SetTickLength(0);
  xaxis->SetLabelSize(0.075);
  yaxis->SetTitle( ylab );
  yaxis->SetTitleSize(0.06);
  yaxis->SetTitleOffset(0.45);
  m_hxy.back()->Draw();
}     //end BGHist::DrawXY

//#########################################################     
//  Draw a stack of 1D or 2D histogram over a background
//  nhist   => number of histograms in hislist
//  hislist => List of Histograms to be stacked
//           Title and x, y axis units taken from hplot
//  can   => TCanvas assumed to be open.  If supplied
//           hplot is plotted on background and image
//           file "hplot.GetName()".png is created
//  fname => Name for PNG file. default: hplot.GetName()
//  logy  => log/linear scale (1/0), default=0 linear
//  lpos  =>
//#########################################################
void BGHist::DrawStack( const int nhist, TH1F *hlist[], const char **labels, const char *htitle, TCanvas *can, const char *fname, int logy, const Double_t *lpos[] ) {
  TCanvas *c0 = NULL;
  
 //  Create the stack
  HStack *hstack = new HStack(nhist,hlist,labels,htitle,c0,fname,lpos);
  Double_t ymin = hstack->stack->GetMinimum();
  Double_t ymax = hstack->stack->GetMaximum()*1.05;

 //  Draw colored background histogram
  DrawXY(ymin,ymax,htitle,hlist[0]->GetXaxis()->GetTitle(),hlist[0]->GetYaxis()->GetTitle(),logy);

 // Draw hplot 
  hstack->stack->Draw("same");
  if( hstack->legend != NULL ) hstack->legend->Draw();
  if( can != NULL ) { 
    if( fname[0] == '\0' ) can->Print(Form("%s.png",hstack->stack->GetName()));
    else                   can->Print(Form("%s.png",fname));
  }

}     //end BGHist::DrawStack
//#########################################################     
//  Draw a several histograms on one plot, using scale of first one (i.e. not stacked)
//  hlist => List of Histograms to be stacked
//           Title and x, y axis units taken from hplot
//  can   => TCanvas assumed to be open.  If supplied
//           hplot is plotted on background and image
//           file "hplot.GetName()".png is created
//  fname => Name for PNG file. default: hplot.GetName()
//  logy  => log/linear scale (1/0), default=0 linear
//#########################################################
void BGHist::DrawHists( const int nhist, TH1F **hlist, const char **labels, const char *htitle, TCanvas *can, const char *fname, int logy, const Double_t **lpos ) {
//  Use the scale of the first histogram
  Double_t ymin = -999999.;
  Double_t ymax = 0.;

// Check that histogram pointers are valid and set scale with the first good one
  for( int i=0; i<nhist; i++ ) {
    hlist[i]->SetLineColor(ROOTcolors[i]);
    hlist[i]->SetLineWidth(2);
    hlist[i]->SetFillColor(ROOTcolors[i]);
    hlist[i]->SetFillStyle(1001);
    if( ymin == -999999. ) {
      ymin = hlist[i]->GetMinimum();
      ymax = hlist[i]->GetMaximum()*1.05;
    }
  }

//  Draw colored background histogram
  DrawXY(ymin,ymax,htitle,hlist[0]->GetXaxis()->GetTitle(),hlist[0]->GetYaxis()->GetTitle(),logy);

// Draw histograms 
  for( int i=0; i<nhist; i++ ) {
    hlist[i]->Draw("same");
  }

// Make the legend 
  TLegend *legend = NULL;
  if( labels != NULL ) {
    if( lpos == NULL ) legend = new TLegend( 0.84, 0.99-nhist*0.08, 0.99, 0.99 );
    else               legend = new TLegend((*lpos)[0],(*lpos)[1],(*lpos)[2],(*lpos)[3]);
    for( int i=0; i<nhist; i++ ) {
      legend->AddEntry(hlist[i],labels[i]);
    }
    legend->Draw();
  }

  if( can != NULL ) { 
    if( fname[0] == '\0' ) can->Print(Form("%s.png",hlist[0]->GetName()));
    else                   can->Print(Form("%s.png",fname));
  }
}     //end BGHist::DrawHists

// Check that a histogram exists before writing
void safeWrite( TH1* h1, const int check_entries ) {
  if( h1 == 0 ) return;
  if( check_entries && h1->GetEntries() == 0 ) return;
  h1->Write();
}
#endif
