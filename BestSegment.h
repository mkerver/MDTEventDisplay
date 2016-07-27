#include <map>
#include <vector>
#include <algorithm>
#include "chamberlist.h"
#include "muonfixedid.h"

// If ONE_SEGMENT_PER_CHAMBER=1 then only allow one best segment per chamber
// if =0 then check if segments share more than 50% of hits.
const Int_t ONE_SEGMENT_PER_CHAMBER=1;

// HitSegment provides a list of all segments with MDT hits.
Int_t HitSegment( class selector* t, Long64_t jentry,
		 std::map<Int_t, std::vector<Int_t> >& map_seghit, int DBG=0 );
// BestSegment should be provided with the a list of segments (e.g. from HitSegment)
// and it provides a list of segments with the following cuts:
//  - require at least as many hits as tube layers - NMiss
//  - allow only one segment per chamber, using the one with the lowest chisq.
Int_t BestSegment( class selector* t, Long64_t jentry,
		   std::map<Int_t,std::vector<Int_t> >& map_seghit, int NMiss=1, int DBG=0 );

//how to use them
//void selector::Loop() {
//
//  std::map<Int_t,std::vector<Int_t> > map_seghit;
//  std::vector<Int_t> vi_best;
//
//  Long64_t nbytes=0, nb=0;
//  for( Long64_t jentry=0; jentry<nentries; ++jentry ){
//    Long64_t ientry = LoadTree(jentry);
//    if (ientry < 0) break;
//    nb = fChain->GetEntry(jentry);
//    nbytes += nb;
//
//    HitSegment ( this, jentry, map_seghit, DBG );
//    BestSegment( this, jentry, map_seghit, vi_best, DBG );
//  } // jentry-loop
//
//  return;
//} // Loop ====================================================================

// Make a map (map_seghit)
Int_t HitSegment( class selector* t, Long64_t jentry,
		  std::map<Int_t, std::vector<Int_t> >& map_seghit, int DBG ) {
  map_seghit.clear();
  std::vector<Int_t> vi_hit;  //vector for MDT hits in 1 segment
  Int_t idx = 0;

// Loop over segments
  for( int ss=0; ss!=t->seg_nSegments; ss++ ) {
    vi_hit.clear();
// Loop over MDT hits and put hits from this segment in a vector
    for( int hh=0; hh<t->mdt_nMdt; hh++ ) {
// Check if hit is on segment
      if( t->mdt_segIndex[hh]!=ss ) continue;
// If chamber has valid muonfixedid add to vector of hits 
      idx = MDTindex(t->mdt_id[hh]);
      if( idx<0 ) {        //if unknown muonfixedid
	std::cout << Form("HitSegment: BAD MUONFIXEDID %i event %i MFID %i\n",
			  t->event_runNumber,t->event_eventNumber,t->mdt_id[hh]);
	continue;
      }
      vi_hit.push_back(hh);
    } // hh-loop
//  If found any MDT hits add vector to segment hit map
//  Store vector in map using segment number as index
    if( vi_hit.size() > 0 ) map_seghit[ss]=vi_hit;
  } // ss-loop

  if( DBG ) {
    std::cerr << Form("\nHitSegment: Run %i event %i jentry=%ld\n",t->event_runNumber,t->event_eventNumber,(long int)jentry);
    std::map<Int_t,std::vector<Int_t> >::iterator it_map;
    for( it_map=map_seghit.begin(); it_map!=map_seghit.end(); it_map++ ){
      Int_t ss = (*it_map).first;
      vi_hit   = (*it_map).second;
      std::cerr << Form("  ss=%i   #hit %i\n",ss,(int)vi_hit.size());
      for( unsigned int uu=0; uu!=vi_hit.size(); uu++ ){
        Int_t hh = vi_hit[uu];
	char chamber[20];
	TubeName(t->mdt_id[hh],chamber);
        std::cerr << Form("\t %i %i %s\n",uu,hh,chamber);
      } // uu-loop
    } // it_map-loop
  } // DBG
  return 0;
} // HitSegment ===============================================================

Int_t BestSegment( class selector* t, Long64_t jentry,
		   std::map<Int_t,std::vector<Int_t> >& map_seghit, int NMiss, int DBG ) {

  if( DBG ) std::cerr << Form("\nBestSegment: Run %i event %i jentry=%ld\n",t->event_runNumber,t->event_eventNumber,(long int)jentry);

  Int_t idx=0, ss=0, zz=0;
  std::map<Int_t,Int_t> map_best;            // map of flags indicating "best segments"
  std::map<Int_t,std::vector<int> > map_idx; // map of list of MDTs in eash segment
  std::vector<Int_t> vi_hit,vi_hit2,vi_mdt;
//maybe not really needed?
  map_best.clear();
  map_idx.clear();    

// Loop over map of segments
  std::map<Int_t,std::vector<Int_t> >::iterator it_map,it2_map;
  for( it_map=map_seghit.begin(); it_map!=map_seghit.end(); it_map++ ) {
    ss     = (*it_map).first;
    vi_hit = (*it_map).second;
    vi_mdt.clear();                 //list of MDTs in segment
    //    if(DBG) std::cerr << Form("%3i: chi2=%8.2f   #hit=%2i\n",ss,t->seg_chi2[ss],(int)vi_hit.size());
    for( unsigned int uu=0; uu!=vi_hit.size(); uu++ ) {
      idx = MDTindex( t->mdt_id[vi_hit[uu]] );   
      unsigned int imdt=0;
      for( ; imdt<vi_mdt.size(); imdt++ ) if( idx == vi_mdt[imdt] ) break;
      if( imdt == vi_mdt.size()) vi_mdt.push_back(idx); // save the MDT in segment
      //      if( DBG ){
      //	TubeName(t->mdt_id[hh],chamber);
      //        std::cerr << Form("\t%2i %2i idx=%i %s nMDT=%i",uu,hh,idx,chamber,map_nmdt[ss]);
      //      } // DBG
    } // uu-loop
    map_idx[ss] = vi_mdt;

// Require number of hits to be >= number of layers in chamber
//flag 1=>Segment is a best segment; 0=>not
    if( vi_hit.size() < (unsigned int) chlist[idx].nly_ml1 + chlist[idx].nly_ml2 - NMiss ) {
      map_best[ss] = 0;
      continue;
    }
    map_best[ss] = 1;
                   
  } // it_map-loop

  //  if(DBG) std::cerr << std::endl;

// Do double loop over segments and find ones which overlap in more than 50% of tubes
// and find best chi2 in that bunch
  for( it_map=map_seghit.begin(); it_map!=map_seghit.end(); it_map++ ) {
    ss     = (*it_map).first;
    if( map_best[ss] == 0 ) continue;
    vi_hit = (*it_map).second;
    if(DBG){
      std::cerr << Form("\tSeg1=%2i  #hit %i",ss,(int)vi_hit.size());
      for( unsigned int imdt=0; imdt<map_idx[ss].size(); imdt++ ) std::cerr << Form(" %s",chlist[map_idx[ss][imdt]].hardname);
      std::cerr << Form(" #mdt=%i chi2=%.2f\n",(int)map_idx[ss].size(),t->seg_chi2[ss]);
    } // DBG

    for( it2_map=map_seghit.begin(); it2_map!=map_seghit.end(); it2_map++ ) {
      zz = (*it2_map).first;
      if( ss >= zz || map_best[zz] == 0 ) continue;
      vi_hit2 = (*it2_map).second;
      int overlap=0;
      for( unsigned int imdt=0; imdt<map_idx[ss].size() && overlap==0; imdt++ ) {
	for( unsigned int imdt2=0; imdt2<map_idx[zz].size() && overlap==0; imdt2++ ) {
          if( map_idx[ss][imdt] == map_idx[zz][imdt2] ) overlap = 1;
	}
      }
      if(DBG) {
	std::cerr << Form("\t  Seg2=%2i  #hit %i",zz,(int)vi_hit2.size());
	for( unsigned int imdt=0; imdt<map_idx[zz].size(); imdt++ ) std::cerr << Form(" %s",chlist[map_idx[zz][imdt]].hardname);
	std::cerr << Form(" #mdt=%i chi2=%.2f",(int)map_idx[zz].size(),t->seg_chi2[zz]);
	if( overlap == 0 ) std::cerr << "\t  NO OVERLAP";
        std::cerr << std::endl;
      }

// If each segment only contains 1 chamber which are different then do not check further
      if( overlap == 0 ) continue;
  
// 
      Int_t check_chi2 = 0;
      if( ONE_SEGMENT_PER_CHAMBER ) {
	check_chi2 = 1;
      } else {
        Int_t nshared=0; // #same hit in segment ss and zz
// Loop over hits in first segment
        for( unsigned int hh=0; hh!=vi_hit.size(); hh++ ) {
	  Int_t mfid = t->mdt_id[vi_hit[hh]];
	  if(DBG) {
	    char chamber[20];
	    TubeName(mfid,chamber);
	    std::cerr << Form("\t    Check Hit %2i %s",hh,chamber);
	  }
// Loop over hits in second segment and look for hit in same tube (mfid)
	  int found=0;
	  for( unsigned int uu=0; uu!=vi_hit2.size(); uu++ ) {
	    if( mfid == t->mdt_id[vi_hit2[uu]] ) {
	      found=1;
	      nshared++;
	      break;
	    }
	  }  // uu-loop
	  if(DBG) {
	    if(found) std::cerr << " FOUND";
	    else      std::cerr << "      ";
	    std::cerr << "  #common=" << nshared << std::endl;
	  }
	}  // hh-loop

	if(DBG) std::cerr << Form("\t    #hit(%i&&%i) %i vs %.1f\n",
				  ss,zz,nshared,0.5*TMath::Min(vi_hit.size(),vi_hit2.size()));
	check_chi2 = nshared > 0.5*TMath::Min(vi_hit.size(),vi_hit2.size());
      }

      if( check_chi2 ) {
	if( t->seg_chi2[zz] < t->seg_chi2[ss] ) {
          map_best[ss]=0;
	} else if( t->seg_chi2[zz] > t->seg_chi2[ss] ) {
          map_best[zz]=0;
	} else if( vi_hit.size() <= vi_hit2.size() ) {
	  map_best[ss]=0;
	} else {
	  map_best[zz]=0;
	}
      }

    } // it2_map-loop
  }   // it_map-loop

// Remove segments which are not best segments
  std::map<Int_t,Int_t>::iterator it_best;
  for( it_best=map_best.begin(); it_best!=map_best.end(); it_best++ ) {
    ss         = (*it_best).first;
    Int_t best = (*it_best).second;
    if(DBG) {
      std::cerr << Form(" segment #%2i %s #hit %i chi2=%.2f best? %s\n",
			ss,chlist[map_idx[ss][0]].hardname,(int)map_seghit[ss].size(),t->seg_chi2[ss],(best ? "Y" : "N"));
    }
    if( best == 0 ) map_seghit.erase(ss);
  } 

  return 0;
} // BestSegment ==============================================================
