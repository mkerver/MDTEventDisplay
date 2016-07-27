/**********************************************************
  selector_event.C  Edward Diehl 

  This version prints calib ntuple data to the screen.
  You must rename this to selector.C to make it run.

  This has several extra selector methods which must be put into 
  selector_event.h   If you add a method to print events here, 
  add the template to selector_event.h in the "#ifdef selector_event" section.
  Extra methods:
    selector::PrintHit => Print hit information
    selector::SegmentPrint => Print hits by segment
    selector::RawPrint => Print raw hits
    selector::DupPrint => Print tubes with multiple hits per event
  version  Date        Comment
   1.0     08-May-2007
   2.0     10-Oct-2007 Add T0, residual print-out
**********************************************************/
#define selector_event
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <ctype.h>
#include <TH2.h>
#include <TStyle.h>
#include <TFile.h>
#include <TMath.h>
#include "selector.h"
#include "/atlas/data18c/muoncal/root/macros/chamberlist.h"
#include "/atlas/data18c/muoncal/root/macros/rtfunction.h"

int nevents=0;
int ievent=-1;
int options=0;
//  Bit masks for options flag
const int PRINT_EVENT_DATA=0x1;     //print only event summary line
const int PRINT_RAW_DATA  =0x2;     //print all MDT hits (otherwise only segment hits)
const int PRINT_OLD_FORMAT=0x4;     //print old format (all hits)
const int PRINT_DUPLICATE =0x8;     //print only MDT duplicate tube hits
const int PRINT_NOPROMPT  =0x10;    //do not prompt at end of event
const int PRINT_ENDCAP    =0x20;    //print only events with endcap MDT hits
const int PRINT_SEGMENTS  =0x40;    //print only events with MDT segments
const int PRINT_TGC       =0x80;    //print TGC hits
const int PRINT_RPC       =0x100;   //print TGC hits
const int PRINT_CSC       =0x200;   //print TGC hits

//  Types for print statements
enum {EVENT,MDT,MDTOLD,MDTRAW,RPC,RPCRAW,TGC,TGCRAW,CSC,CSCRAW};

//  Possible segment authors
static const char *SegAuthors[] = { "0", "1", "2", "MuonBoy", "Moore" };

//  RT function ascii file names
int  nrtfiles=0;            //number of rt file names read in from rtlist.txt
char rtfiles[MAXCHAMBERS][255];
RtFunction *rt[MAXCHAMBERS];

/**********************************************************
*   Initialization
**********************************************************/
void selector::Begin(TTree *) {
  char copt[80];

  TString option = GetOption();
//  Print help message to users
  if( option == "h" ) {
    printf("selector_event: Print event information from calib ntuples\n");
    printf("    Edward Diehl  7/08/09\n");
    printf("   List of options - can concatenate several together like \"enr\"\n");
    printf("e => print only event summary line\n");
    printf("n => do not prompt at end of event\n");
    printf("r => print all MDT hits (otherwise only segment hits)\n");
    printf("E => print only events with endcap MDT hits\n");
    printf("S => print only events with MDT segments\n");
    printf("T => print TGC hits\n");
    printf("R => print RPC hits\n");
    printf("C => print CSC hits\n");
    printf("o => print old format (all hits)\n");
    printf("d => print only MDT duplicate tube hits\n");
    exit(0);    //Abort job
  }  
// Parse options
  options = PRINT_SEGMENTS;  //set default
  sprintf(copt,"%s",option.Data());
  for( int i=0; i<10; i++ ) {
    if( copt[i] == '\0') break;
    if( copt[i] == 'e' ) options |= PRINT_EVENT_DATA;
    if( copt[i] == 'o' ) options |= PRINT_OLD_FORMAT;
    if( copt[i] == 'd' ) options |= PRINT_DUPLICATE;
    if( copt[i] == 'r' ) options |= PRINT_RAW_DATA;
    if( copt[i] == 'n' ) options |= PRINT_NOPROMPT;
    if( copt[i] == 'E' ) options |= PRINT_ENDCAP;
    if( copt[i] == 'S' ) options |= PRINT_SEGMENTS;
    if( copt[i] == 'T' ) options |= PRINT_TGC;
    if( copt[i] == 'R' ) options |= PRINT_RPC;
    if( copt[i] == 'C' ) options |= PRINT_CSC;
  }

// Read in list of RT functions
  nrtfiles=0;
  ifstream fin( "rtlist.txt" );
  if( fin.is_open() == 1 )  {
    printf("Reading RTs from rtlist.txt\n");
  } else {
    printf("ERROR: No RT list file\n");
  }
    
//  Read in filenames from rtlist.txt
  while( fin>>rtfiles[nrtfiles] && !fin.eof() ) nrtfiles++;
  fin.close();
  printf("Read %i RT filenames from rtlist.txt\n",nrtfiles);

// Init rtfunctions
  for( int i=0; i<MAXCHAMBERS; i++ ) rt[i]=NULL; 

}  //end selector::Begin

/**********************************************************
*  Process each event
**********************************************************/
Bool_t selector::Process(Long64_t entry) {
  char cname[20];
  int i, id, print_event;
  int emdt_raw, emdt_seg;

//  Bookkeeping
  nevents++;

// Get event data
  fChain->GetTree()->GetEntry(entry);

  print_event = 0;
  emdt_raw = 0;
  emdt_seg = 0;

//  Loop over raw MDT hits - count endcap hits
  for( i=0; i<rawMdt_nRMdt; i++ ) {
    id = rawMdt_id[i];
    ChamberName(id,cname);
    if( cname[0] == 'E' ) emdt_raw++;
  }
//  Loop over segment MDT hits - count endcap hits
  for( i=0; i<mdt_nMdt; i++ ) {
    id = mdt_id[i];
    ChamberName(id,cname);
    if( cname[0] == 'E' ) emdt_seg++;
  }

//  Print one line event summary
  PrintHit(EVENT,emdt_seg,emdt_raw);

//  Decide whether to print event hits
  print_event = 0;
  if( options&PRINT_SEGMENTS && mdt_nMdt>0 ) print_event = 1;
  if( options&PRINT_RAW_DATA && rawMdt_nRMdt>0 ) print_event = 1;
  if( options&PRINT_ENDCAP ) {
    if( options&PRINT_SEGMENTS && emdt_raw>0 ) print_event = 1;
    if( options&PRINT_SEGMENTS && emdt_seg==0 ) print_event = 0;
  }
  if( options&PRINT_EVENT_DATA ) print_event = 0;
  if( event_eventNumber == ievent ) print_event = 1;
  else if( ievent != -1 ) return kTRUE;
    
  if( print_event ) {
    if( options&PRINT_DUPLICATE ) DupPrint();
    else if( options&PRINT_OLD_FORMAT ) RawPrint();
    else SegmentPrint();
  } 

  if( options&PRINT_NOPROMPT || print_event==0 ) return kTRUE;
//  Prompt the user to continue
  sprintf(cname,"n");
  cout << "\nHit n to continue, q to quit: ";
  cin >> cname;
  if( cname[0] == 'q' || cname[0] == 'Q' ) abort(); 
  if( isdigit(cname[0]) ) ievent = atoi(cname);
  else                    ievent = -1;
  return kTRUE;
}  //end selector::Process

/******************************************
*  Terminate program
******************************************/
void selector::Terminate() {
  printf("Processed %i events\n",nevents);
}   //end selector::Terminate

/******************************************
*  Print out hits by segment
******************************************/
void selector::SegmentPrint( void ) {
  char cname[20],cname1[20];
  int h, i, j, k, id=0;
  int mdt_segment_nhits[100],mdt_segment_hits[100][250];
  //  int rpc_segment_nhits[100],rpc_segment_hits[100][50];  //RPC data re-organized; obselete
  int tgc_segment_nhits[100],tgc_segment_hits[100][50];

//zero arrays
  for( i=0; i<100; i++ ) {
    mdt_segment_nhits[i] = 0;
    tgc_segment_nhits[i] = 0;
    //    rpc_segment_nhits[i] = 0;
  }

// Sort MDT hits by segment
  for( h=0; h<mdt_nMdt; h++ ) {
    i = mdt_segIndex[h];
    j = ++mdt_segment_nhits[i];
    mdt_segment_hits[i][j-1] = h;
    /*    if( j<50) {
      mdt_segment_hits[i][j-1] = h;
    } else {
      printf("1 Segment %i has %i MDT hits\n",i,j);
      PrintHit(MDT,h,0);
    }
    */    
  }

// 1/9/10 RPC data completely reorganized in ntuple.
// data no longer sorted by segment(?)
// Sort RPC hits by segment
//  for( h=0; h<rpc_nRpcHits; h++ ) {
//    i = rpc_segIndex[h];
//    j = ++rpc_segment_nhits[i];
//    rpc_segment_hits[i][j-1] = h;
//  }
// Sort TGC hits by segment
  for( h=0; h<tgc_nTgc; h++ ) {
    i = tgc_segIndex[h];
    j = ++tgc_segment_nhits[i];
    tgc_segment_hits[i][j-1] = h;
  }

//  Loop over segments and print hits
  for( i=0; i<seg_nSegments; i++ ) {
    char Author[20];
    if( seg_author[i] == 3 || seg_author[i] == 4 ) sprintf(Author,"%s", SegAuthors[seg_author[i]]); 
    else sprintf(Author,"AUTHOR=%i",seg_author[i]); 
    printf(" Event %i Seg %i %s nHit=%i nMDT=%i nRPC=%i nTGC=%i nCSC=%i nMDT=%i Chi2=%.4f T0fit=%.2lf\n",
	     event_eventNumber, i, Author, seg_nHits[i], seg_nMdtHits[i], seg_nRpcHits[i],
	     seg_nTgcHits[i], seg_nCscHits[i],mdt_segment_nhits[i],seg_chi2[i],seg_fittedT0[i]);

//  print MDT hits
//    for( h=0; h<seg_nMdtHits[i]; h++ ) {
    for( h=0; h<mdt_segment_nhits[i]; h++ ) {
      /*      if( h == 50 ) {
        printf("2 Segment %i has %i MDT hits\n",i,h);
        break;
	} */
      j  = mdt_segment_hits[i][h];      
      id = PrintHit(MDT,j,h+1);
//  Find this tube in the raw tube list and set id=0 so it is not
//  printed twice.   Also compare TDCs since tubes may have more than 1 hit
      for( k=0; k<rawMdt_nRMdt; k++ ) {
        if( id == rawMdt_id[k] && mdt_tdc[j] == rawMdt_tdc[k] ) {
	  rawMdt_id[k] = 0; break;
	}
      }
    } 
//  Print raw hits which are in the current chamber
//  Use last chamber for hit in segments; assume all are in same chamber.
//  Put all raw hits in the chamber with the first segment found in that chamber.
//  Set rawID=0 as flag to indicate that raw hit is already printed.
    if( seg_nMdtHits[i] ) {
      ChamberName(id,cname);
      for( k=0; k<rawMdt_nRMdt; k++ ) {
        id = rawMdt_id[k];
        if( id != 0 ) {
          ChamberName(id,cname1);
          if( strcmp(cname,cname1) == 0 ) {
            PrintHit(MDTRAW,k,0);
            rawMdt_id[k] = 0;  //set to zero so not printed twice
          }
        }
      }
    }
//  Print RPC hits
//    if( options&PRINT_RPC ) {
//      for( h=0; h<seg_nRpcHits[i]; h++ ) {
//        j  = rpc_segment_hits[i][h];
//        id = PrintHit(RPC,j,h+1);
//  Find hit in raw list and set id=0 so it is not printed twice.   
//        for( k=0; k<rawRpc_nRRpc; k++ ) {
//         if( id == rawRpc_id[k] ) {
//	    rawRpc_id[k] = 0; break;
//    	  }
//	}
//      }
//    }
//  Print raw hits which are in the current chamber
//  Use last chamber for hit in segments; assume all are in same chamber.
//  Put all raw hits in the chamber with the first segment found in that chamber.
//  Set rawID=0 as flag to indicate that raw hit is already printed.
    if( seg_nRpcHits[i] && (options&PRINT_RPC) ) {
      RpcChamber(id,cname);
      for( k=0; k<rawRpc_nRRpc; k++ ) {
        id = rawRpc_id[k];
        if( id != 0 ) {
          RpcChamber(id,cname1);
          if( strcmp(cname,cname1) == 0 ) {
            PrintHit(RPCRAW,k,0);
            rawRpc_id[k] = 0;  //set to zero so not printed twice
          }
        }
      }
    }
//  Print TGC hits
    if( options&PRINT_TGC ) {
      for( h=0; h<seg_nTgcHits[i]; h++ ) {
        j  = tgc_segment_hits[i][h];
        id = PrintHit(TGC,j,h+1);
//  Find hit in raw list and set id=0 so it is not printed twice.   
        for( k=0; k<rawTgc_nRTgc; k++ ) {
          if( id == rawTgc_id[k] ) {
	    rawTgc_id[k] = 0; break;
  	  }
	}
      }
    }
//  Use last chamber for hit in segments; assume all are in same chamber.
//  Put all raw hits in the chamber with the first segment found in that chamber.
//  Set rawID=0 as flag to indicate that raw hit is already printed.
    if( seg_nTgcHits[i] && (options&PRINT_TGC) ) {
      TgcChamber(id,cname);
      for( k=0; k<rawTgc_nRTgc; k++ ) {
        id = rawTgc_id[k];
        if( id ) {
          TgcChamber(id,cname1);
          if( strcmp(cname,cname1) == 0 ) {
            PrintHit(TGCRAW,k,0);  
            rawTgc_id[k] = 0;  //set to zero so not printed twice
          }
        }
      }
    }
  }  //end loop over segments

//  Loop over raw MDT hits and print any that have not already been printed
  if( options&PRINT_RAW_DATA || event_eventNumber == ievent ) {
    printf(" Event %i Raw Hits not in segments\n",event_eventNumber);
    for( i=0, j=0; i<rawMdt_nRMdt; i++ ) {
//  Check if this hit has already been printed
      if( rawMdt_id[i] ) PrintHit(MDTRAW,i,++j);
    }
//  Print RPC raw hits
    if( options&PRINT_RPC ) {
      for( i=0, j=0; i<rawRpc_nRRpc; i++ ) {
        if( rawRpc_id[i] ) PrintHit(RPCRAW,i,++j);
      }
    }
//  Print TGC raw hits
    if( options&PRINT_TGC ) {
      for( i=0, j=0; i<rawTgc_nRTgc; i++ ) {
        if( rawTgc_id[i] ) PrintHit(TGCRAW,i,++j);
      }
    }
  } //if print RAW hits
}  //end selector::SegmentPrint

/******************************************
*  Print out raw hits 
******************************************/
void selector::RawPrint( void ) {
  char cname[20];
  int i, j, id;

//  Print truth info if it exists
  if( truth_PDGCode[1] != 0 ) {
    printf(" SIM Part=%i E=%.3lf P=(%.3lf,%.3lf,%.3lf)\n", 
         truth_PDGCode[1], truth_kinEnergy[1]/1000., 
         truth_pX[1]/1000., truth_pY[1]/1000., truth_pZ[1]/1000. );
  } 
  printf("  Detector Truth  Raw  Seg nPatterns=%i nSegments=%i\n",pat_nPatterns,seg_nSegments);
  printf("    MDT     %4i %4i %4i\n", mdtTruth_nMdtTruthHit, rawMdt_nRMdt, mdt_nMdt );
  printf("    TGC     %4i %4i %4i\n", tgcTruth_nTgcTruthHit, rawTgc_nRTgc, tgc_nTgc );
  printf("    CSC     %4i %4i %4i\n", cscTruth_nCscTruthHit, rawCsc_nRCsc, csc_nCsc );
  printf("    RPC     %4i %4i %4i\n", rpcTruth_nRpcTruthHit, rawRpc_nRRpc, rpc_fchan );

//  Loop over segments
  for( i=0; i<seg_nSegments; i++ ) {
    if(i==0) printf(" Seg Auth nHit nMDT nRPC nTGC nCSC\n");
    printf(" %3i  %3i  %3i  %3i  %3i  %3i  %3i\n",i+1, seg_author[i], seg_nHits[i], 
            seg_nMdtHits[i], seg_nRpcHits[i], seg_nTgcHits[i], seg_nCscHits[i]);
  }

//  Loop over data raw MDT hits
  for( i=0; i<rawMdt_nRMdt; i++ ) {
    if(i==0) printf(" MDT Chamber-ML-L-T ADC  TDC  T0 SEG    t    r     rTrk    dr drTrk resid\n");
    id = rawMdt_id[i];
    TubeName(id,cname);
    printf("  MDT %2i %-14s %3i %4i",i+1, cname, rawMdt_adc[i], rawMdt_tdc[i]);
//  Check if this is also a segment hit - check both id and tdc since can have 
//  more than one hit per tube.
    for( j=0; j<mdt_nMdt; j++ ) {
      if( id == mdt_id[j] && mdt_tdc[j] == rawMdt_tdc[i] ) {
        printf(" %4.0f %3i %-6.1f %-5.2f %-5.2f %-5.2f %-5.2f %5.2f",
        mdt_tube_t0[j],mdt_segIndex[j],mdt_t[j],mdt_r[j],mdt_rTrk[j],
        1000.*mdt_dr[j],1000.*mdt_drTrk[j],1000.*mdt_resi[j]);
        if( mdt_segIndex[j] == 99 ) printf(" XXX"); 
        break;
      }
    } 
    printf("\n");
  }
//  Check for segment hits which are not raw hits (not sure if this will ever happen)
  for( j=0; j<mdt_nMdt; j++ ) {
    id = mdt_id[j];
    for( i=0; i<rawMdt_nRMdt; i++ ) {
      if( id == rawMdt_id[i] ) {
        i = -1;
        break;
      }
    }
    if( i != -1 ) {
      TubeName(id,cname);
      printf("  MDT %2i %-14s %4i %4i SEG NOT RAW!",j+1, cname, mdt_tdc[j], mdt_adc[j]);
    }
  }

//  Loop over data raw TGC hits
  for( i=0; i<rawTgc_nRTgc; i++ ) {
    if(i==0) printf(" TGC hit Chamber       occ width   length \n");
    id = rawTgc_id[i];
    TgcName(id,cname);
    printf("  TGC %2i %-13s %2i %6.1lf %8.1lf",i+1, 
                cname, rawTgc_occupancy[i], rawTgc_width[i], rawTgc_length[i]);
//  Check if this is also a segment hit
    for( j=0; j<tgc_nTgc; j++ ) {
      if( id == tgc_id[j] ) {
        printf(" SEG");
        break;
      }
    } 
    printf("\n");
  }

/*  Loop over data raw CSC hits
  for( i=0; i<rawCsc_nRCsc; i++ ) {
    if(i==0) printf(" CSC hit Chamber             t   width   charge\n");
    id = rawCsc_id[i];
    CscName(id,cname);
    printf("  CSC %2i %-15s %6.1lf %6.1lf %8.1lf",i+1, 
              cname, rawCsc_t[i], rawCsc_width[i], rawCsc_charge[i]);
//  Check if this is also a segment hit
    for( j=0; j<csc_nCsc; j++ ) {
      if( id == csc_id[j] ) {
        printf(" SEG");
        break;
      }
    } 
    printf("\n");
  }
*/
//  Loop over data raw RPC hits
  for( i=0; i<rawRpc_nRRpc; i++ ) {
    if(i==0) printf("RPC  hit Chamber-G-S    t   width   length \n");
    id = rawRpc_id[i];
    RpcName(id,cname);
    printf("  RPC %2i %-12s %6.1lf %6.1lf %8.1lf",i+1, 
              cname, rawRpc_t[i], rawRpc_width[i], rawRpc_length[i]);
//  Check if this is also a segment hit
    for( j=0; j<rpc_fchan; j++ ) {
      if( id == rpc_padId[j] ) {
        printf(" SEG");
        break;
      }
    } 
    printf("\n");
  }
}  //end selector::RawPrint

/******************************************
*  Print out duplicate MDT tube hits 
******************************************/
void selector::DupPrint( void ) {
  int i,k,id,ifound;  
  char cname[20];

//  Loop over data raw MDT hits
  ifound = 0;
  for( i=0; i<rawMdt_nRMdt; i++ ) {
    id = rawMdt_id[i];
    for( k=i+1; k<rawMdt_nRMdt; k++ ) {
      if( id == rawMdt_id[k] ) {
        ifound++;
        if(ifound == 1) {
          printf("  DUP Chamber-ML-L-T   ID     ADC  TDC  deltaTDC\n");
        }
//  Print i'th hit
        TubeName(id,cname);
        printf("  %3i %-14s %-8x %3i %4i %4i\n", 
               ifound, cname, id, rawMdt_adc[i], rawMdt_tdc[i], 
               rawMdt_tdc[k]-rawMdt_tdc[i]);
//  Print k'th hit
        id = rawMdt_id[k];
        TubeName(id,cname);
        printf("      %-14s %-8x %3i %4i\n", 
               cname, id, rawMdt_adc[k], rawMdt_tdc[k]);
      }
    }

  }
  if( ifound ) printf("     Run %i Evt %i Found %i duplicates in %i MDT hits\n",
    event_runNumber,event_eventNumber,ifound,mdt_nMdt);
}  //end selector::DupPrint

/******************************************
*  Print out hit data 
*  Have one place for print statements to avoid upkeep
*  itype = type of data to print
*  i     = which hit to print 
*  j     = flag whether to print header line (1=print)
*  (except for itype=EVENT i,j = endcap seg,raw hits) 
******************************************/
int selector::PrintHit( int itype, int i, int j ) {
  int id=0,idx=0;
  char cname[20];
  char bmess[80]="";
  Float_t t,rcheck;

  switch (itype) {
  case EVENT:
    printf("***** Run %6i Event %6i Pat=%i Seg=%i BMDT=%i/%i EMDT=%i/%i RPC=%i/%i TGC=%i/%i",
	 event_runNumber,event_eventNumber,pat_nPatterns,seg_nSegments,
	 (mdt_nMdt-i),(rawMdt_nRMdt-j),i,j,
	 rpc_fchan,rawRpc_nRRpc,tgc_nTgc,rawTgc_nRTgc);

    /*  print eventTag in the vain hope of seeing the trigger bits
    for( i=0; i<64; i++ ) {
      if( event_eventTag[i] == 0 ) break;
      if( isalpha(event_eventTag[i]) ) printf("%c-%i",event_eventTag[i],i);    
      else 
      printf("%x",event_eventTag[i]);
    }
     */
    printf("\n");  
    break;
//  Rename this to MDT (and vice versa) to use this format
  case MDTOLD:
     if(j==1) printf("  MDT Chamber-ML-L-T ADC  TDC  T0    t    r     rTrk    dr drTrk resid\n");
     id = mdt_id[i];
     TubeName(id,cname);
     printf("  MDT %-14s %3i %4i %3.0f %4.0f %6.2f %6.2f %5.0f %5.0f %5.0f\n",
 	  cname, mdt_adc[i], mdt_tdc[i], mdt_tube_t0[i],
       mdt_t[i],mdt_r[i],mdt_rTrk[i],
       1000.*mdt_dr[i],1000.*mdt_drTrk[i],1000.*mdt_resi[i]);
//     if(j==1) printf("  MDT Chamber-ML-L-T ADC  TDC  T0    t    r     rTrk NTresi resi |resi|\n");
//     id = mdt_id[i];
//     TubeName(id,cname);
//     printf("  MDT %-14s %3i %4i %3.0f %4.0f %6.2f %6.2f %5.0f %5.0f %5.0f\n",
// 	   cname, mdt_adc[i], mdt_tdc[i], mdt_tube_t0[i],
// 	   mdt_t[i],mdt_r[i],mdt_rTrk[i],
// 	   1000.*mdt_resi[i],
// 	   1000.*(mdt_r[i]-mdt_rTrk[i]),
// 	   1000.*(TMath::Abs(mdt_r[i])-TMath::Abs(mdt_rTrk[i])) );
    break;
//  Rename this to MDT (and vice versa) to use this format
  case MDT:
    //    if(j==1) printf("  MDT Chamber-ML-L-T ADC TDC   T0    t     r      resi   TOF   tProp  tMag  tSlew  tSag  tTemp DistR0  Bperp  Bpara  ProjSag\n");
    if(j==1) printf("  MDT Chamber-ML-L-T ADC TDC   T0    t   tcheck   r    rcheck    resi   TOF  tProp tMag tSlew  Xpos    Bperp  Bpara  ProjSag\n");
    id = mdt_id[i];
    ChamberName(id,cname);
    idx = MDTindex(cname);
// Load rt function for each chamber if not already loaded
    rcheck = 999.;
    if( rt[idx] == NULL ) {
      MDTcalname (cname, idx );   
      for( int ff=0; ff<nrtfiles; ff++ ) {       
	if( strstr(rtfiles[ff],cname) != NULL ) { 
	  rt[idx] = new RtFunction(rtfiles[ff]);
	  rcheck = rt[idx]->GetRadius(mdt_t[i]);
	  break;      
	}
      }
    } else { 
      rcheck = rt[idx]->GetRadius(mdt_t[i]);
    }
    t  = mdt_tdc[i]*0.78125;
    t -= mdt_tube_t0[i];
    t -= mdt_tof[i];
    t -= mdt_propTime[i];
    t += mdt_lorTime[i];       //Add the Bfield correction!
    t -= mdt_slewTime[i];
    //   t -= mdt_wire_sag_time[i];
    //   t -= mdt_temperature_time[i];
// Figure out if Bfield correction has been added or subtracted
    bmess[0]='\0';
    if( mdt_lorTime[i] != 0 ) {
      if( TMath::Abs(mdt_t[i]-t) < 0.1 ) {
	sprintf(bmess,"+tMag");
      } else {
	t -= 2*mdt_lorTime[i];       //subtract the Bfield correction
	sprintf(bmess,"-tMag");
      }
    }
    TubeName(id,cname);
    printf("  MDT %-14s %3i %4i %5.1f %5.1f %5.1f %6.2f %6.2f %7.1f %5.1f %5.1f %5.1f %5.1f %6.2f %.1e %.1e %s",
	   cname, mdt_adc[i], mdt_tdc[i], mdt_tube_t0[i], mdt_t[i], t, mdt_r[i], rcheck, 1000.*mdt_resi[i],
	   mdt_tof[i], mdt_propTime[i], mdt_lorTime[i], mdt_slewTime[i], 
           mdt_posX[i],mdt_bFieldPerp[i],mdt_bFieldPara[i],bmess);
    if( TMath::Abs(mdt_t[i]-t) > 0.1 ) printf(" time error! (dt=%.1f)\n",mdt_t[i]-t);
    else printf("\n");
    break;
  case MDTRAW:
//     if(j==1) printf("  MDT Chamber-ML-L-T ADC  TDC  T0    t    r             dr\n");
//     id = rawMdt_id[i];
//     TubeName(id,cname);
//     printf("  MDT %-14s %3i %4i     %4.0f %6.2f        %5.0f\n",
//            cname, rawMdt_adc[i], rawMdt_tdc[i],
//            rawMdt_t[i],rawMdt_r[i],1000.*rawMdt_dr[i]);
    if(j==1) printf("  MDT Chamber-ML-L-T ADC  TDC  T0    t    r\n");
    id = rawMdt_id[i];
    TubeName(id,cname);
    printf("  MDT %-14s %3i %4i       %4.0f  %6.2f\n",
           cname, rawMdt_adc[i], rawMdt_tdc[i], rawMdt_t[i],rawMdt_r[i]);
    break;
  case RPC:
    //    if(j==1) printf("  RPC  Chamber-G-S strip width  length  time\n");
    //    id = rpc_padId[i];
    //    RpcName(id,cname);
    //    printf("  RPC %-12s %3i %6.1lf %8.1lf %6.1lf\n",
    //      cname,rpc_nStrips[i], rpc_stripWidth[i],
    //      rpc_stripLength[i], rpc_time[i]);
    break;
  case RPCRAW:
    if(j==1) printf("  RPC  Chamber-G-S  occ  width  length\n");
    id = rawRpc_id[i];
    RpcName(id,cname);
    printf("  RPC %-12s %2i %6.1lf %8.1lf %6.1lf RAW\n",
      cname,rawRpc_occupancy[i], rawRpc_width[i],
      rawRpc_length[i], rawRpc_t[i]);
    break;
  case TGC:
    if(j==1) printf(" TGC hit Chamber Strips width   length error\n");
    id = tgc_id[i];
    TgcName(id,cname);
    printf("  TGC %-13s %2i %6.1lf %8.1lf %8.1lf\n",
	   cname, tgc_nStrips[i], tgc_stripWidth[i],
           tgc_stripLength[i], tgc_error[i]);
    break;
  case TGCRAW:
    if(j==1) printf(" TGC hit Chamber   occ  width   length\n");
    id = rawTgc_id[i];
    TgcName(id,cname);
    printf("  TGC %-13s %2i %6.1lf %8.1lf RAW\n",cname,
            rawTgc_occupancy[i], rawTgc_width[i],rawTgc_length[i] );
    break;
  case CSC:
    break;
  case CSCRAW:
    break; 
  }
  return id;
}   //end selector::PrintHit

//  These are not normally used
void selector::SlaveBegin(TTree *) { }
void selector::SlaveTerminate() { }
