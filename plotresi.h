#include <fstream>
#include <stdio.h>
#include <TStyle.h>
#include <TDirectory.h>
#include <TPad.h>
#include <TH1.h>
#include <TMath.h>
#include <TPaveText.h>
#include <TF1.h>
#include "edplot.h"
#include "chamberwheel.h"
#include "chamberlist.h"

// These can be changed by the program including resiplot.h 
const char chend[4]="png";
// These are prepended to file and plot names, respectively
char filename[255],plotname[255];

// Size of values[] array
const int VALUESMAX=20;     //maximum number of items returned by DrawGaus, DrawResi

int WriteResiHeader( std::ofstream &fout );
int WriteT0FitHeader( std::ofstream &fout );
int WriteADCHeader( std::ofstream &fout );
int DrawGaus( TH1 *hp,           Double_t values[VALUESMAX], const char *fname="" );
int DrawGaus( const char *hname, Double_t values[VALUESMAX], const char *fname="" );
int DrawGaus_( TH1 *hp,          Double_t values[VALUESMAX], const char *fname="" );
// Original version with double gaussian fit (independent gaussians)
int DrawResi(TH1 *hp,           Double_t values[VALUESMAX], const Double_t *params, const char *fname="" );
int DrawResi(const char *hname, Double_t values[VALUESMAX], const Double_t *params, const char *fname="" );
int DrawResi_( TH1 *hp,         Double_t values[VALUESMAX], const Double_t *params, const char *fname="" );

// Version for fitting 2 distributions and putting on same plot (e.g. to compare MLs)
int DrawResi(const char *hname1, const char *hname2, Double_t values1[VALUESMAX], Double_t values2[VALUESMAX], const Double_t *params, const char *pname="", const char *fname="" );
int DrawResi(TH1 *hp1, TH1 *hp2,  Double_t values1[VALUESMAX], Double_t values2[VALUESMAX], const Double_t *params, const char *pname="", const char *fname="" );

// This just does the double gaussion fit, does not draw plot
int FitDoubleGaussian(TH1 *hp, Double_t values[VALUESMAX], const Double_t *params);

// Claudio's version which constrains 2 gaussians to have same mean
Double_t DoubleG(Double_t *x,Double_t *par);
Double_t G1     (Double_t *x,Double_t *par);
Double_t G2     (Double_t *x,Double_t *par);
int DrawResi_(TH1 *hp, Double_t values[VALUESMAX], const Double_t *params, const char *fname, int dummy );

// print TH1F histogram to file
void Drawh1( const char *hname, const char *opt="", const char *stat="eMR");
// print 2 TH1F histogram2 overlaid to file
void Drawh1h1(const char *hname1, const char *hname2, Double_t data[6], const char *pname="", const char *fname="", const int iline=0);
void Drawh1h1h1(const char *hname1, const char *hname2, const char *hname3, Double_t data[6], const char *pname );
void Drawh1h1h1_(TH1 *h1, TH1 *h2, const char *hname3, Double_t data[6], const char *pname );
// print TH2F histogram to file
void Drawh2(const char *hname, const char *chopt="", const char *xlab="", const char *ylab="", 
	       const char *chstat="" );
void makebackground(TCanvas *can, int iw, TH1F *h1, TH1F *h2=NULL, Double_t ylo=0., Double_t yhi=0.);

//  Use this when writing T0 fit results using DrawGaus
int WriteT0FitHeader( std::ofstream &fout ) {
// Limits for variables (min,max)
  int T0Fit_Entries[2] = {0,0};
  int T0Fit_Ave[2] = {-60,60};
  int T0Fit_RMS[2] = {0,40};
  int T0Fit_GMean[2] = {-60,60};
  int T0Fit_GSigma[2] = {0,40};
  int T0Fit_Ave_Err[2] = {0,0};
  int T0Fit_GMean_Err[2] = {0,0};
  int T0Fit_GSigma_Err[2] = {0,0};
// Units for variables 
  char cT0Fit_Entries[10] = " x";
  char cT0Fit_Ave[10] = " [ns]";
  char cT0Fit_RMS[10] = " [ns]";  
  char cT0Fit_GMean[10] = " [ns]";
  char cT0Fit_GSigma[10] = " [ns]";
  char cT0Fit_Ave_Err[10] = " x";
  char cT0Fit_GMean_Err[10] = " x";
  char cT0Fit_GSigma_Err[10] = " x";
  char minmax[2][20] = {"min type ","max type "};
  fout << "chamber type Entries Ave RMS Fit_Mean Fit_#sigma Ave_Err Fit_Mean_Err Fit_#sigma_Err\n";
  fout << "units type " << cT0Fit_Entries << cT0Fit_Ave << cT0Fit_RMS << cT0Fit_GMean << cT0Fit_GSigma;
  fout << cT0Fit_Ave_Err << cT0Fit_GMean_Err << cT0Fit_GSigma_Err << "\n";
  for( int j=0; j<2; j++ ) {
    fout << minmax[j]; 
    fout << T0Fit_Entries[j]  << " ";
    fout << T0Fit_Ave[j] << " ";
    fout << T0Fit_RMS[j] << " ";
    fout << T0Fit_GMean[j] << " ";
    fout << T0Fit_GSigma[j] << " ";
    fout << T0Fit_Ave_Err[j] << " ";
    fout << T0Fit_GMean_Err[j] << " ";
    fout << T0Fit_GSigma_Err[j] << "\n";
  }
  return 0;
}   //end WriteT0FitHeader

//  Use this when writing T0 fit results using DrawGaus
int WriteADCHeader( std::ofstream &fout ) {
// Limits for variables (min,max)
  int Raw_ADC_Entries[2] = {0,0};
  int Raw_ADC_Ave[2]     = {30,180};
  int Raw_ADC_RMS[2]     = {0,80};
  int ADC_Entries[2]     = {0,0};
  int ADC_Ave[2]         = {120,180};
  int ADC_RMS[2]         = {20,60};
// Units for variables 
  char cADC_Entries[10] = " x";
  char cADC_Ave[20]     = " [ticks]";
  char cADC_RMS[20]     = " [ticks]";
  char minmax[2][20] = {"min type ","max type "};
  fout << "chamber type Raw_ADC_Entries Raw_ADC_Ave Raw_ADC_RMS Segment_ADC_Entries Segment_ADC_Ave Segment_ADC_RMS Raw_ADC80_Entries Raw_ADC80_Ave Raw_ADC80_RMS\n";
  fout << "units type" << cADC_Entries << cADC_Ave << cADC_RMS << cADC_Entries << cADC_Ave << cADC_RMS << cADC_Entries << cADC_Ave << cADC_RMS << "\n";
  for( int j=0; j<2; j++ ) {
    fout << minmax[j]; 
    fout << Raw_ADC_Entries[j] << " "  << Raw_ADC_Ave[j] << " " << Raw_ADC_RMS[j] << " ";
    fout << ADC_Entries[j] << " "  << ADC_Ave[j] << " " << ADC_RMS[j] << " ";
    fout << ADC_Entries[j] << " "  << ADC_Ave[j] << " " << ADC_RMS[j] << "\n";
  }
  return 0;
}   //end WriteADCHeader

int WriteResiHeader( std::ofstream &fout ) {
// Limits for variables (min,max)
 int Residual_Entries[2] = {0,0};
 int Residual_Ave[2] = {-40,40};
 int Residual_RMS[2] = {0,300};
 int Residual_G1Mean[2] = {-100,100};
 int Residual_G1Sigma[2] = {0,200};
 int Residual_G2Mean[2] = {-100,200};
 int Residual_G2Sigma[2] = {0,800};
 int Ratio_of_Gaussians[2] = {0,10};
 int Residual_Ave_Err[2] = {0,0};
 int Residual_G1Mean_Err[2] = {0,0};
 int Residual_G2Mean_Err[2] = {0,0};
 int Residual_G1Sigma_Err[2] = {0,0};
 int Residual_G2Sigma_Err[2] = {0,0};
 int Residual_G1G2Mean[2] = {-100,100};
 int Residual_G1G2SigmaHM[2] = {0,200};
 int Residual_G1G2Mean_Err[2] = {0,0};
 int Residual_G1G2SigmaHM_Err[2] = {0,0};
// Units for variables 
 char minmax[2][20] = {"min type ","max type "};
 char cResidual_Entries[10] = " x";
 char cResidual_Ave[10] = " [#mum]";
 char cResidual_RMS[10] = " [#mum]";
 char cResidual_G1Mean[10] = " [#mum]";
 char cResidual_G1Sigma[10] = " [#mum]";
 char cResidual_G2Mean[10] = " [#mum]";
 char cResidual_G2Sigma[10] = " [#mum]";
 char cRatio_of_Gaussians[10] = " x";
 char cResidual_Ave_Err[10] = " x";
 char cResidual_G1Mean_Err[10] = " x";
 char cResidual_G2Mean_Err[10] = " x";
 char cResidual_G1Sigma_Err[10] = " x";
 char cResidual_G2Sigma_Err[10] = " x";
 char cResidual_G1G2Mean[10] = " [#mum]";
 char cResidual_G1G2SigmaHM[10] = " [#mum]";
 char cResidual_G1G2Mean_Err[10] = " x";
 char cResidual_G1G2SigmaHM_Err[10] = " x";
 fout << "chamber type Entries Ave RMS";
 fout << " Gaus1_Mean Gaus1_#Sigma Gaus2_Mean Gaus2_#Sigma Ratio_of_Gaus_Areas";
 fout << " Ave_Err Gaus1_Mean_Err Gaus2_Mean_Err Gaus1_#Sigma_Err Gaus2_#Sigma_Err";
 fout << " G1G2_Mean G1G2_SigmaHalfMax G1G2_Mean_Err G1G2_SigmaHalMax_Err\n";
 fout << "units type " << cResidual_Entries << cResidual_Ave << cResidual_RMS;
 fout << cResidual_G1Mean << cResidual_G1Sigma << cResidual_G2Mean << cResidual_G2Sigma <<  cRatio_of_Gaussians;
 fout << cResidual_Ave_Err << cResidual_G1Mean_Err << cResidual_G2Mean_Err << cResidual_G1Sigma_Err << cResidual_G2Sigma_Err;
 fout << cResidual_G1G2Mean << cResidual_G1G2SigmaHM << cResidual_G1G2Mean_Err << cResidual_G1G2SigmaHM_Err;
 fout << "\n";
 for( int j=0; j<2; j++ ) {
   fout << minmax[j]; 
   fout << Residual_Entries[j] << " ";
   fout << Residual_Ave[j] << " ";
   fout << Residual_RMS[j] << " ";
   fout << Residual_G1Mean[j] << " ";
   fout << Residual_G1Sigma[j] << " ";
   fout << Residual_G2Mean[j] << " ";
   fout << Residual_G2Sigma[j] << " ";
   fout << Ratio_of_Gaussians[j] << " ";
   fout << Residual_Ave_Err[j] << " ";
   fout << Residual_G1Mean_Err[j] << " ";
   fout << Residual_G2Mean_Err[j] << " ";
   fout << Residual_G1Sigma_Err[j] << " ";
   fout << Residual_G2Sigma_Err[j] << " ";
   fout << Residual_G1G2Mean[j] << " ";
   fout << Residual_G1G2SigmaHM[j] << " ";
   fout << Residual_G1G2Mean_Err[j] << " ";
   fout << Residual_G1G2SigmaHM_Err[j] << "\n";
 }
 return 0;
}  //end WriteResiHeader

/************************************************************
*  DrawResi => Do double gaussian fit to a residual distribution
*   Parameters:
*     hp     => Pointer to histogram
*     values => array of fit values, see below for definition
*     filename => If non-blank prepend to PNG filename
*     params[0] => scale factor for residuals 
*                 set to 1000 if residual plots are in mm.
*     params[1] = 0 fit full range of plot 
*               > 0 number of RMSs for limit of DrawResi fit
*               < 0 +-ranges of fit [mm]  
*  values[0] = Number of entries
*  values[1] = Histogram mean
*  values[2] = RMS
*  values[3] = Gaussian1 mean   (narrow)
*  values[4] = Gaussian1 sigma  (narrow) 
*  values[5] = Gaussian2 mean   (wide)
*  values[6] = Gaussian2 sigma  (wide)
*  values[7] = Ratio of area of gaussians (narrow/wide)
*  values[8] = Error of mean = RMS/sqrt(entries)
*  values[9] = Gaussian1 mean error  (narrow)
*  values[10] = Gaussian2 mean error  (wide)
*  values[11] = Gaussian1 sigma error  (narrow)
*  values[12] = Gaussian2 sigma error  (wide)
*  values[13] = X value of maximum of combined function G1+G2
*  values[14] = SigmaHM (==FWHM/2.3548 of the combined G1+G2 function)
*  values[15] = X value of maximum of combined function G1+G2 error
*  values[16] = SigmaHM error
************************************************************/
int DrawResi(TH1 *hp, Double_t values[VALUESMAX], const Double_t *params, const char *fname ) {

//  By using TH1 this works for TH1F, TH1D, etc
  if( hp == 0 ) {
    printf("DrawResi ERROR: Null histogram pointer passed\n");
    return -1;
  }
  return DrawResi_( hp, values, params, fname, 0 );  // last 0 only if single mean is used
}

//  Version which takes a histogram name
int DrawResi(const char *hname, Double_t values[VALUESMAX], const Double_t *params, const char *fname ) {
//  By using TH1 this works for TH1F, TH1D, etc
  TH1 *hp = (TH1 *) gDirectory->Get(hname);
  if( hp == 0 ) {
    printf("DrawResi ERROR: No histogram %s\n",hname);
    return -1;
  }
  return DrawResi_( hp, values, params, fname, 0 ); // last 0 only if single mean is used
}

//  Version which actually does the fits sum of two independent gaussian (NO FINAL dummy integer)
int DrawResi_(TH1 *hp, Double_t values[VALUESMAX], const Double_t *params, const char *fname ) {
// Zero values array
  for( int i=0; i<VALUESMAX; i++ ) values[i] = 0.;
//  Do not plot if no entries or RMS==0 (entries are all underflow/overflow)
  if( hp->GetEntries() == 0 || hp->GetRMS() == 0 ) {
    printf("DrawResi WARNING: histogram %s no entries, skipping plot\n",hp->GetName());
    return -1;
  }

  printf("DrawResi: Fitting %s\n",hp->GetName());

// Add filename to title of histogram
  if( fname != '\0' ) {
    char htitle[80] = "",pname[80];
// Remove _ from filename to use for plot names
    int i=0; 
    for( ; fname[i]!='\0'; i++ ) {
      if( fname[i] == '_' ) pname[i] = ' ';
      else                  pname[i] = fname[i];                 
    }
    pname[i]='\0';
    printf("DrawResi filename=X%sX plotname=X%sX\n",fname,pname);
    sprintf(htitle,"%s %s",pname,hp->GetTitle());
    hp->SetTitle(htitle);
  }

  printf("Create TF1\n");
  TF1 *fun3=new TF1("fun3","gaus(0)+gaus(3)"); 
  fun3->SetParNames("Const1","Mean1","Sigma1","Const2","Mean2","Sigma2");
//  Set initial guesses of parameters
  Int_t       i = hp->GetMaximumBin();
  Double_t mean = hp->GetMean();
  Float_t  ampl = (hp->GetBinContent(i-1) + hp->GetBinContent(i) + hp->GetBinContent(i+1))/3.;
  Float_t  rms  = hp->GetRMS();
  fun3->SetParameters(ampl,mean,rms/4,ampl/10.,mean,rms*2);    //constant,mean,sigma

  gStyle->SetOptStat("emr");
  gStyle->SetOptFit(0);
  gStyle->SetStatY(0.9);
  gStyle->SetTitleH(0.1);
  gStyle->SetTitleW(0.5);
  gStyle->SetStatW(0.38);
  gStyle->SetStatH(0.35);

// Set fit limits according to param[1]
  if(      params[1] > 0. ) hp->Fit("fun3","","",mean-params[1]*rms,mean+params[1]*rms);
  else if( params[1] < 0. ) hp->Fit("fun3","","",params[1],-params[1]);
  else                      hp->Fit("fun3");

// Check results of fit.  If failed try again with restricted range
/*   if( fun3->GetParError(1) > 2.*fun3->GetParameter(1)  || */
/*       fun3->GetParError(4) > 2.*fun3->GetParameter(4) ) { */
/*     fun3->SetParameters(ampl,mean,rms/4,ampl/10.,mean,rms*2);    //constant,mean,sigma */
/*     hp->Fit("fun3","","",mean-2*rms,mean+2*rms); */
/*   } */
//  Determine SigmaHM.  Do binary search to find half max points on both sides
  Double_t Xmax = fun3->GetMaximumX(mean-rms,mean+rms);
  Double_t Hmax = fun3->Eval(Xmax)/2;   //GetMaximum(mean-rms,mean+rms);
  Double_t X1 = Xmax-2*rms;
  Double_t X2 = Xmax;
  Double_t Xlo = (X1+X2)/2.;
  for( int ii=0; ii<25; ii++ ) {
    if( fun3->Eval(Xlo) < Hmax ) X1 = Xlo;
    else                         X2 = Xlo;
    Xlo = (X1+X2)/2.;
  }
  X1 = Xmax;
  X2 = Xmax+2*rms;
  Double_t Xhi = (X1+X2)/2.;
  for( int ii=0; ii<25; ii++ ) {
    if( fun3->Eval(Xhi) < Hmax ) X2 = Xhi;
    else                         X1 = Xhi;
    Xhi = (X1+X2)/2.;
  }
  Xmax *= params[0];
  Double_t sigmaHM = (Xhi-Xlo)/2.3548*params[0];
  //  printf("Xmax=%lf Hmax=%lf Xlo=%lf XloF=%lf\n",Xmax,Hmax,Xlo,fun3->Eval(Xlo));
  //  printf("Xhi=%lf XhiF=%lf sigma=%lf\n",Xhi,fun3->Eval(Xhi),sigmaHM);
//  Draw fit results on plot
//  TPaveText *sigpt = new TPaveText(0.1,0.5,0.38,0.9,"NDC");
  TPaveText *sigpt = new TPaveText(0.1,0.2,0.38,0.9,"NDC");
  sigpt->SetTextAlign(12);
  Float_t sig1   = TMath::Abs(fun3->GetParameter(2))*params[0];
  Float_t sig2   = TMath::Abs(fun3->GetParameter(5))*params[0];
  Float_t mean1  = fun3->GetParameter(1)*params[0];
  Float_t mean2  = fun3->GetParameter(4)*params[0];
  Float_t const1 = fun3->GetParameter(0);
  Float_t const2 = fun3->GetParameter(3);
  Float_t ratio  = 0.;
  if( sig1 < sig2 ) {
    sigpt->AddText(Form("#sigma_{1}=%.1lf #mum",sig1));
    sigpt->AddText(Form("#sigma_{2}=%.1lf #mum",sig2));
    sigpt->AddText(Form("#mu_{1}=%.1lf #mum",mean1));
    sigpt->AddText(Form("#mu_{2}=%.1lf #mum",mean2));
    if( sig2 > 0. && const2 > 0. ) {
      ratio = sig1*const1/sig2/const2;
      sigpt->AddText(Form("A_{1}/A_{2}=%.3lf",ratio));
    } 
    values[3] = mean1;
    values[4] = sig1;
    values[5] = mean2;
    values[6] = sig2;
    values[7] = ratio;
    values[9] = fun3->GetParError(1)*params[0];
    values[10] = fun3->GetParError(4)*params[0];
    values[11] = fun3->GetParError(2)*params[0];
    values[12] = fun3->GetParError(5)*params[0];
 } else {
    sigpt->AddText(Form("#sigma_{1}=%.1lf #mum",sig2));
    sigpt->AddText(Form("#sigma_{2}=%.1lf #mum",sig1));
    sigpt->AddText(Form("#mu_{1}=%.1lf #mum",mean2));
    sigpt->AddText(Form("#mu_{2}=%.1lf #mum",mean1));
    if( sig1 > 0. && const1 > 0. ) {
      ratio = sig2*const2/sig1/const1;
      sigpt->AddText(Form("A_{1}/A_{2}=%.3lf",ratio));
    }
    values[3] = mean2;
    values[4] = sig2;
    values[5] = mean1;
    values[6] = sig1;
    values[7] = ratio;
    values[9] = fun3->GetParError(4)*params[0];
    values[10] = fun3->GetParError(1)*params[0];
    values[11] = fun3->GetParError(5)*params[0];
    values[12] = fun3->GetParError(2)*params[0];
  }
  values[0] = hp->GetEntries();
  values[1] = hp->GetMean()*params[0];
  values[2] = hp->GetRMS()*params[0];
  if( values[0] > 0. ) values[8] = values[2]/TMath::Sqrt(values[0]);
  values[13] = Xmax;
  values[14] = sigmaHM;
// Xmax, sigmaHM errors are errors of G1+G2 added in quadrature 
// These are too big.  Just use narrow gaussian errors
//   values[15] = TMath::Sqrt(values[9]*values[9]+values[10]*values[10]);
//   values[16] = TMath::Sqrt(values[11]*values[11]+values[12]*values[12]);
  values[15] = values[9];
  values[16] = values[11];
 
  sigpt->AddText(Form("#mu_{#epsilon}=%.1lf #mum",Xmax));
  sigpt->AddText(Form("#sigma_{#epsilon}=%.1lf #mum",sigmaHM));
  sigpt->Draw();

  if( fname[0] == '\0' ) gPad->Print(Form("%s.png",hp->GetName()));
  else          gPad->Print(Form("%s_%s.png",fname,hp->GetName()));

  return 0;
}   //end DrawResi()

//  Version fiting 2 gaussians constrained to have the same mean
int DrawResi_(TH1 *hp, Double_t values[VALUESMAX], const Double_t *params, const char *fname, int DBG ) {
// Zero values array
  for( int i=0; i<VALUESMAX; i++ ) values[i] = 0.;
//  Do not plot if no entries or RMS==0 (entries are all underflow/overflow)
  if( hp->GetEntries() == 0 || hp->GetRMS() == 0 ) {
    printf("DrawResi WARNING: histogram %s no entries, skipping plot\n",
	   hp->GetName());
    return -1;
  }

  printf("DrawResi(%d): Fitting %s\n",DBG,hp->GetName());

  // Add filename to title of histogram
  if( fname != '\0' ) {
    char htitle[80] = "",pname[80];
// Remove _ from filename to use for plot names
    for( int i=0; fname[i]!='\0'; i++ ) {
      if( fname[i] == '_' ) pname[i] = ' ';
      else                  pname[i] = fname[i];                 
    }
    sprintf(htitle,"%s %s",pname,hp->GetTitle());
    hp->SetTitle(htitle);
  }

  gStyle->SetOptStat("emr");
  gStyle->SetOptFit(0);
  gStyle->SetTitleX(0.55);
  gStyle->SetTitleY(1.05);
  gStyle->SetTitleH(0.2);
  gStyle->SetTitleW(0.75);
  gStyle->SetStatW(0.4);
  gStyle->SetStatH(0.35);
  gStyle->SetStatY(0.9);

//  Set initial guesses of parameters
  Int_t    imax = hp->GetMaximumBin();
  Double_t mean = hp->GetMean();
  Float_t  ampl = (hp->GetBinContent(imax-1)+hp->GetBinContent(imax)+hp->GetBinContent(imax+1))/3.;
  Float_t  rms  = hp->GetRMS();

// Set fit limits according to param[1]
  Double_t xmin=hp->GetXaxis()->GetBinLowEdge(1);
  Double_t xmax=hp->GetXaxis()->GetBinUpEdge( hp->GetXaxis()->GetNbins() );
  if(      params[1] > 0. ){
    xmin=mean-params[1]*rms;
    xmax=mean+params[1]*rms;
  } else if( params[1] < 0. ){
    xmin= params[1];
    xmax=-params[1];
  }

  TF1 *fun3=new TF1("fun3",DoubleG,xmin,xmax,5);
  fun3->SetParNames("Const1","Sigma1","Mean","Const2","Sigma2");
  fun3->SetParameters(ampl,rms/4,mean,ampl/10.,rms*2); // constant,mean,sigma

  hp->Fit("fun3","RQ");

// Check results of fit.  If failed try again with restricted range
/*   if( fun3->GetParError(1) > 2.*fun3->GetParameter(1)  || */
/*       fun3->GetParError(4) > 2.*fun3->GetParameter(4) ) { */
/*     fun3->SetParameters(ampl,mean,rms/4,ampl/10.,mean,rms*2);    //constant,mean,sigma */
/*     hp->Fit("fun3","","",mean-2*rms,mean+2*rms); */
/*   } */
//  Determine SigmaHM.  Do binary search to find half max points on both sides
  Double_t Xmax = fun3->GetMaximumX(mean-rms,mean+rms);
  Double_t Hmax = fun3->Eval(Xmax)/2;   //GetMaximum(mean-rms,mean+rms);
  Double_t X1 = Xmax-2*rms;
  Double_t X2 = Xmax;
  Double_t Xlo = (X1+X2)/2.;
  for( int ii=0; ii<25; ii++ ) {
    if( fun3->Eval(Xlo) < Hmax ) X1 = Xlo;
    else                         X2 = Xlo;
    Xlo = (X1+X2)/2.;
  }
  X1 = Xmax;
  X2 = Xmax+2*rms;
  Double_t Xhi = (X1+X2)/2.;
  for( int ii=0; ii<25; ii++ ) {
    if( fun3->Eval(Xhi) < Hmax ) X2 = Xhi;
    else                         X1 = Xhi;
    Xhi = (X1+X2)/2.;
  }
  Xmax *= params[0];
  Double_t sigmaHM = (Xhi-Xlo)/2.3548*params[0];
  //  printf("Xmax=%lf Hmax=%lf Xlo=%lf XloF=%lf\n",Xmax,Hmax,Xlo,fun3->Eval(Xlo));
  //  printf("Xhi=%lf XhiF=%lf sigma=%lf\n",Xhi,fun3->Eval(Xhi),sigmaHM);
//  Draw fit results on plot
//  TPaveText *sigpt = new TPaveText(0.1,0.5,0.38,0.9,"NDC");
  TPaveText *sigpt = new TPaveText(0.1,0.2,0.38,0.9,"NDC");
  sigpt->SetTextAlign(12);
  Float_t sig1   = TMath::Abs(fun3->GetParameter(1))*params[0];
  Float_t sig2   = TMath::Abs(fun3->GetParameter(4))*params[0];
  Float_t mean1  = fun3->GetParameter(2)*params[0];
  Float_t mean2  = fun3->GetParameter(2)*params[0];
  Float_t const1 = fun3->GetParameter(0);
  Float_t const2 = fun3->GetParameter(3);
  Float_t ratio  = 0.;
  if( sig1 < sig2 ) {
    sigpt->AddText(Form("#sigma_{1}=%.1lf #mum",sig1));
    sigpt->AddText(Form("#sigma_{2}=%.1lf #mum",sig2));
    sigpt->AddText(Form("#mu=%.1lf #mum",mean1));
    if( sig2 > 0. && const2 > 0. ) {
      ratio = sig1*const1/sig2/const2;
      sigpt->AddText(Form("A_{1}/A_{2}=%.3lf",ratio));
    } 
    values[3] = mean1;
    values[4] = sig1;
    values[5] = mean2;
    values[6] = sig2;
    values[7] = ratio;
    values[9]  = fun3->GetParError(2)*params[0];
    values[10] = fun3->GetParError(2)*params[0];
    values[11] = fun3->GetParError(1)*params[0];
    values[12] = fun3->GetParError(4)*params[0];
 } else {
    sigpt->AddText(Form("#sigma_{1}=%.1lf #mum",sig2));
    sigpt->AddText(Form("#sigma_{2}=%.1lf #mum",sig1));
    sigpt->AddText(Form("#mu=%.1lf #mum",mean2));
    if( sig1 > 0. && const1 > 0. ) {
      ratio = sig2*const2/sig1/const1;
      sigpt->AddText(Form("A_{1}/A_{2}=%.3lf",ratio));
    }
    values[3] = mean2;
    values[4] = sig2;
    values[5] = mean1;
    values[6] = sig1;
    values[7] = ratio;
    values[9]  = fun3->GetParError(2)*params[0];
    values[10] = fun3->GetParError(2)*params[0];
    values[11] = fun3->GetParError(4)*params[0];
    values[12] = fun3->GetParError(1)*params[0];
  }
  values[0] = hp->GetEntries();
  values[1] = hp->GetMean()*params[0];
  values[2] = hp->GetRMS()*params[0];
  if( values[0] > 0. ) values[8] = values[2]/TMath::Sqrt(values[0]);
  values[13] = Xmax;
  values[14] = sigmaHM;
  values[15] = values[9];
  values[16] = values[11];

  sigpt->AddText(Form("#sigma_{#epsilon}=%.1lf #mum",sigmaHM));
  sigpt->Draw();

  if( fname[0] == '\0' ) gPad->Print(Form("%s.png",hp->GetName()));
  else                   gPad->Print(Form("%s_%s.png",fname,hp->GetName()));

  return 0;
} //end DrawResi_ ==========================================================

Double_t DoubleG(Double_t *x,Double_t *par) {
  return G1(x,par)+G2(x,&par[2]);
} // DoubleG ================================================================

Double_t G1(Double_t *x,Double_t *par) {
  // 0=peak  1=sigma  2=mean
  Double_t t = (x[0]-par[2])/par[1];
  return par[0]*TMath::Exp(-0.5*t*t);
} // G1 ================================================================

Double_t G2(Double_t *x,Double_t *par) {
  // 0=mean  1=peak  2=sigma  
  Double_t t = (x[0]-par[0])/par[2];
  return par[1]*TMath::Exp(-0.5*t*t);
} // G2 ================================================================

//  Version to do fit 2 plots at once
int DrawResi(const char *hname1, const char *hname2, Double_t values1[VALUESMAX], Double_t values2[VALUESMAX], const Double_t *params, const char *pname, const char *fname ) {
//  By using TH1 this works for TH1F, TH1D, etc
  TH1 *hp1 = (TH1 *) gDirectory->Get(hname1);
  if( hp1 == 0 ) {
    printf("DrawResi ERROR: No histogram %s\n",hname1);
    return -1;
  }
  TH1 *hp2 = (TH1 *) gDirectory->Get(hname2);
  if( hp2 == 0 ) {
    printf("DrawResi ERROR: No histogram %s\n",hname2);
    return -1;
  }
  return DrawResi( hp1, hp2, values1, values2, params, pname, fname );
}

//  Version fiting 2 gaussians constrained to have the same mean
int DrawResi(TH1 *hp1, TH1 *hp2, Double_t values1[VALUESMAX], Double_t values2[VALUESMAX], const Double_t *params, const char *pname, const char *fname ) {
// Zero values array
  for( int i=0; i<VALUESMAX; i++ ) {
    values1[i] = 0.;
    values2[i] = 0.;
  }
//  Do not plot if no entries or RMS==0 (entries are all underflow/overflow)
  if( hp1->GetEntries() == 0 || hp1->GetRMS() == 0 ) {
    printf("DrawResi WARNING: histogram %s no entries, skipping plot\n",hp1->GetName());
    return -1;
  }

  printf("DrawResi double fit: Fitting %s %s\n",hp1->GetName(),hp2->GetName());

// Add filename to title of histogram
// Rename histogram
  if( plotname != '\0' ) {
    if( pname != '\0' ) {
      hp1->SetTitle(Form("%s %s",plotname,pname));
    } else {
      hp1->SetTitle(Form("%s %s",plotname,hp1->GetTitle()));
    }
  } else if( pname != '\0' ) {  
    hp1->SetTitle(pname);
  }

  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  gStyle->SetTitleX(0.5);
  gStyle->SetTitleH(0.25);
  gStyle->SetTitleW(0.95);

  // Reset hp1 maximum if hp2 max is larger so hp2 does not go offscale
  if( hp2!=0 && hp1->GetMaximum() < hp2->GetMaximum() ) {
    hp1->SetMaximum( 1.05*hp2->GetMaximum() );
  }

  // Do the double gaussian fits
  FitDoubleGaussian(hp1,values1,params);
  FitDoubleGaussian(hp2,values2,params);

  TF1 *fun = hp1->GetFunction("fun3");
  fun->SetLineColor(kBlue);
  hp1->SetLineColor(kBlue);
  hp1->Draw();

  TPaveText *sigpt1 = new TPaveText(0.1,0.53,0.40,0.9,"NDC");
  sigpt1->SetTextAlign(12);
  sigpt1->SetTextColor(kBlue);
  sigpt1->AddText(Form("Hits %.0lf",hp1->GetEntries()));
  sigpt1->AddText(Form("#sigma_{1}=%.1lf #mum",values1[4]));
  if( values1[6] > 1000. ) {
    sigpt1->AddText(Form("#sigma_{2}=%.1lf mm",values1[6]/1000.));
  } else {
    sigpt1->AddText(Form("#sigma_{2}=%.1lf #mum",values1[6]));
  }
  sigpt1->AddText(Form("#mu=%.1lf #mum",values1[3]));
  sigpt1->Draw();

  if( hp2 ) {
    hp2->SetLineColor(kRed);
    hp2->Draw("same");
    TPaveText *sigpt2 = new TPaveText(0.64,0.53,0.94,0.9,"NDC");
    sigpt2->SetTextAlign(12);
    sigpt2->SetTextColor(kRed);
    sigpt2->AddText(Form("Hits %.0lf",hp2->GetEntries()));
    sigpt2->AddText(Form("#sigma_{1}=%.1lf #mum",values2[4]));
    if( values2[6] > 1000. ) {
      sigpt1->AddText(Form("#sigma_{2}=%.1lf mm",values2[6]/1000.));
    } else {
      sigpt2->AddText(Form("#sigma_{2}=%.1lf #mum",values2[6]));
    }
    sigpt2->AddText(Form("#mu=%.1lf #mum",values2[3]));
    sigpt2->Draw();
  }

// Print plot to file
  if( filename != '\0' ) {
    if( fname != '\0' ) {
      gPad->Print(Form("%s_%s.png",filename,fname));
    } else {
      gPad->Print(Form("%s_%s.png",filename,hp1->GetTitle()));
    }
  } else if( fname != '\0' ) {  
    gPad->Print(Form("%s.png",hp1->GetTitle()));
  }

  //  if( fname[0] == '\0' ) gPad->Print(Form("%s.png",hp1->GetName()));
  //else                   gPad->Print(Form("%s.png",fname));

  return 0;
} //end DrawResi (two histogram version)


// Do a DoubleGaussian fit on a histogram.  Mean of 2 gaussians constrained to be the same
int FitDoubleGaussian(TH1 *hp, Double_t values[VALUESMAX], const Double_t *params) {

  if( hp == 0 ) return -1;

//  Set initial guesses of parameters
  Int_t    imax = hp->GetMaximumBin();
  Double_t mean = hp->GetMean();
  Float_t  ampl = (hp->GetBinContent(imax-1)+hp->GetBinContent(imax)+hp->GetBinContent(imax+1))/3.;
  Float_t  rms  = hp->GetRMS();

// Set fit limits according to param[1]
  Double_t xmin=hp->GetXaxis()->GetBinLowEdge(1);
  Double_t xmax=hp->GetXaxis()->GetBinUpEdge( hp->GetXaxis()->GetNbins() );
  if(      params[1] > 0. ){
    xmin=mean-params[1]*rms;
    xmax=mean+params[1]*rms;
  } else if( params[1] < 0. ){
    xmin= params[1];
    xmax=-params[1];
  }

  TF1 *fun3 = new TF1("fun3",DoubleG,xmin,xmax,5);
  fun3->SetParNames("Const1","Sigma1","Mean","Const2","Sigma2");
  fun3->SetParameters(ampl,rms/4,mean,ampl/10.,rms*2); // constant,mean,sigma

  hp->Fit("fun3","RQ");

//  Determine SigmaHM.  Do binary search to find half max points on both sides
  Double_t Xmax = fun3->GetMaximumX(mean-rms,mean+rms);
  Double_t Hmax = fun3->Eval(Xmax)/2;   //GetMaximum(mean-rms,mean+rms);
  Double_t X1 = Xmax-2*rms;
  Double_t X2 = Xmax;
  Double_t Xlo = (X1+X2)/2.;
  for( int ii=0; ii<25; ii++ ) {
    if( fun3->Eval(Xlo) < Hmax ) X1 = Xlo;
    else                         X2 = Xlo;
    Xlo = (X1+X2)/2.;
  }
  X1 = Xmax;
  X2 = Xmax+2*rms;
  Double_t Xhi = (X1+X2)/2.;
  for( int ii=0; ii<25; ii++ ) {
    if( fun3->Eval(Xhi) < Hmax ) X2 = Xhi;
    else                         X1 = Xhi;
    Xhi = (X1+X2)/2.;
  }
  Xmax *= params[0];
  Double_t sigmaHM = (Xhi-Xlo)/2.3548*params[0];
  Float_t sig1   = TMath::Abs(fun3->GetParameter(1))*params[0];
  Float_t sig2   = TMath::Abs(fun3->GetParameter(4))*params[0];
  Float_t mean1  = fun3->GetParameter(2)*params[0];
  Float_t mean2  = fun3->GetParameter(2)*params[0];
  Float_t const1 = fun3->GetParameter(0);
  Float_t const2 = fun3->GetParameter(3);
  if( sig1 < sig2 ) {
    values[3] = mean1;
    values[4] = sig1;
    values[5] = mean2;
    values[6] = sig2;
    values[7] = sig1*const1/sig2/const2;
    values[9]  = fun3->GetParError(2)*params[0];
    values[10] = fun3->GetParError(2)*params[0];
    values[11] = fun3->GetParError(1)*params[0];
    values[12] = fun3->GetParError(4)*params[0];
 } else {
    values[3] = mean2;
    values[4] = sig2;
    values[5] = mean1;
    values[6] = sig1;
    values[7] = sig2*const2/sig1/const1;
    values[9]  = fun3->GetParError(2)*params[0];
    values[10] = fun3->GetParError(2)*params[0];
    values[11] = fun3->GetParError(4)*params[0];
    values[12] = fun3->GetParError(1)*params[0];
  }
  values[0] = hp->GetEntries();
  values[1] = hp->GetMean()*params[0];
  values[2] = hp->GetRMS()*params[0];
  if( values[0] > 0. ) values[8] = values[2]/TMath::Sqrt(values[0]);
  values[13] = Xmax;
  values[14] = sigmaHM;
  values[15] = values[9];
  values[16] = values[11];

  return 0;
} //end FitDoubleGaussian


/************************************************************
*  Draw individual 1D histograms with a single Gaussian fit
*  values[0] = Number of entries
*  values[1] = Histogram mean
*  values[2] = RMS
*  values[3] = Gasssian mean;
*  values[4] = Gaussian sigma;
*  values[5] = Histogram mean error = rms/sqrt(n)
*  values[6] = Gaussian mean error
*  values[7] = Gaussian sigma error
************************************************************/
int DrawGaus(const char *hname, Double_t values[VALUESMAX], const char *fname ) {
// Zero values array
  for( int i=0; i<VALUESMAX; i++ ) values[i] = 0.;

//  By using TH1 this works for TH1F, TH1D, etc
  TH1 *hp = (TH1 *) gDirectory->Get(hname);
  if( hp == 0 ) {
    printf("DrawGaus ERROR: No histogram %s\n",hname);
    return -1;
  }
  return DrawGaus_( hp, values, fname );
}
int DrawGaus( TH1 *hp , Double_t values[VALUESMAX], const char *fname ) {
// Zero values array
  for( int i=0; i<VALUESMAX; i++ ) values[i] = 0.;
  return DrawGaus_( hp, values, fname );
}

//  The version which does everything
int DrawGaus_( TH1 *hp , Double_t values[VALUESMAX], const char *fname ) {

//  By using TH1 this works for TH1F, TH1D, etc
  if( hp == 0 ) {
    printf("DrawGaus ERROR: Null histogram pointer passed\n");
    return -1;
  }
//  Do not plot if no entries or if RMS=0
  if( hp->GetEntries() == 0 || hp->GetRMS() == 0 ) {
    printf("DrawGaus WARNING: histogram %s no entries, skipping plot\n",hp->GetName());
    return -1;
  }

  printf("DrawGaus: Fitting %s\n",hp->GetName());

// Add filename to title of histogram
  if( fname != '\0' ) {
    char htitle[80] = "",pname[80];
// Remove _ from filename to use for plot names
    for( int i=0; fname[i]!='\0'; i++ ) {
      if( fname[i] == '_' ) pname[i] = ' ';
      else                  pname[i] = fname[i];                 
    }
    sprintf(htitle,"%s %s",plotname,hp->GetTitle());
    hp->SetTitle(htitle);
  }

  gStyle->SetOptStat("emr"); 
//   gStyle->SetStatX(1.0);
  gStyle->SetStatW(0.4);
  gStyle->SetStatH(0.35);

  TF1 *fun1=new TF1("fun1","gaus");
  fun1->SetParNames("Const","Mean","Sigma");
//  Set initial guesses of parameters
  Int_t       i = hp->GetMaximumBin();
  Double_t ampl = (hp->GetBinContent(i-1) + hp->GetBinContent(i) + hp->GetBinContent(i+1))/3.;
  Double_t mean = hp->GetMean();
  Double_t rms  = hp->GetRMS();
  Double_t peak = hp->GetBinCenter(hp->GetMaximumBin());
//  Find narrow peak on a background.  
  Double_t  lo, hi;
//   Double_t halfmax = hp->GetMaximum()/3.;     //set to half max
//   lo = hp->GetBinCenter(1);
//   hi = hp->GetBinCenter(hp->GetNbinsX()-1);
//  Find lo
//   for( int i=hp->GetMaximumBin(); i>0; i-- ) {
//     if( hp->GetBinContent(i) < halfmax ) {
//       lo = hp->GetBinCenter(i);
//       break;
//     } 
//   }
//  Find hi
//   for( int i=hp->GetMaximumBin(); i<hp->GetNbinsX(); i++ ) {
//     if( hp->GetBinContent(i) < halfmax ) {
//       hi = hp->GetBinCenter(i);
//       break;
//     } 
//   }
/*   lo = mean-rms; */
/*   hi = mean+rms;   */
  lo = peak-6;
  hi = peak+6;  
  fun1->SetParameters(ampl,peak,rms);    //constant,mean,sigma
  hp->Fit("fun1","","",lo,hi);
//  Draw fit results on plot
//  TPaveText *sigpt = new TPaveText(0.16,0.79,0.45,0.99,"NDC");  //without histo title 
//  TPaveText *sigpt = new TPaveText(0.16,0.74,0.45,0.94,"NDC");    //with histo title
  TPaveText *sigpt = new TPaveText(0.1,0.7,0.4,0.9,"NDC"); 
  sigpt->SetFillColor(0);
  sigpt->SetTextAlign(12);
  Float_t mean1 = fun1->GetParameter(1);
  Float_t sig1  = TMath::Abs(fun1->GetParameter(2));
  //  sigpt->AddText(Form("Peak=%.1lf ns",peak));
  sigpt->AddText(Form("#mu=%.1lf ns",mean1));
  sigpt->AddText(Form("#sigma=%.1lf ns",sig1));
  sigpt->Draw();

  if( fname[0] == '\0' ) gPad->Print(Form("%s.png",hp->GetName()));
  else                   gPad->Print(Form("%s_%s.png",fname,hp->GetName()));

//  We already know entries>0
  values[0] = hp->GetEntries();
  values[1] = mean;
  values[2] = rms;
  if( values[0] > 0. ) values[5] = values[2]/TMath::Sqrt(values[0]);
// Make sure gaussian mean falls within limits of the histogram range
// i.e. check that fit is reasonable.
  if( mean < hp->GetXaxis()->GetXmax() && mean > hp->GetXaxis()->GetXmin() ) {
    values[3] = mean1;
    values[4] = sig1;
    values[6] = fun1->GetParError(1);
    values[7] = fun1->GetParError(2);
  }
  
  return 0;
}  //end DrawGaus()

/************************************************************
*  Draw individual 1D histograms
*  Histogram hname is read from the open ROOT file
************************************************************/
void Drawh1( const char *hname, const char *opt, const char *stat) {
//  By using TH1 this works for TH1F, TH1D, etc
  TH1 *hp = (TH1 *) gDirectory->Get(hname);
  if( hp == 0 ) {
    printf("Drawh1 ERROR: No histogram %s \n",hname);
    return;
  }

//  Do not plot if no entries or RMS==0 (entries are all underflow/overflow)
  if( hp->GetEntries() == 0 ) {
    printf("Drawh1 WARNING: histogram %s no entries, skipping plot\n",hname);
    return;
  }

// Rename histogram
  if( plotname != '\0' ) {
    char htitle[80] = "";
    sprintf(htitle,"%s %s",plotname,hp->GetTitle());
    hp->SetTitle(htitle);
  }

  gStyle->SetOptStat(stat); 
//   gStyle->SetStatX(1.0);
//   gStyle->SetStatW(0.25);
//   gStyle->SetStatY(1.0);
//   gStyle->SetStatH(0.2);

//  Reset various parameters of the histogram
  hp->Draw(opt);
  gPad->Print(Form("%s_%s.%s",filename,hname,chend));
  return;
}  //end Drawh1

/************************************************************
*  Draw individual 2D histograms
************************************************************/
void Drawh2(const char *hname, const char *chopt, const char *xlab, const char *ylab, 
	    const char *chstat ) {
//  By using TH2 this works for TH2F, TH2D, etc
  TH2 *hp = (TH2 *) gDirectory->Get(hname);
  if( hp == 0 ) {
    printf("Drawh2 ERROR: No histogram %s\n",hname);
    return;
  }
// Skip empty histograms
  if( hp->GetEntries() == 0. ) {
    printf("Drawh2 Warning: histogram %s no entries, not printing\n",hname);
    return;
  }

// Rename histogram
  if( plotname != '\0' ) {
    char htitle[80] = "";
    sprintf(htitle,"%s %s",plotname,hp->GetTitle());
    hp->SetTitle(htitle);
  }

  gStyle->SetOptStat(chstat); 
//   gStyle->SetStatX(1.0);
//   gStyle->SetStatW(0.25);
//   gStyle->SetStatY(1.0);
//   gStyle->SetStatH(0.2);

//  Reset various parameters of the histogram
  if( xlab[0] != '\0' ) hp->GetXaxis()->SetTitle(xlab); 
  if( ylab[0] != '\0' ) hp->GetYaxis()->SetTitle(ylab); 
  hp->Draw(chopt);
  gPad->Print(Form("%s_%s.%s",filename,hname,chend));
  return;
}

// void TwoPlot( TH1 *h[], int inorm, const char *title ) {
//   printf("TwoPlot inorm = %i \n",inorm);
//   for( int i=0; i<2; i++ ) {
//     if( h[i] ) {
//       printf("TwoPlot segfault now histo %s\n",h[0]->GetName());
//     } else {
//       printf("h[%i] is null\n",i);
//     }
//   }
//   gStyle->SetOptStat(0);
//   gPad->SetTicks(0,0);

//   if( h[0] == NULL || h[1] == NULL ) {
//     printf("ERROR: Twoplot null pointer\n");
//     return;
//   }
//   h[0]->SetFillColor(3);
//   h[0]->Draw();

//   gPad->Update();   //I don't know what this does but if you don't do it the axis will not work
//   Float_t rightmax = 1.1*h[0]->GetMaximum();
//   if( inorm == 1 && h[1]->GetEntries()>0) {
//     printf("inorm = 1\n");
//     rightmax *= h[0]->GetEntries()/h[1]->GetEntries();
//     h[1]->Scale(h[0]->GetEntries()/h[1]->GetEntries());
//   } else if (inorm == 2 && h[1]->GetMaximum()>0) {
//     printf("inorm = 2\n");
//     rightmax = 1.1*h[1]->GetMaximum();
//     h[1]->Scale(gPad->GetUymax()/rightmax);
//   }
// //draw an axis on the right side
//   h[1]->Draw("SAME");
// //  Using NDC coordinates, as per ROOT manual example, but which does not work
//   TGaxis *axis = new TGaxis(gPad->GetUxmax(),gPad->GetUymin(),gPad->GetUxmax(),
// 			    gPad->GetUymax(),0,rightmax,510,"NDC+L");
//   axis->Draw();

// //  Make a title from the titles of the 2 histograms else use the one supplied
//   h[0]->SetBit(TH1::kNoTitle);   //suppress the original histogram title
// //  TPaveText *titlept = new TPaveText(0.02,0.92,0.75,0.99,"NDC");   //with histo title
//   TPaveText *titlept = new TPaveText(0.02,0.82,0.75,0.89,"NDC");       //without histo title
//   if( title[0] != '\0' ) {
//     titlept->AddText(title);                        
//   } else {
//     titlept->AddText(Form("%s (L) & %s (R)",h[0]->GetTitle(),h[1]->GetTitle()));
//   }
//   titlept->Draw();     

// //  Print to a file
//   gPad->Print(Form("%s_%s_TP.png",h[0]->GetName(),h[1]->GetName()));
//   h[0]->ResetBit(TH1::kNoTitle);   //enable the histogram title
// }     //End TwoPlot
void makebackground(TCanvas *can, int iw, TH1F *h1, TH1F *h2, Double_t ylo, Double_t yhi ) {
  TH2F *bckh2=NULL;
  TH1F *bckgd = new TH1F("tzbg","",wheelbins[iw],binedges[iw]);
  if( ylo < yhi ) {
    bckh2 = new TH2F("bckh2",h1->GetTitle(), wheelbins[iw%6],binedges[iw%6],
		     100,h1->GetMinimum(),1.05*h1->GetMaximum());
  } else {
    bckh2 = new TH2F("bckh2",h1->GetTitle(), wheelbins[iw%6],binedges[iw%6],100,ylo,yhi);
  }
  bckh2->SetStats(0);
  bckh2->GetXaxis()->SetTitle("chamber (by phi & eta)");
  bckh2->GetYaxis()->SetTitle(h1->GetYaxis()->GetTitle());
  bckh2->GetXaxis()->SetTickLength(0);  //suppress X ticks
  AddHistBinLabels(bckh2,&binlabels[iw%6][0]);
  Background( can, bckgd, bckh2, 18 ); 
  if( h2 != NULL ) {
    h1->SetLineColor(4);
    h2->SetLineColor(2);
    h2->Draw("same");
    TLegend *tzlegend = new TLegend( 0.70, 0.11, 0.89, 0.31 );
    tzlegend->AddEntry(h1,"Standalone");
    tzlegend->AddEntry(h2,"Combined");
    tzlegend->Draw();
  }
  h1->Draw("same");
  gPad->Print(Form("%s.png",h1->GetName()));
  printf("print %s.png\n",h1->GetName());
  bckgd->Delete();
  bckh2->Delete();
}
/*########################################################
  Draw 2 histograms to TCanvas c1 and save to a file.
   hname1, hname2 = Names of histograms to be plotted (assumed to be in gDirectory)
   pname = Plot title (overwrites hname1 title)
   fname = File name for image file, including extension (png, gif, etc)
       Default filename is <Histogram Name>.png
   iline = flag to indicate if 2nd histogram is drawn as solid red (0) or red line (1)
   data = Array to write histogram stats data 
          [0,1,2] = events, mean, rms of hname1, 
          [3,4,5] = events, mean, rms of hname2
  Stats box for second histogram is drawn below first histo stats box
  ########################################################*/
void Drawh1h1(const char *hname1, const char *hname2, Double_t data[6], const char *pname, const char *fname, const int iline ) {
  for( int i=0; i<6; i++ ) data[i]=0.;

//  By using TH1 this works for TH1F, TH1D, etc
  TH1 *h1 = (TH1 *) gDirectory->Get(hname1);
  if( h1 == 0 ) {
    printf("Drawh1h1 ERROR: No histogram %s\n",hname1);
    return;
  }
// Get the other histogram
  TH1 *h2 = (TH1 *) gDirectory->Get(hname2);

//  Do not plot if no entries 
  if( h1->GetEntries() == 0. ) {
    printf("Drawh1h1 WARNING: histogram %s no entries, skipping plot\n",hname1);
    return;
  }

// Name for PNG file (do before changing histogram title)
  char pngname[80] = "file.png";
  if( fname[0] != '\0' ) {
    if( filename[0] != '\0' ) {
      sprintf(pngname,"%s_%s.png",filename,fname);
    } else {
      sprintf(pngname,"%s.png",fname);
    }
  } else if( filename[0] != '\0' ) {  
    sprintf(pngname,"%s_%s.png",filename,h1->GetName());
  } else {
    sprintf(pngname,"%s.png",h1->GetName());
  }
printf("%s fname=%s plotname=%s pngname=%s\n",h1->GetTitle(),fname,plotname,pngname);

// Rename histogram
  if( plotname != '\0' ) {
    if( pname != '\0' ) {
      h1->SetTitle(Form("%s %s",plotname,pname));
    } else {
      h1->SetTitle(Form("%s %s",plotname,h1->GetTitle()));
    }
  } else if( pname != '\0' ) {  
    h1->SetTitle(pname);
  }

  // Reset h1 maximum if h2 max is larger so h2 does not go offscale
  if( h2!=0 && h1->GetMaximum() < h2->GetMaximum() ) {
    h1->SetMaximum( 1.05*h2->GetMaximum() );
  }

  // Set h1 min to 0 to prevent h2 from being cut off if it has many fewer events 
  h1->SetMinimum(0.);
  h1->Draw();

  data[0] = h1->GetEntries();
  data[1] = h1->GetMean();
  data[2] = h1->GetRMS();

  if( h2!=0 && h2->GetEntries()!=0. ) {
    data[3] = h2->GetEntries();
    data[4] = h2->GetMean();
    data[5] = h2->GetRMS();
    if( iline == 0 ) {
      h2->SetFillColor(kRed);
    } else if( iline == 1 ) {
      h1->SetLineWidth(2);
      h2->SetLineWidth(2);
      h2->SetLineColor(kRed);
    }
    h2->Draw("sames");
    gPad->Update();
//  Change color of h2 stats box and move it below the other stats box
    TPaveStats *s = (TPaveStats*) h2->FindObject("stats");
    if( s == 0 ) {
      printf("Drawh1h1 ERROR finding stats box %s\n",hname2);
    } else {
      float ymin = s->GetY1NDC();
      float ymax = s->GetY2NDC();
      s->SetY2NDC(ymin);
      s->SetY1NDC(2*ymin-ymax);
      s->SetTextColor(kRed);
    }
  }

// Print plot to file
  gPad->Print(pngname);
  return;
}  //Drawh1h1

void Drawh1h1h1(const char *hname1, const char *hname2, const char *hname3, Double_t data[6], const char *pname ) {
//  By using TH1 this works for TH1F, TH1D, etc
  TH1 *h1 = (TH1 *) gDirectory->Get(hname1);
  if( h1 == 0 ) {
    printf("Drawh1h1h1 ERROR: No histogram %s\n",hname1);
    return;
  }

//  Do not plot if no entries 
  if( h1->GetEntries() == 0. ) {
    printf("Drawh1h1h1 WARNING: histogram %s no entries, skipping plot\n",hname1);
    return;
  }

  TH1 *h2 = (TH1 *) gDirectory->Get(hname2);
  if( h2==0 ) {
    printf("Drawh1h1h1 WARNING: NO histogram %s\n",hname2);
  }

  Drawh1h1h1_( h1, h2, hname3, data, pname );
  return;
}   //end Drawh1h1h1
/*########################################################
  Draw 3 histograms and save to a file.
   fname = File name for image file, including extension (png, gif, etc)
       Default filename is <Histogram Name>.png
  Second histogram is draw on first histogram in different color set by fillcolor
  Stats box for second histogram is drawn below first histo stats box
  ########################################################*/
void Drawh1h1h1_(TH1 *h1, TH1 *h2, const char *hname3, Double_t data[6], const char *pname ) {
  for( int i=0; i<6; i++ ) data[i]=0.;

//  By using TH1 this works for TH1F, TH1D, etc
  if( h1 == 0 ) return;

//  Do not plot if no entries 
  if( h1->GetEntries() == 0. ) {
    printf("Drawh1h1h1 WARNING: histogram %s no entries, skipping plot\n",h1->GetName());
    return;
  }

// Rename histogram
  if( plotname != '\0' ) {
    char htitle[80] = "";
    if( pname != '\0' ) {
      sprintf(htitle,"%s %s",plotname,pname);
    } else {
      sprintf(htitle,"%s %s",plotname,h1->GetTitle());
    }
    h1->SetTitle(htitle);
  }

  h1->SetMinimum(0.);
  h1->SetLineWidth(2);
  h1->Draw();
  gPad->Update();

  data[0] = h1->GetEntries();
  data[1] = h1->GetMean();
  data[2] = h1->GetRMS();

  if( h2!=0 && h2->GetEntries()!=0. ) {
    h2->SetLineColor(kGreen);
    h2->SetLineWidth(2);
    h2->Draw("sames");
    gPad->Update();
//  Change color of h2 stats box and move it below the other stats box
    TPaveStats *s = (TPaveStats*) h2->FindObject("stats");
    if( s == 0 ) {
      printf("Drawh1h1h1 ERROR finding stats box %s\n",h2->GetName());
    } else {
      float ymin = s->GetY1NDC();
      float ymax = s->GetY2NDC();
      s->SetY2NDC(ymin);
      s->SetY1NDC(2*ymin-ymax);
      s->SetTextColor(kGreen);
    }
  } 

  TH1 *h3 = (TH1 *) gDirectory->Get(hname3);
  if( h3!=0 && h3->GetEntries()!=0. ) {
    data[3] = h3->GetEntries();
    data[4] = h3->GetMean();
    data[5] = h3->GetRMS();
    h3->SetLineColor(kRed);

//scale h3 to the pad coordinates
    Float_t rightmax = 1.1*h3->GetMaximum();
    Float_t scale = gPad->GetUymax()/rightmax;
    h3->SetLineWidth(2);
    h3->Scale(scale);
    //   h3->Draw("same");
//draw an axis on the right side
    TGaxis *axis = new TGaxis(gPad->GetUxmax(),gPad->GetUymin(),gPad->GetUxmax(),
			      gPad->GetUymax(),0,rightmax,510,"-L");
    axis->SetLineColor(kRed);
    axis->SetLabelColor(kRed);
    axis->Draw();

    h3->Draw("sames");
    gPad->Update();
//  Change color of h3 stats box and move it below the other stats box
    TPaveStats *s = (TPaveStats*) h3->FindObject("stats");
    if( s == 0 ) {
      printf("Drawh1h1h1 ERROR finding stats box %s\n",hname3);
    } else {
      float ymin = s->GetY1NDC();
      float ymax = s->GetY2NDC();
      s->SetY2NDC(2*ymin-ymax);
      s->SetY1NDC(3*ymin-2*ymax);
      s->SetTextColor(kRed);
    }
  } else {
    printf("Drawh1h1h1 WARNING: NO histogram %s\n",hname3);
  }

  gPad->Print(Form("%s_%s.png",filename,h1->GetName()));
  return;
}
