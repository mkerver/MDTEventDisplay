#ifndef muonfixedid_h
#define muonfixedid_h
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* A bunch of utility functions to convert a MuonFixedID to ordinary
chamber info.  Code adapted from Athena MuonFixedID.h 
Made these a bunch of independent functions, rather than part of a class.
The "Muon fixed ID" is passed/returned to/from functions
Got rid of all the "index" functions - combined them in other functions.

Added a number of new functions:
void ChamberName(int mfid, char *c);  //writes MDT chambername on c (e.g BML3A13)
void TubeName(int mfid, char *c);     //writes MDT tube name: chamber-ml-l-t on c
int SetMdtID( char *chamber, int ml, int ly, int tb ); //returns mfid for an MDT chamber ml-layer-tube
void RpcName(int mfid, char *c);      //writes RPC name (guess)
void RpcChamber(int mfid, char *c);   //writes RPC chamber (guess)
void TgcName(int mfid, char *c);      //writes full TGC name (guess)
void TgcChamber(int mfid, char *c);   //writes TGC chamber name (guess)
void CscName(int mfid, char *cname);  //Writes the CSC chamber name (guess)
int CompareChambers( const char *cham1, const char *cham2 ); //String-based comparison
int CompChambers( int mfid1, int mfid2 ); //Return 0 if (MDT) chambers are the same
  Edward Diehl 8-May-2007
*/

// Unused bits 11-13 which are set to 111 in MDT muonfixedid's
static const int kUnusedBits       = 7168;

static const int kTechnologyMask   = 3;
static const int kTechnologyShift  = 30;
static const int kTechnologyMin    = 0;

static const int kStationNameMask  = 63;
static const int kStationNameShift = 24;
static const int kStationNameMin   = 1;

static const int kStationEtaMask   = 31;
static const int kStationEtaShift  = 19;
static const int kStationEtaMin    = -8;

static const int kStationPhiMask   = 63;
static const int kStationPhiShift  = 13;
static const int kStationPhiMin    = 1;

//   Mdt specific code 
static const int kMdtMultilayerMask    = 1;
static const int kMdtMultilayerShift   = 9;
static const int kMdtMultilayerMin     = 1;

static const int kMdtTubeLayerMask     = 3;
static const int kMdtTubeLayerShift    = 7;
static const int kMdtTubeLayerMin      = 1;

static const int kMdtTubeMask          = 127;
static const int kMdtTubeShift         = 0;
static const int kMdtTubeMin           = 1;

//  Rpc specific code
static const int kRpcDoubletRMask      = 1;
static const int kRpcDoubletRShift     = 12; 
static const int kRpcDoubletRMin       = 1;
static const int kRpcDoubletZMask      = 3;
static const int kRpcDoubletZShift     = 10; 
static const int kRpcDoubletZMin       = 1;
static const int kRpcDoubletPhiMask    = 1;
static const int kRpcDoubletPhiShift   = 9; 
static const int kRpcDoubletPhiMin     = 1;
static const int kRpcGasGapMask        = 1;
static const int kRpcGasGapShift       = 8;  
static const int kRpcGasGapMin         = 1;
static const int kRpcMeasuresPhiMask   = 1;
static const int kRpcMeasuresPhiShift  = 7;  
static const int kRpcMeasuresPhiMin    = 0;
static const int kRpcStripMask         = 127;
static const int kRpcStripShift        = 0; 
static const int kRpcStripMin          = 1;

// Tgc specific code 
static const int kTgcGasGapMask        = 3;
static const int kTgcGasGapShift       = 9;
static const int kTgcGasGapMin         = 1;
static const int kTgcIsStripMask       = 1;
static const int kTgcIsStripShift      = 8;
static const int kTgcIsStripMin        = 0;
static const int kTgcChannelMask       = 255;
static const int kTgcChannelShift      = 0;
static const int kTgcChannelMin        = 1;

//  Csc specific code
static const int kCscChamberLayerMask  = 1;
static const int kCscChamberLayerShift = 11;
static const int kCscChamberLayerMin   = 1;
static const int kCscWireLayerMask     = 3;
static const int kCscWireLayerShift    = 9;
static const int kCscWireLayerMin      = 1;
static const int kCscMeasuresPhiMask   = 1;
static const int kCscMeasuresPhiShift  = 8;
static const int kCscMeasuresPhiMin    = 0;
static const int kCscStripMask         = 255;
static const int kCscStripShift        = 0;
static const int kCscStripMin          = 1;

// BOE has offline name BOL, hence no BOE listed here
// BMG will be installed in 2017
static const int maxstations = 36;
static const char mdtstations[maxstations][4] = { "XXX",
  "BIL", "BIS", "BML", "BMS", "BOL", "BOS", "BEE", "BIR", "BMF", "BOF", "BOG", "BME", "BIM", 
  "EIC", "EIL", "EEL", "EES", "EMC", "EML", "EMS", "EOC", "EOL", "EOS", "EIS", 
  "T1F", "T1E", "T2F", "T2E", "T3F", "T3E", "T4F", "T4E", "CSS", "CSL", "BMG" };

/*****************************************************************
*  List of general routines
*    mfid == Muon Fixed ID
******************************************************************/
int StationID(int mfid);               //returns station ID  [-1,1..34]
int StationEta(int mfid);              //returns station eta [-8..8]
int StationPhi(int mfid);              //returns station phi [1..9]
int SetStationID( int id, int mfid );  //returns mfid with station id set
int SetStationEta( int id, int mfid ); //returns mfid with eta set
int SetStationPhi( int id, int mfid ); //returns mfid with phi set
const char *ChamberType(int station);  //returns 3 letter station type (e.g. "BIL")
int IChamberType( char *station );     //returns integer type given 3 letter station type
int CompareChambers( const char *cham1, const char *cham2 ); //String-based comparison
int CompChambers( int mfid1, int mfid2 ); //Return 0 if (MDT) chambers are the same
int TechID(int mfid);    //return values: 0=MDT 1=CSC(?) 2=TGC 3=RPC  
int cTechID(int mfid, char *ctech);    //return name of technology: values: 0=MDT 1=CSC(?) 2=TGC 3=RPC  
//Utility routines for calib ntuple data
int segment_author( int iauthor, char *cauthor ); //write segment author
int track_author( int iauthor, char *cauthor );   //Write track author
int track_hittype( int itype, char *ctype );      //Write track hit type 

/*****************************************************************
*  List of MDT routines
*    mfid == Muon Fixed ID
******************************************************************/
void StationName(int mfid, char *c);   //writes name of station (works for MDT/TGC/RPC/CSC)
void ChamberName(int mfid, char *c);   //writes MDT chambername on c (e.g BML3A13)
void TubeName(int mfid, char *c);      //writes MDT tube name: chamber-ml-l-t on c
std::string TubeName(int mfid);
int MdtML(int mfid);                   //returns MDT multilayer
int MdtLayer(int mfid);                //returns MDT layer
int MdtTube(int mfid);                 //returns MDT tube number
int SetMdtML( int ml, int mfid );      //returns mfid with MDT multilayer set
int SetMdtLayer( int l, int mfid );    //returns mfid with MDT layer set
int SetMdtTube( int t, int mfid );     //returns mfid with MDT tube set
//int SetMdtID( char *chamber, int ml=1, int ly=1, int tb=1 ); //returns mfid MDT chamber ml-layer-tube
int SetMdtID( char *chamber, int ml, int ly, int tb ); //returns mfid MDT chamber (hardware) ml-layer-tube 
int SetMdtID( char *station, int phi, int eta, int ml, int ly, int tb ); //returns mfid MDT chamber (hardware) ml-layer-tube 
/*****************************************************************
*  List of RPC routines
*    mfid == Muon Fixed ID
******************************************************************/
void RpcName(int mfid, char *c);      //writes RPC name (guess)
void RpcChamber(int mfid, char *c);   //writes RPC chamber (guess)
int rpcDoubletR(int mfid);      //returns RPC doublet R
int rpcDoubletZ(int mfid);      //returns RPC doublet Z
int rpcDoubletPhi(int mfid);    //returns RPC doublet Phi
int rpcGasGap(int mfid);        //returns RPC gas gap
int rpcMeasuresPhi(int mfid);   //returns RPC measures phi (?)
int rpcStrip(int mfid);         //returns RPC strip

int setRpcDoubletR( int num, int mfid );
int setRpcDoubletR( int num, int mfid );
int setRpcDoubletZ( int num, int mfid );
int setRpcDoubletPhi( int num, int mfid );
int setRpcGasGap( int num, int mfid );
int setRpcMeasuresPhi( int num, int mfid );
int setRpcStrip( int num, int mfid );

/*****************************************************************
*  List of TGC routines
*    mfid == Muon Fixed ID
******************************************************************/
void TgcName(int mfid, char *c);      //writes full TGC name (guess)
void TgcChamber(int mfid, char *c);   //writes TGC chamber name (guess)
int tgcGasGap(int mfid);
int tgcIsStrip(int mfid);
int tgcChannel(int mfid);
int setTgcGasGap( int num, int mfid );
int setTgcIsStrip( int num, int mfid );
int setTgcChannel( int num, int mfid );

/*****************************************************************
*  List of CSC routines
*    mfid == Muon Fixed ID
******************************************************************/
void CscName(int mfid, char *c);      //writes CSC name (guess)
int cscChamberLayer(int mfid);
int cscWireLayer(int mfid);
int cscMeasuresPhi(int mfid);
int cscStrip(int mfid);
int setCscChamberLayer( int num, int mfid );
int setCscWireLayer( int num, int mfid );
int setCscMeasuresPhi( int num, int mfid );
int setCscStrip( int num, int mfid );

/*****************************************************************
*  General routines
******************************************************************/
int TechID(int mfid) {
  return ((mfid >> kTechnologyShift) & kTechnologyMask) + kTechnologyMin;
}
int cTechID(int mfid, char *ctech) {
  static const char techname[4][4]={"MDT","CSC","TGC","RPC"};
  int techid=((mfid >> kTechnologyShift) & kTechnologyMask) + kTechnologyMin;
  if( techid >=0 && techid <= 3 ) {
    sprintf(ctech,"%s",techname[techid]);
    return 0;
  }
  sprintf(ctech,"%i",techid);
  return -1;
}
int StationID(int mfid) {
  return ((mfid >> kStationNameShift) & kStationNameMask) + kStationNameMin;
}
int StationEta(int mfid) {
  return ((mfid >> kStationEtaShift) & kStationEtaMask) + kStationEtaMin;
}
//  This returns the PHI as defined in Athena (phi=1 is at top of detector?)
int StationPhi(int mfid) {
  return ((mfid >> kStationPhiShift) & kStationPhiMask) + kStationPhiMin;
}
//  This returns the PHI as defined in the TDR
//int StationPhi(int mfid) {
//  int iphi;
//  iphi = ((mfid >> kStationPhiShift) & kStationPhiMask) + kStationPhiMin + 4;
//  if( iphi > 16 ) iphi -= 16;
//  return iphi;
//}
const char *ChamberType(int station) {
  if(station > 0 && station < maxstations ) return mdtstations[station];
  return mdtstations[0];
}

int IChamberType( char *station ) {
  for( int i=1; i<maxstations; i++ ) {
    if( strcmp(station,mdtstations[i]) == 0 ) return i;
  }
  return -1;   //return error if station ID not found
}

int SetStationID( int id, int mfid ) {
  id -= kStationNameMin;
  if ( id > kStationNameMask ) {
    printf("Error SetStationID bad id=%i\n",id); 
    return mfid;
   }
   mfid &= ~(kStationNameMask << kStationNameShift);
   mfid |= (id & kStationNameMask) << kStationNameShift;
   return mfid;
}

int SetStationEta( int id, int mfid ) {
  id -= kStationEtaMin;
  if ( id > kStationEtaMask ) {
    printf("Error SetStationEta bad id=%i\n",id); 
    return mfid;
  }
  mfid &= ~(kStationEtaMask << kStationEtaShift);
  mfid |= (id & kStationEtaMask) << kStationEtaShift;
  return mfid;
}

int SetStationPhi( int id, int mfid ) {
  id -= kStationPhiMin;
  if ( id > kStationPhiMask ) {
    printf("Error SetStationPhi bad id =%i\n",id); 
    return mfid;
  }
  mfid &= ~(kStationPhiMask << kStationPhiShift);
  mfid |= (id & kStationPhiMask) << kStationPhiShift;
  return mfid;
}

/******************************************
*  Check if 2 chambers are the same
*  They need to be in the same case.
*  Really this is just strcmp all over again
******************************************/
int CompareChambers( const char *cham1, const char *cham2 ) {
  int i;
  for( i=0; cham1[i]!='\0'; i++ ) { 
    if( cham1[i] != cham2[i] ) return 0; 
  }
  return 1;
}

/******************************************
*  Check if 2 chambers are the same station, eta, phi
******************************************/
int CompChambers( int mfid1, int mfid2 ) {
  if( StationID(mfid1)  != StationID(mfid2)  ||
      StationEta(mfid1) != StationEta(mfid2) ||
      StationPhi(mfid1) != StationPhi(mfid2) ) return 0;
  return 1;
}
/*****************************************************************
*  MDT routines
******************************************************************/
//  Writes the ATLAS hardware (online) name for a given MuonFixedID
void ChamberName(int mfid, char *cname) {
  char cend, station[4];  
  int sid  = StationID(mfid);
  sprintf(station,"%s",ChamberType(sid));
  int iphi = StationPhi(mfid);
  int ieta = StationEta(mfid);
//  Convert from software phi to hardware phi
//  Software: both L, S chambers Phi = [1,8]
//  Hardware: Phi = [1,16] L,[BI]R,[BI]M=odd, S=even
  iphi = 2*iphi;
  if( station[2] == 'L' || station[2] == 'R' || station[2] == 'M' || 
      (station[0]=='B' &&  station[1]=='M' && station[2]=='E') ) {
    iphi -= 1;
  }
//  Convert eta to A/C ends
//  Is this valid for endcap chambers
  if( ieta < 0 ) { 
    ieta = -ieta;   //make positive
    cend = 'C';
  } else {
    cend = 'A';
  }
//  Fix irregularly named chambers
//  software EEL1x05 => hardware EEL2x05
  if( station[0]=='E' && station[1]=='E' && station[2]=='L' && iphi==5 ) {
    ieta = 2;
//  BOF is in eta 1,3,5,7   but these are numbered 1,2,3,4 in software
  } else if( station[0]=='B' && station[1]=='O' && station[2]=='F' ) {
    ieta = 2*ieta - 1;
//  BOG is in eta 0,2,4,6,8 but these are numbered 0,1,2,3,4 
  } else if( station[0]=='B' && station[1]=='O' && station[2]=='G' ) {
    ieta = 2*ieta;
// BML4A13 does not exist; BML5x13 has eta=4; BML6x13 has eta=5; 
  } else if( station[0]=='B' && station[1]=='M' && station[2]=='L' && ieta>3 && iphi==13 ) {
    ieta += 1;
// software BME1x13 is called BME4x13 in hardware 
  } else if( station[0]=='B' && station[1]=='M' && station[2]=='E' && ieta==1 && iphi==13 ) {
    ieta = 4;
// software BOL7x13 is called BOE3x13 in hardware 
  } else if( station[0]=='B' && station[1]=='O' && station[2]=='L' && ieta==7 && iphi==13 ) {
    station[2] = 'E';
    ieta = 3;
  }

  sprintf(cname,"%s%i%c%2.2i",station,ieta,cend,iphi);
  return;
}

//  Writes the ATLAS chamber name-ML-LAYER-TUBE for a given MuonFixedID
void TubeName(int mfid, char *fcname) {
  char cname[8];  
  ChamberName( mfid, cname );
  int ml    = MdtML(mfid);
  int layer = MdtLayer(mfid);
  int tube  = MdtTube(mfid);
  sprintf(fcname,"%s-%i-%i-%02i",cname,ml,layer,tube);
  return;
}

std::string TubeName(int mfid) {
  char fcname[20];
  TubeName(mfid, fcname);
  std::string s_ret = fcname;
  return s_ret;
}

int MdtML(int mfid) {
  return ((mfid >> kMdtMultilayerShift) & kMdtMultilayerMask) + kMdtMultilayerMin;
}
int MdtLayer(int mfid) {
   return ((mfid >> kMdtTubeLayerShift) & kMdtTubeLayerMask) + kMdtTubeLayerMin;
}
int MdtTube(int mfid) {
  return ((mfid >> kMdtTubeShift) & kMdtTubeMask) + kMdtTubeMin;
}

int SetMdtML( int ml, int mfid ) {
  ml -= kMdtMultilayerMin;
  if ( ml > kMdtMultilayerMask ) {
    printf("Error SetMdtML bad ML number=%i\n",ml); 
    return mfid;
  }
  mfid &= ~(kMdtMultilayerMask << kMdtMultilayerShift);
  mfid |= (ml & kMdtMultilayerMask) << kMdtMultilayerShift;
  return mfid;
}

int SetMdtLayer( int l, int mfid ) {
  l -= kMdtTubeLayerMin;
  if ( l > kMdtTubeLayerMask ) {
    printf("Error SetMdtLayer bad layer number=%i\n",l); 
    return mfid;
  }
  mfid &= ~(kMdtTubeLayerMask << kMdtTubeLayerShift);
  mfid |= (l & kMdtTubeLayerMask) << kMdtTubeLayerShift;
  return mfid;
}

int SetMdtTube( int t, int mfid ) {
  t -= kMdtTubeMin;
  if ( t > kMdtTubeMask ) {
    printf("Error SetMdtTube bad tube number=%i\n",t); 
    return mfid;
  }
  mfid &= ~(kMdtTubeMask << kMdtTubeShift);
  mfid |= (t & kMdtTubeMask) << kMdtTubeShift;
  return mfid;
}

//  Ed's routine to set the entire MDT fixed ID
//  This routine does not check if the chamber actually exists, however.
int SetMdtID( char *chamber, int ml, int ly, int tb) {
  int mfid=kUnusedBits;    //set starting number to value of unused bits
  char station[4]="EML",chphi[3],cheta[2];
  int i,iphi,ieta;
  
  for( i=0; i<3; i++ ) station[i] = chamber[i];
  if( chamber[5] == '0' ) sprintf(chphi,"%c",chamber[6]);
  else                    sprintf(chphi,"%c%c",chamber[5],chamber[6]);
  if( chamber[2]=='L' || chamber[2]=='l' ||
      chamber[2]=='M' || chamber[2]=='m' ||
      chamber[2]=='E' || chamber[2]=='e' ||
      chamber[2]=='R' || chamber[2]=='r' ) iphi = (atoi(chphi) + 1)/2;
  else iphi = atoi(chphi)/2;
  sprintf(cheta,"%c",chamber[3]);
  ieta = atoi(cheta);
//  Fix irregularly named chambers
//  software EEL1x05 => hardware EEL2x05
  if( station[0]=='E' && station[1]=='E' && station[2]=='L' && iphi==3 ) {
    ieta = 1;
//  BOF is in eta 1,3,5,7   but these are numbered 1,2,3,4 in software
  } else if( station[0]=='B' && station[1]=='O' && station[2]=='F' ) {
    ieta = (ieta + 1)/2;
//  BOG is in eta 0,2,4,6,8 but these are numbered 0,1,2,3,4 
  } else if( station[0]=='B' && station[1]=='O' && station[2]=='G' ) {
    ieta = ieta/2;
// BML4x13 does not exist; BML5x13 has eta=4; BML6x13 has eta=5; 
  } else if( station[0]=='B' && station[1]=='M' && station[2]=='L' && ieta>3 && iphi==7 ) {
    ieta -= 1;
// software BME1x13 is called BME4x13 in hardware 
  } else if( station[0]=='B' && station[1]=='M' && station[2]=='E' && ieta==4 && iphi==7 ) {
    ieta = 1;
// software BOL7x13 is called BOE3x13 in hardware 
  } else if( station[0]=='B' && station[1]=='O' && station[2]=='E' && ieta==3 && iphi==7 ) {
    station[2] = 'L';
    ieta = 7;
  }
  if(chamber[4]=='C' || chamber[4]=='c') ieta = -ieta;
    
  mfid = SetStationID( IChamberType(station), mfid );
  mfid = SetStationEta( ieta, mfid );
  mfid = SetStationPhi( iphi, mfid );
  mfid = SetMdtML( ml, mfid );
  mfid = SetMdtLayer( ly, mfid );
  mfid = SetMdtTube( tb, mfid );
  return mfid;
}

// Version which sets mfid from basic MDT info
int SetMdtID( char *station, int iphi, int ieta, int ml, int ly, int tb ) {
  int mfid=kUnusedBits;    //set starting number to value of unused bits
  mfid = SetStationID( IChamberType(station), mfid );
  mfid = SetStationEta( ieta, mfid );
  mfid = SetStationPhi( iphi, mfid );
  mfid = SetMdtML( ml, mfid );
  mfid = SetMdtLayer( ly, mfid );
  mfid = SetMdtTube( tb, mfid );
  return mfid;
}

/*****************************************************************
*  RPC routines
******************************************************************/
//  Writes the full RPC chamber name for a given MuonFixedID
void RpcName(int mfid, char *cname) {
  char cend, station[4];  
  int sid = StationID(mfid);
  sprintf(station,"%s",ChamberType(sid));
  int igap     = rpcGasGap(mfid);
  int istrip   = rpcStrip(mfid);
  int iphi = StationPhi(mfid);
  int ieta = StationEta(mfid);
//  Convert from software phi to hardware phi
  if( station[2] == 'L' ) iphi = 2*iphi - 1;
  else                  iphi = 2*iphi;
//  Convert eta to A/C ends
//  Will need to add code for endcap chambers
  if( ieta < 0 ) { 
    ieta = -ieta;
    cend = 'C';
  } else {
    cend = 'A';
  }

//  Fix irregularly named chambers
//  BOF is in eta 1,3,5,7   but these are numbered 1,2,3,4 in software
  if( station[0]=='B' && station[1]=='O' && station[2]=='F' ) {
    ieta = 2*ieta - 1;
//  BOG is in eta 0,2,4,6,8 but these are numbered 0,1,2,3,4 
  } else if( station[0]=='B' && station[1]=='O' && station[2]=='G' ) {
    ieta = 2*ieta;
// BML4A13 does not exist; BML5x13 has eta=4; BML6x13 has eta=5; 
  } else if( station[0]=='B' && station[1]=='M' && station[2]=='L' && ieta>3 && iphi==13 ) {
    ieta += 1;
// software BME1x13 is called BME4x13 in hardware 
  } else if( station[0]=='B' && station[1]=='M' && station[2]=='E' && ieta==1 && iphi==13 ) {
    ieta = 4;
// software BOL7x13 is called BOE3x13 in hardware 
  } else if( station[0]=='B' && station[1]=='O' && station[2]=='L' && ieta==7 && iphi==13 ) {
    station[2] = 'E';
    ieta = 3;
  }

  sprintf(cname,"%s%i%c%2.2i-%i-%i",station,ieta,cend,iphi,igap,istrip);
  return;
}
//  Writes the RPC chamber name for a given MuonFixedID
void RpcChamber(int mfid, char *cname) {
  char cend, station[4];  
  int sid = StationID(mfid);
  sprintf(station,"%s",ChamberType(sid));
  int iphi = StationPhi(mfid);
  int ieta = StationEta(mfid);
//  Convert from software phi to hardware phi
  if( station[2] == 'L' ) iphi = 2*iphi - 1;
  else                    iphi = 2*iphi;
//  Convert eta to A/C ends
  if( ieta < 0 ) { 
    ieta = -ieta;
    cend = 'C';
  } else {
    cend = 'A';
  }
//  Fix irregularly named chambers
//  BOF is in eta 1,3,5,7   but these are numbered 1,2,3,4 in software
  if( station[0]=='B' && station[1]=='O' && station[2]=='F' ) {
    ieta = 2*ieta - 1;
//  BOG is in eta 0,2,4,6,8 but these are numbered 0,1,2,3,4 
  } else if( station[0]=='B' && station[1]=='O' && station[2]=='G' ) {
    ieta = 2*ieta;
// BML4A13 does not exist; BML5x13 has eta=4; BML6x13 has eta=5; 
  } else if( station[0]=='B' && station[1]=='M' && station[2]=='L' && ieta>3 && iphi==13 ) {
    ieta += 1;
// software BME1x13 is called BME4x13 in hardware 
  } else if( station[0]=='B' && station[1]=='M' && station[2]=='E' && ieta==1 && iphi==13 ) {
    ieta = 4;
// software BOL7x13 is called BOE3x13 in hardware 
  } else if( station[0]=='B' && station[1]=='O' && station[2]=='L' && ieta==7 && iphi==13 ) {
    station[2] = 'E';
    ieta = 3;
  }
  sprintf(cname,"%s%i%c%2.2i",station,ieta,cend,iphi);
  return;
}

int rpcDoubletR(int mfid) {
  return ((mfid >> kRpcDoubletRShift) & kRpcDoubletRMask) + kRpcDoubletRMin;
}
int rpcDoubletZ(int mfid ) {
  return ((mfid >> kRpcDoubletZShift) & kRpcDoubletZMask) + kRpcDoubletZMin;
}
int rpcDoubletPhi(int mfid) {
  return ((mfid >> kRpcDoubletPhiShift) & kRpcDoubletPhiMask) + kRpcDoubletPhiMin;
}
int rpcGasGap(int mfid) {
   return ((mfid >> kRpcGasGapShift) & kRpcGasGapMask) + kRpcGasGapMin;
}
int rpcMeasuresPhi(int mfid) {
  return ((mfid >> kRpcMeasuresPhiShift) & kRpcMeasuresPhiMask) + kRpcMeasuresPhiMin;
}
int rpcStrip(int mfid) {
  return ((mfid >> kRpcStripShift) & kRpcStripMask) + kRpcStripMin;
}

int setRpcDoubletR( int num, int mfid ) {
  num -=  kRpcDoubletRMin;
  if( num > kRpcDoubletRMask ) {
    printf("Error setRpcDoubletR bad number =%i\n",num); 
    return mfid;
  }
  mfid &= ~( kRpcDoubletRMask <<  kRpcDoubletRShift );
  mfid |= (num &  kRpcDoubletRMask) << kRpcDoubletRShift;
  return mfid;
}

int setRpcDoubletZ( int num, int mfid ) {
  num -=  kRpcDoubletZMin;
  if ( num > kRpcDoubletZMask ) {
    printf("Error setRpcDoubletZ bad number =%i\n",num); 
    return mfid;
  }
  mfid &= ~( kRpcDoubletZMask <<  kRpcDoubletZShift );
  mfid |= (num &  kRpcDoubletZMask) << kRpcDoubletZShift;
  return mfid;
}

int setRpcDoubletPhi( int num, int mfid ) {
  num -= kRpcDoubletPhiMin;
  if ( num > kRpcDoubletPhiMask ) {
    printf("Error setRpcDoubletPhi bad number =%i\n",num); 
    return mfid;
  }
  mfid &= ~( kRpcDoubletPhiMask <<  kRpcDoubletPhiShift );
  mfid |= (num &  kRpcDoubletPhiMask) << kRpcDoubletPhiShift;
  return mfid;
}

int setRpcGasGap( int num, int mfid ) {
  num -= kRpcGasGapMin;
  if ( num > kRpcGasGapMask ) {
    printf("Error setRpcGasGap bad number =%i\n",num); 
    return mfid;
  }
  mfid &= ~( kRpcGasGapMask <<  kRpcGasGapShift );
  mfid |= (num &  kRpcGasGapMask) << kRpcGasGapShift;
  return mfid;
}

int setRpcMeasuresPhi( int num, int mfid ) {
  num -= kRpcMeasuresPhiMin;
  if ( num > kRpcMeasuresPhiMask ) {
    printf("Error setRpcGasGap bad number =%i\n",num); 
    return mfid;
  }
  mfid &= ~( kRpcMeasuresPhiMask <<  kRpcMeasuresPhiShift );
  mfid |= (num &  kRpcMeasuresPhiMask) << kRpcMeasuresPhiShift;
  return mfid;
}

int setRpcStrip( int num, int mfid ) {
  num -= kRpcStripMin;
  if ( num > kRpcStripMask ) {
    printf("Error setRpcStrip bad number =%i\n",num); 
    return mfid;
  }
  mfid &= ~( kRpcStripMask <<  kRpcStripShift );
  mfid |= (num &  kRpcStripMask) << kRpcStripShift;
  return mfid;
}

/*****************************************************************
*  TGC routines
******************************************************************/
//  Writes the full TGC chamber name for a given MuonFixedID
void TgcName(int mfid, char *cname) {
  char ctype[4];  
  int sid = StationID(mfid);
  sprintf(ctype,"%s",ChamberType(sid));
  int is   = tgcIsStrip(mfid);
  int ig   = tgcGasGap(mfid);
  int ic   = tgcChannel(mfid);
  int iphi = StationPhi(mfid);
  int ieta = StationEta(mfid);
  sprintf(cname,"%s%i%2.2i-%i-%i-%i",ctype,ieta,iphi,is,ig,ic);
  return;
}
//  Writes the TGC chamber name for a given MuonFixedID
void TgcChamber(int mfid, char *cname) {
  char ctype[4];  
  int sid = StationID(mfid);
  sprintf(ctype,"%s",ChamberType(sid));
  int iphi = StationPhi(mfid);
  int ieta = StationEta(mfid);
  sprintf(cname,"%s%i%2.2i",ctype,ieta,iphi);
  return;
}

int tgcGasGap(int mfid) {
  return ((mfid >> kTgcGasGapShift) & kTgcGasGapMask) + kTgcGasGapMin;
}
int tgcIsStrip(int mfid) {
  return ((mfid >> kTgcIsStripShift) & kTgcIsStripMask) + kTgcIsStripMin;
}
int tgcChannel(int mfid) {
  return ((mfid >> kTgcChannelShift) & kTgcChannelMask) + kTgcChannelMin;
}

int setTgcGasGap( int num, int mfid ) {
  num -= kTgcGasGapMin;
  if ( num > kTgcGasGapMask ) {
    printf("Error setTgcGasGap bad number =%i\n",num); 
    return mfid;
  }
  mfid &= ~( kTgcGasGapMask <<  kTgcGasGapShift );
  mfid |= (num &  kTgcGasGapMask) << kTgcGasGapShift;
  return mfid;
}

int setTgcIsStrip( int num, int mfid ) {
  num -= kTgcIsStripMin;
  if ( num > kTgcIsStripMask ) {
    printf("Error setTgcIsStrip bad number =%i\n",num); 
    return mfid;
  }
  mfid &= ~( kTgcIsStripMask <<  kTgcIsStripShift );
  mfid |= (num &  kTgcIsStripMask) << kTgcIsStripShift;
  return mfid;
}

int setTgcChannel( int num, int mfid ) {
  num -= kTgcChannelMin;
  if ( num > kTgcChannelMask ) {
    printf("Error setTgcChannel bad number =%i\n",num); 
    return mfid;
  }
  mfid &= ~( kTgcChannelMask <<  kTgcChannelShift );
  mfid |= (num &  kTgcChannelMask) << kTgcChannelShift;
  return mfid;
}

/*****************************************************************
*  CSC routines
******************************************************************/
//  Writes the CSC chamber name for a given MuonFixedID
void CscName(int mfid, char *cname) {
  char ctype[4];  
  int sid = StationID(mfid);
  sprintf(ctype,"%s",ChamberType(sid));
  int il   = cscChamberLayer(mfid);
  int iw   = cscWireLayer(mfid);
  int is   = cscStrip(mfid);
  int iphi = StationPhi(mfid);
  int ieta = StationEta(mfid);
  sprintf(cname,"%s%i%2.2i-%i-%i-%i",ctype,ieta,iphi,il,iw,is);
  return;
}
int cscChamberLayer(int mfid) {
  return ((mfid >> kCscChamberLayerShift) & kCscChamberLayerMask) + kCscChamberLayerMin;
}
int cscWireLayer(int mfid) {
  return ((mfid >> kCscWireLayerShift) & kCscWireLayerMask) + kCscWireLayerMin;
}
int cscMeasuresPhi(int mfid) {
  return ((mfid >> kCscMeasuresPhiShift) & kCscMeasuresPhiMask) + kCscMeasuresPhiMin;
}
int cscStrip(int mfid) {
  return ((mfid >> kCscStripShift) & kCscStripMask) + kCscStripMin;
}

int setCscChamberLayer( int num, int mfid ) {
  num -= kCscChamberLayerMin;
  if ( num > kCscChamberLayerMask ) {
    printf("Error setCscChamberLayer bad number =%i\n",num); 
    return mfid;
  }
  mfid &= ~( kCscChamberLayerMask <<  kCscChamberLayerShift );
  mfid |= (num &  kCscChamberLayerMask) << kCscChamberLayerShift;
  return mfid;
}

int setCscWireLayer( int num, int mfid ) {
  num -= kCscWireLayerMin;
  if ( num > kCscWireLayerMask ) {
    printf("Error setCscWireLayer bad number =%i\n",num); 
    return mfid;
  }
  mfid &= ~( kCscWireLayerMask <<  kCscWireLayerShift );
  mfid |= (num &  kCscWireLayerMask) << kCscWireLayerShift;
  return mfid;
}

int setCscMeasuresPhi( int num, int mfid ) {
  num -= kCscMeasuresPhiMin;
  if ( num > kCscMeasuresPhiMask ) {
    printf("Error setCscMeasuresPhi bad number =%i\n",num); 
    return mfid;
  }
  mfid &= ~( kCscMeasuresPhiMask <<  kCscMeasuresPhiShift );
  mfid |= (num &  kCscMeasuresPhiMask) << kCscMeasuresPhiShift;
  return mfid;
}

int setCscStrip( int num, int mfid ) {
  num -= kCscStripMin;
  if ( num > kCscStripMask ) {
    printf("Error setCscStrip bad number =%i",num); 
    return mfid;
  }
  mfid &= ~( kCscStripMask <<  kCscStripShift );
  mfid |= (num &  kCscStripMask) << kCscStripShift;
  return mfid;
}

/**************************************************************/
// Write character string for segment author in calib ntuple
/**************************************************************/
int segment_author( int iauthor, char *cauthor ) {
  if( iauthor==3 )      sprintf(cauthor,"Muonboy");
  else if( iauthor==4 ) sprintf(cauthor,"Moore");
  else                  sprintf(cauthor,"unknown_%i",iauthor);
  return 0;
}

/**************************************************************/
// Write character string for track author in calib ntuple
/**************************************************************/
int track_author( int iauthor, char *cauthor ) {
  if(      iauthor==0 ) sprintf(cauthor,"Moore");
  else if( iauthor==1 ) sprintf(cauthor,"BckExSAnoCal"); //BackExtrapolated SA track with MuonTrackThroughCalo without calorimeter energy correction
  else if( iauthor==2 ) sprintf(cauthor,"BckExSAwCal");  //BackExtrapolated SA track with MuonTrackThroughCalo with calorimeter energy correction
  else if( iauthor==10 ) sprintf(cauthor,"MuidSAExIP");  //MuidSA/ExtrapolateToIP (Moore track to vertex and refitted)
  else if( iauthor==20 ) sprintf(cauthor,"MuidCBComMu"); //MuidCB/CombinedMuon (combined inner detector - muon spectrometer track)
  else if( iauthor==25 ) sprintf(cauthor,"MuonCombinedFit");
  else if( iauthor==30 ) sprintf(cauthor,"MuTagIMO");
  else if( iauthor==35 ) sprintf(cauthor,"MuidMuGirl");
  else if( iauthor==40 ) sprintf(cauthor,"AODMuidCB");   //AOD MuidCB/CombinedMuon
  else if( iauthor==41 ) sprintf(cauthor,"AODMuidSAExIP");//MuidSA/ExtrapolateToIP
  else if( iauthor==42 ) sprintf(cauthor,"AODMuTagIMO");
  else if( iauthor==43 ) sprintf(cauthor,"AODMuGirl");
  else if( iauthor==100 ) sprintf(cauthor,"Muonboy");
  else if( iauthor==120 ) sprintf(cauthor,"Staco");
  else if( iauthor==130 ) sprintf(cauthor,"MuTag");
  else if( iauthor==140 ) sprintf(cauthor,"AODStacoMuonCB");
  else if( iauthor==141 ) sprintf(cauthor,"AODStacoMuonSA");
  else if( iauthor==142 ) sprintf(cauthor,"AODStacoMuonTag");
  else if( iauthor==1000 ) sprintf(cauthor,"IDtrack");   //Inner Detector track
  else sprintf(cauthor,"unknown_%i",iauthor); 
  return 0;
}
/**************************************************************/
// Write character string for track hit type in calib ntuple
/**************************************************************/
int track_hittype( int itype, char *ctype ) {
  if(      itype==-2 ) sprintf(ctype,"RIOcompetingRIO"); //RIO contained in the competing RIO
  else if( itype==1 ) sprintf(ctype,"HitOnTrack");
  else if( itype==2 ) sprintf(ctype,"CompeteTGCRPC"); //Competing RIO used for TGC and RPC
  else if( itype==3 ) sprintf(ctype,"PseudoHit");     //Pseudo measurement (needed to stabilize the fit)
  else if( itype==4 ) sprintf(ctype,"Outlier");
  else if( itype==5 ) sprintf(ctype,"DeltaRay");
  else if( itype==6 ) sprintf(ctype,"Hole");
  else if( itype==11 ) sprintf(ctype,"PhiScatCenter");   //Scattering centre in phi
  else if( itype==12 ) sprintf(ctype,"ThetaScatCenter"); //Scattering centre in theta 
  else sprintf(ctype,"unknown_%i",itype);
  return 0;
}

//  Write chamber name for any type of muonsystem detector.
void StationName(int mfid, char *c) {
  int tid = TechID(mfid);
  if( tid == 0 ) TubeName( mfid, c );
  else if( tid == 1 ) CscName( mfid, c );
  else if( tid == 2 ) TgcName( mfid, c );
  else if( tid == 3 ) TgcName( mfid, c );
  else sprintf(c,"unknown");
}
#endif
