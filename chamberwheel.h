//********************************************************************
//  Arrays/routine for defining a "wheel" & chamber within wheel for plots
//  150313 Update for Run2 (add BME,BOE)
//********************************************************************
#ifndef chamberwheel_h
#define chamberwheel_h

#include <stdio.h>
// Hardcoded for condor jobs
#include "chamberlist.h"

// This structure is returned by get_chwheel()
typedef struct { 
  int idx;     //chamber index in chamberlist.h [0..1173]
  int icham;   //chamber index within wheel [1..x] (varies by wheel)
  int iwheel;  //wheel of chamber [0..11]
} wheeldata;

// Returns info on chamber and wheel
wheeldata get_chwheel( char *chamber );

// Names of all MDT wheels 
const char *chwheel[] = { "BIA", "BMA", "BOA", "EIA", "EMA", "EOA",  
                          "BIC", "BMC", "BOC", "EIC", "EMC", "EOC", "ALL" };
// Number of chambers in each wheel
const int wheelmax[]  = {   118,   103,   115,   112,    80,   96 };
// Number of bins (eta regions) of each wheel
const int wheelbins[] = {   8  ,   7  ,     8,    10,    5 ,    6 };
// Labels for eta regions of wheels
const char  *binlabels[6][11] = { 
  {"BI1","BI2","BI3","BI4"  ,"BI5","BI6","BIM","BIR"  ,""    ,""    ,""},
  {"BM1","BM2","BM3","BM4"  ,"BM5","BM6","BMEF" ,""    ,""    ,""    ,""},
  {"BO1","BO2","BO3","BO4"  ,"BO5","BO6","BOEF"  ,"BOG" ,""    ,""    ,""},
  {"EI1","EI2","EI3","EI4/5","EE1","EE2","BIS7","BIS8","BEE1","BEE2",""},
  {"EM1","EM2","EM3","EM4"  ,"EM5",""   ,""    ,""    ,""    ,""    ,""},
  {"EO1","EO2","EO3","EO4"  ,"EO5","EO6",""    ,""    ,""    ,""    ,""} 
};
// Edges (chamber numbers) of wheel eta regions
const Double_t binedges[6][11] = {
  {1    ,17   ,33   ,49   ,65     ,81   ,97    ,107   ,119   ,0     ,0}, //BI
  {1    ,17   ,33   ,49   ,65     ,81   ,97    ,105   ,0     ,0     ,0}, //BM
  {1    ,17   ,33   ,49   ,65     ,81   ,97    ,106   ,116   ,0     ,0}, //BO
  {1    ,17   ,33   ,41   ,51     ,67   ,83    ,91    ,99    ,107, 113}, //EI
  {1    ,17   ,33   ,49   ,65     ,81   ,0     ,0     ,0     ,0     ,0}, //EM
  {1    ,17   ,33   ,49   ,65     ,81   ,97    ,0     ,0     ,0     ,0}  //EO 
};
const int MAXCHWHEEL=118;   //max number of chambers in wheel

// Labels for using chamber numbering from chamberlist.h
const int chambins = 15;
const char *chamlabels[] = { 
  "BE","BIL","BIMR","BIS","BMEFL","BMS","BOFGL","BOS","EE","EIL","EIS","EML","EMS","EOL","EOS", "" };
const Double_t chamedges[] = {
    0.,  32.,  104., 148.,   276., 384.,   456., 588.,660., 722., 790., 822., 902., 982.,1078., 1174. };
// Labels for splitter regions
const int regionbins = 14;
const char *regionlabels[] = { 
  "BA12","BA34","BA56","BC12","BC34","BC56","EA45","BA7","EA23","EA12","EC45","BC7","EC23","EC12", "" };
const Double_t regionedges[] = {
       1,    17,    33,    51,    67,    83,   101,  109,   117,   133,   153,  161,   169,   185, 205  };


/********************************************************************/
//  Compute the wheel and chamber numbers for a given chamber
//  This is pretty thorny because of all the special chambers
//  iwheel = [0..11]  => wheel index in chwheel[]
//  icham  = [0..117] => chamber index within wheel
//           This numbering goes:
//  <all phi stations (L,S) for eta=1>, <all phi stations (L,S) for eta=2>, ...
//  idx    = [0..1173] => chamber index within chlist[]
//  chamber = MDT chamber hardware name
/********************************************************************/
wheeldata get_chwheel( char *chamber ) {
  char type[4];
  int idx, iphi, ieta, iphi_skip, ioff, iwheel;
  wheeldata wd = { -1, -1, -1 };

  sprintf(type,"%c%c%c",chamber[0],chamber[1],chamber[4]);
  idx = checkchamber(chamber,1,1,1);
  if( idx == -1 ) {
    printf("chamberwheel ERROR: unknown chamber %s\n",chamber);
    return wd;
  }
// General eta,phi conversion, works for L,S chambers.  Handle exceptions below
  ieta = chlist[idx].eta;           //hardware eta (unsigned)
  if( ieta < 0 ) ieta = -ieta;   
  iphi = 2*chlist[idx].phi;         //hardware phi [1..16]
  if( chamber[2] == 'L' ) iphi -= 1;
  ioff = 0;                //offset for special chambers in each wheel
  iphi_skip = 16;          //number of chambers at given eta value
//  BIS (except 7,8), BIL, BIM, BIR
//    1- 96: BIS/BIL
//   97-106: BIM
//  107-118: BIR
  if( (strcmp(type,"BIA") == 0 || strcmp(type,"BIC") == 0) && ieta<7) { 
    iwheel = 0;
    if( chamber[2] == 'M' || chamber[2] == 'R' ) { //BIM/BIR
      iphi_skip = 2;
      iphi = ( chlist[idx].phi == 6 ) ? 1 : 2;  //iphi=1,2 
      ioff = ( chamber[2] == 'M')    ? 96 : 106;
    }
//  BMS, BML, BME, BMF 
//    1- 96: BMS/BML
//       97: BME
//   98-104: BMF
  } else if( strcmp(type,"BMA") == 0 || strcmp(type,"BMC") == 0 ) { 
    iwheel = 1;
    if( chamber[2] == 'E' ) { //BME
      iphi_skip = 0;
      iphi = 1;       //only one BME per BMA, BMC wheel           
      ioff = 96;                      
    } else if( chamber[2] == 'F' ) { //BMF
      iphi_skip = 2;                //2 phi values for each eta
      iphi = chlist[idx].phi - 5;   //iphi=1,2 
      ioff = 97;
    }
//  BOS, BOL, BOE, BOF, BOG 
//    1- 96: BOS/BOL
//       97: BOE
//   98-105: BOF
//  106-115: BOG (BOG0=114,115)
  } else if( strcmp(type,"BOA") == 0 || strcmp(type,"BOC") == 0 ) {
    iwheel = 2;
    if( chamber[2] == 'E' ) { //BOE
      iphi_skip = 0;
      iphi = 1;
      ioff = 96;
    } else if( chamber[2] == 'F' || chamber[2] == 'G') { //BOF/BOG
      iphi_skip = 2;
      iphi = chlist[idx].phi - 5;   //iphi=1,2 
      ioff = ( chamber[2] == 'F') ? 97 : 105;
      if( ieta == 0 ) ioff = 115;    //put BOG0 after the rest (only on A side)
    }

// EI, EE, BIS7,8, BEE
//  1-16 =EIS1,EIL1 
//  17-32=EIS2,EIL2
//  33-40=EIL3
//  41-48=EIL4
//  49-50=EIL5
//  51-66=EES1,EEL1
//  67-82=EES2,EEL2
//  83-90= BIS7
//  91-98= BIS8
//  99-106=BEE1
//  107-112=BEE2
  } else if( strcmp(type,"EIA") == 0 || strcmp(type,"EIC") == 0 ) { 
    iwheel = 3;
    if( ieta == 3 || ieta == 4 ) { //EIL3,4
      ieta -= 2;
      iphi = chlist[idx].phi;
      iphi_skip = 8;
      ioff = 32;
    } else if( ieta == 5 ) {        //EIL5
      ieta = 1;
      iphi = ( chlist[idx].phi == 1 ) ? 1 : 2;
      iphi_skip = 2;
      ioff = 48;
    }
  } else if( strcmp(type,"EEA") == 0 || strcmp(type,"EEC") == 0 ) { 
    iwheel = 3;
    ioff   = 50;
  } else if( (strcmp(type,"BIA") == 0 || strcmp(type,"BIC") == 0) && ieta>6) { 
    iwheel = 3;
    iphi = chlist[idx].phi;
    ieta -= 6;
    iphi_skip = 8;
    ioff = 82;
  } else if( strcmp(type,"BEA") == 0 || strcmp(type,"BEC") == 0 ) { 
    iwheel = 3;
    iphi = chlist[idx].phi;
    iphi_skip = 8;
    ioff = 98;
//  EMS, EML 1-80
  } else if( strcmp(type,"EMA") == 0 || strcmp(type,"EMC") == 0 ) { 
    iwheel = 4;
//  EOS, EOL 1-96
  } else if( strcmp(type,"EOA") == 0 || strcmp(type,"EOC") == 0 ) { 
    iwheel = 5;
  } else { 
    printf("chamberwheel ERROR: unknown chamber %s\n",chamber);
    return wd;
  }
  if( chamber[4] == 'C' ) iwheel += 6;

  wd.idx    = idx;
  wd.icham  = ioff + iphi + iphi_skip*(ieta - 1);
  wd.iwheel = iwheel;
  return wd;
}  //end get_chwheel()
#endif
