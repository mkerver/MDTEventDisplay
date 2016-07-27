   ///////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu May 30 16:18:57 2013 by ROOT version 5.34/03
// from TTree Segments/My first tree
// found on file: test_calib_ntuple.root
//////////////////////////////////////////////////////////

#ifndef selector_h
#define selector_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class selector : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

   // Declaration of leaf types
   Int_t           event_nEvent;
   Int_t           event_eventNumber;
   Int_t           event_runNumber;
   Int_t           event_timeStamp;
   Int_t           event_lumiBlock;
   Int_t           event_bcId;
   Float_t         event_mbtsTimeDiff;
   Float_t         event_larTimeDiff;
   UChar_t         event_eventTag[64];
   Int_t           event_n_on_TAV_bits;
   Int_t           event_on_TAV_bits[1];   //[event_n_on_TAV_bits]
   Int_t           event_n_on_TAP_bits;
   Int_t           event_on_TAP_bits[1];   //[event_n_on_TAP_bits]
   Int_t           pat_nPatterns;
   Float_t         pat_chi2[100];   //[pat_nPatterns]
   Float_t         pat_z0[100];   //[pat_nPatterns]
   Float_t         pat_r0[100];   //[pat_nPatterns]
   Float_t         pat_invP[100];   //[pat_nPatterns]
   Float_t         pat_phi[100];   //[pat_nPatterns]
   Float_t         pat_theta[100];   //[pat_nPatterns]
   Int_t           pat_nSegments[100];   //[pat_nPatterns]
   Int_t           pat_nmdt[100];   //[pat_nPatterns]
   Int_t           pat_nrpc[100];   //[pat_nPatterns]
   Int_t           pat_ncsc[100];   //[pat_nPatterns]
   Int_t           pat_ntgc[100];   //[pat_nPatterns]
   Int_t           seg_nSegments;
   Int_t           seg_patIndex[400];   //[seg_nSegments]
   Int_t           seg_author[400];   //[seg_nSegments]
   Int_t           seg_quality[400];   //[seg_nSegments]
   Float_t         seg_chi2[400];   //[seg_nSegments]
   Float_t         seg_fittedT0[400];   //[seg_nSegments]
   Float_t         seg_gPosX[400];   //[seg_nSegments]
   Float_t         seg_gPosY[400];   //[seg_nSegments]
   Float_t         seg_gPosZ[400];   //[seg_nSegments]
   Float_t         seg_gDirX[400];   //[seg_nSegments]
   Float_t         seg_gDirY[400];   //[seg_nSegments]
   Float_t         seg_gDirZ[400];   //[seg_nSegments]
   Float_t         seg_posX[400];   //[seg_nSegments]
   Float_t         seg_posY[400];   //[seg_nSegments]
   Float_t         seg_posZ[400];   //[seg_nSegments]
   Float_t         seg_dirX[400];   //[seg_nSegments]
   Float_t         seg_dirY[400];   //[seg_nSegments]
   Float_t         seg_dirZ[400];   //[seg_nSegments]
   Int_t           seg_nHits[400];   //[seg_nSegments]
   Int_t           seg_nMdtHits[400];   //[seg_nSegments]
   Int_t           seg_nRpcHits[400];   //[seg_nSegments]
   Int_t           seg_nTgcHits[400];   //[seg_nSegments]
   Int_t           seg_nCscHits[400];   //[seg_nSegments]
   Double_t        seg_transPhi[400];   //[seg_nSegments]
   Double_t        seg_transTheta[400];   //[seg_nSegments]
   Double_t        seg_transPsi[400];   //[seg_nSegments]
   Double_t        seg_transX[400];   //[seg_nSegments]
   Double_t        seg_transY[400];   //[seg_nSegments]
   Double_t        seg_transZ[400];   //[seg_nSegments]
   Int_t           mdt_nMdt;
   Int_t           mdt_segIndex[3000];   //[mdt_nMdt]
   Int_t           mdt_id[3000];   //[mdt_nMdt]
   Int_t           mdt_tdc[3000];   //[mdt_nMdt]
   Int_t           mdt_adc[3000];   //[mdt_nMdt]
   Float_t         mdt_t[3000];   //[mdt_nMdt]
   Float_t         mdt_r[3000];   //[mdt_nMdt]
   Float_t         mdt_dr[3000];   //[mdt_nMdt]
   Float_t         mdt_rTrk[3000];   //[mdt_nMdt]
   Float_t         mdt_drTrk[3000];   //[mdt_nMdt]
   Float_t         mdt_resi[3000];   //[mdt_nMdt]
   Float_t         mdt_distRO[3000];   //[mdt_nMdt]
   Float_t         mdt_slewTime[3000];   //[mdt_nMdt]
   Float_t         mdt_lorTime[3000];   //[mdt_nMdt]
   Float_t         mdt_propTime[3000];   //[mdt_nMdt]
   Float_t         mdt_tof[3000];   //[mdt_nMdt]
   Float_t         mdt_bFieldPerp[3000];   //[mdt_nMdt]
   Float_t         mdt_bFieldPara[3000];   //[mdt_nMdt]
   Float_t         mdt_temperature[3000];   //[mdt_nMdt]
   Float_t         mdt_projSag[3000];   //[mdt_nMdt]
   Float_t         mdt_posX[3000];   //[mdt_nMdt]
   Float_t         mdt_posY[3000];   //[mdt_nMdt]
   Float_t         mdt_posZ[3000];   //[mdt_nMdt]
   Float_t         mdt_gPosX[3000];   //[mdt_nMdt]
   Float_t         mdt_gPosY[3000];   //[mdt_nMdt]
   Float_t         mdt_gPosZ[3000];   //[mdt_nMdt]
   Float_t         mdt_closesApproachX[3000];   //[mdt_nMdt]
   Float_t         mdt_closesApproachY[3000];   //[mdt_nMdt]
   Float_t         mdt_closesApproachZ[3000];   //[mdt_nMdt]
   Float_t         mdt_gClosesApproachX[3000];   //[mdt_nMdt]
   Float_t         mdt_gClosesApproachY[3000];   //[mdt_nMdt]
   Float_t         mdt_gClosesApproachZ[3000];   //[mdt_nMdt]
   Float_t         mdt_temperature_time[3000];   //[mdt_nMdt]
   Float_t         mdt_wire_sag_time[3000];   //[mdt_nMdt]
   Float_t         mdt_tube_t0[3000];   //[mdt_nMdt]
   Float_t         mdt_xtwin[3000];   //[mdt_nMdt]
   Bool_t          mdt_segmentT0Applied[3000];   //[mdt_nMdt]
   Int_t           rpcOs_nRpcHits;
   Int_t           rpcOs_segIndex[3000];   //[rpcOs_nRpcHits]
   Int_t           rpcOs_id[3000];   //[rpcOs_nRpcHits]
   Int_t           rpcOs_nStrips[3000];   //[rpcOs_nRpcHits]
   Float_t         rpcOs_stripWidth[3000];   //[rpcOs_nRpcHits]
   Float_t         rpcOs_stripLength[3000];   //[rpcOs_nRpcHits]
   Float_t         rpcOs_error[3000];   //[rpcOs_nRpcHits]
   Float_t         rpcOs_time[3000];   //[rpcOs_nRpcHits]
   Float_t         rpcOs_posX[3000];   //[rpcOs_nRpcHits]
   Float_t         rpcOs_posY[3000];   //[rpcOs_nRpcHits]
   Float_t         rpcOs_posZ[3000];   //[rpcOs_nRpcHits]
   Float_t         rpcOs_gPosX[3000];   //[rpcOs_nRpcHits]
   Float_t         rpcOs_gPosY[3000];   //[rpcOs_nRpcHits]
   Float_t         rpcOs_gPosZ[3000];   //[rpcOs_nRpcHits]
   Float_t         rpcOs_distanceToRO[3000];   //[rpcOs_nRpcHits]
   Int_t           tgc_nTgc;
   Int_t           tgc_segIndex[3000];   //[tgc_nTgc]
   Int_t           tgc_id[3000];   //[tgc_nTgc]
   Int_t           tgc_nStrips[3000];   //[tgc_nTgc]
   Float_t         tgc_stripWidth[3000];   //[tgc_nTgc]
   Float_t         tgc_stripLength[3000];   //[tgc_nTgc]
   Float_t         tgc_error[3000];   //[tgc_nTgc]
   Float_t         tgc_posX[3000];   //[tgc_nTgc]
   Float_t         tgc_posY[3000];   //[tgc_nTgc]
   Float_t         tgc_posZ[3000];   //[tgc_nTgc]
   Float_t         tgc_gPosX[3000];   //[tgc_nTgc]
   Float_t         tgc_gPosY[3000];   //[tgc_nTgc]
   Float_t         tgc_gPosZ[3000];   //[tgc_nTgc]
   Int_t           csc_nCsc;
   Int_t           csc_segIndex[3000];   //[csc_nCsc]
   Int_t           csc_id[3000];   //[csc_nCsc]
   Int_t           csc_nStrips[3000];   //[csc_nCsc]
   Float_t         csc_stripWidth[3000];   //[csc_nCsc]
   Int_t           csc_charge[3000];   //[csc_nCsc]
   Float_t         csc_error[3000];   //[csc_nCsc]
   Float_t         csc_posX[3000];   //[csc_nCsc]
   Float_t         csc_posY[3000];   //[csc_nCsc]
   Float_t         csc_posZ[3000];   //[csc_nCsc]
   Float_t         csc_gPosX[3000];   //[csc_nCsc]
   Float_t         csc_gPosY[3000];   //[csc_nCsc]
   Float_t         csc_gPosZ[3000];   //[csc_nCsc]
   Int_t           rawMdt_nRMdt;
   Int_t           rawMdt_id[13000];   //[rawMdt_nRMdt]
   Int_t           rawMdt_tdc[13000];   //[rawMdt_nRMdt]
   Int_t           rawMdt_adc[13000];   //[rawMdt_nRMdt]
   Float_t         rawMdt_gPosX[13000];   //[rawMdt_nRMdt]
   Float_t         rawMdt_gPosY[13000];   //[rawMdt_nRMdt]
   Float_t         rawMdt_gPosZ[13000];   //[rawMdt_nRMdt]
   Int_t           rawRpc_nRRpc;
   Int_t           rawRpc_occupancy[3000];   //[rawRpc_nRRpc]
   Int_t           rawRpc_id[3000];   //[rawRpc_nRRpc]
   Float_t         rawRpc_t[3000];   //[rawRpc_nRRpc]
   Float_t         rawRpc_width[3000];   //[rawRpc_nRRpc]
   Float_t         rawRpc_length[3000];   //[rawRpc_nRRpc]
   Float_t         rawRpc_gPosX[3000];   //[rawRpc_nRRpc]
   Float_t         rawRpc_gPosY[3000];   //[rawRpc_nRRpc]
   Float_t         rawRpc_gPosZ[3000];   //[rawRpc_nRRpc]
   Int_t           rawRpcCoin_nRpcCoin;
   Int_t           rawRpcCoin_occupancy[3000];   //[rawRpcCoin_nRpcCoin]
   Int_t           rawRpcCoin_id[3000];   //[rawRpcCoin_nRpcCoin]
   Float_t         rawRpcCoin_t[3000];   //[rawRpcCoin_nRpcCoin]
   Float_t         rawRpcCoin_width[3000];   //[rawRpcCoin_nRpcCoin]
   Float_t         rawRpcCoin_length[3000];   //[rawRpcCoin_nRpcCoin]
   Float_t         rawRpcCoin_gPosX[3000];   //[rawRpcCoin_nRpcCoin]
   Float_t         rawRpcCoin_gPosY[3000];   //[rawRpcCoin_nRpcCoin]
   Float_t         rawRpcCoin_gPosZ[3000];   //[rawRpcCoin_nRpcCoin]
   Int_t           rawRpcCoin_ijk[3000];   //[rawRpcCoin_nRpcCoin]
   Int_t           rawRpcCoin_threshold[3000];   //[rawRpcCoin_nRpcCoin]
   Int_t           rawRpcCoin_overlap[3000];   //[rawRpcCoin_nRpcCoin]
   Int_t           rawRpcCoin_parentCmId[3000];   //[rawRpcCoin_nRpcCoin]
   Int_t           rawRpcCoin_parentPadId[3000];   //[rawRpcCoin_nRpcCoin]
   Int_t           rawRpcCoin_parentSectorId[3000];   //[rawRpcCoin_nRpcCoin]
   Int_t           rawRpcCoin_lowPtCm[3000];   //[rawRpcCoin_nRpcCoin]
   Int_t           rawCsc_nRCsc;
   Int_t           rawCsc_occupancy[3000];   //[rawCsc_nRCsc]
   Int_t           rawCsc_id[3000];   //[rawCsc_nRCsc]
   Float_t         rawCsc_width[3000];   //[rawCsc_nRCsc]
   Float_t         rawCsc_charge[3000];   //[rawCsc_nRCsc]
   Float_t         rawCsc_t[3000];   //[rawCsc_nRCsc]
   Float_t         rawCsc_gPosX[3000];   //[rawCsc_nRCsc]
   Float_t         rawCsc_gPosY[3000];   //[rawCsc_nRCsc]
   Float_t         rawCsc_gPosZ[3000];   //[rawCsc_nRCsc]
   Int_t           rpc_SL_nSectors;
   Int_t           rpc_SL_sectorId[100];   //[rpc_SL_nSectors]
   Int_t           rpc_SL_lvl1id[100];   //[rpc_SL_nSectors]
   Int_t           rpc_SL_bcid[100];   //[rpc_SL_nSectors]
   Int_t           rpc_SL_errorCode[100];   //[rpc_SL_nSectors]
   Int_t           rpc_SL_crc[100];   //[rpc_SL_nSectors]
   Int_t           rpc_SL_hasMoreThan2TriggerCand[100];   //[rpc_SL_nSectors]
   Int_t           rpc_SLnTriggerHits;
   Int_t           rpc_SLhit_sector[100];   //[rpc_SLnTriggerHits]
   Int_t           rpc_SLhit_rowinBcid[100];   //[rpc_SLnTriggerHits]
   Int_t           rpc_SLhit_padId[100];   //[rpc_SLnTriggerHits]
   Int_t           rpc_SLhit_ptId[100];   //[rpc_SLnTriggerHits]
   Int_t           rpc_SLhit_roi[100];   //[rpc_SLnTriggerHits]
   Int_t           rpc_SLhit_outerPlane[100];   //[rpc_SLnTriggerHits]
   Int_t           rpc_SLhit_overlapPhi[100];   //[rpc_SLnTriggerHits]
   Int_t           rpc_SLhit_overlapEta[100];   //[rpc_SLnTriggerHits]
   Int_t           rpc_SLhit_triggerBcid[100];   //[rpc_SLnTriggerHits]
   Int_t           rpc_SLhit_isInput[100];   //[rpc_SLnTriggerHits]
   Int_t           rpc_fchan;
   Int_t           rpc_sector[3000];   //[rpc_fchan]
   Int_t           rpc_padId[3000];   //[rpc_fchan]
   Int_t           rpc_status[3000];   //[rpc_fchan]
   Int_t           rpc_ercode[3000];   //[rpc_fchan]
   Int_t           rpc_cmaId[3000];   //[rpc_fchan]
   Int_t           rpc_fel1Id[3000];   //[rpc_fchan]
   Int_t           rpc_febcId[3000];   //[rpc_fchan]
   Int_t           rpc_crc[3000];   //[rpc_fchan]
   Int_t           rpc_bcId[3000];   //[rpc_fchan]
   Int_t           rpc_ticks[3000];   //[rpc_fchan]
   Int_t           rpc_ijk[3000];   //[rpc_fchan]
   Int_t           rpc_cmachan[3000];   //[rpc_fchan]
   Int_t           rpc_overlap[3000];   //[rpc_fchan]
   Int_t           rpc_threshold[3000];   //[rpc_fchan]
   //   Int_t           phi_nPhiHits;
   //   Int_t           phi_phiPatIndex[1];   //[phi_nPhiHits]
   //   UInt_t          phi_id[1];   //[phi_nPhiHits]
   //   Float_t         phi_posX[1];   //[phi_nPhiHits]
   //   Float_t         phi_posY[1];   //[phi_nPhiHits]
   //   Float_t         phi_posZ[1];   //[phi_nPhiHits]
   //   Float_t         phi_error[1];   //[phi_nPhiHits]
   Int_t           trkHit_nHits;
   Int_t           trkHit_trackIndex[3000];   //[trkHit_nHits]
   UInt_t          trkHit_id[3000];   //[trkHit_nHits]
   Float_t         trkHit_posX[3000];   //[trkHit_nHits]
   Float_t         trkHit_posY[3000];   //[trkHit_nHits]
   Float_t         trkHit_posZ[3000];   //[trkHit_nHits]
   Float_t         trkHit_driftRadius[3000];   //[trkHit_nHits]
   Float_t         trkHit_error[3000];   //[trkHit_nHits]
   Float_t         trkHit_resi[3000];   //[trkHit_nHits]
   Float_t         trkHit_pull[3000];   //[trkHit_nHits]
   Int_t           trkHit_type[3000];   //[trkHit_nHits]
   //   Int_t           trkHole_nHoles;
   //   Int_t           trkHole_trackIndex[116];   //[trkHole_nHoles]
   //   UInt_t          trkHole_id[116];   //[trkHole_nHoles]
   //   Float_t         trkHole_posX[116];   //[trkHole_nHoles]
   //   Float_t         trkHole_posY[116];   //[trkHole_nHoles]
   //   Float_t         trkHole_posZ[116];   //[trkHole_nHoles]
   Int_t           trk_nTracks;
   Float_t         trk_x0[3000];   //[trk_nTracks]
   Float_t         trk_y0[3000];   //[trk_nTracks]
   Float_t         trk_z0[3000];   //[trk_nTracks]
   Float_t         trk_phi[3000];   //[trk_nTracks]
   Float_t         trk_theta[3000];   //[trk_nTracks]
   Float_t         trk_qOverP[3000];   //[trk_nTracks]
   Int_t           trk_author[3000];   //[trk_nTracks]
   Float_t         trk_cov00[3000];   //[trk_nTracks]
   Float_t         trk_cov01[3000];   //[trk_nTracks]
   Float_t         trk_cov02[3000];   //[trk_nTracks]
   Float_t         trk_cov03[3000];   //[trk_nTracks]
   Float_t         trk_cov04[3000];   //[trk_nTracks]
   Float_t         trk_cov11[3000];   //[trk_nTracks]
   Float_t         trk_cov12[3000];   //[trk_nTracks]
   Float_t         trk_cov13[3000];   //[trk_nTracks]
   Float_t         trk_cov14[3000];   //[trk_nTracks]
   Float_t         trk_cov22[3000];   //[trk_nTracks]
   Float_t         trk_cov23[3000];   //[trk_nTracks]
   Float_t         trk_cov24[3000];   //[trk_nTracks]
   Float_t         trk_cov33[3000];   //[trk_nTracks]
   Float_t         trk_cov34[3000];   //[trk_nTracks]
   Float_t         trk_cov44[3000];   //[trk_nTracks]
   Float_t         trk_chi2[3000];   //[trk_nTracks]
   Int_t           trk_ndof[3000];   //[trk_nTracks]
   Int_t           trkSeg_nTrkSegs;
   UShort_t        trkSeg_trkIndex[163];   //[trkSeg_nTrkSegs]
   UShort_t        trkSeg_segIndex[163];   //[trkSeg_nTrkSegs]
   Int_t           muctpi_nHits;
   Int_t           muctpi_roiWord[100];   //[muctpi_nHits]
   Int_t           muctpi_bcID[100];   //[muctpi_nHits]
   Int_t           muctpi_sysID[100];   //[muctpi_nHits]
   Int_t           muctpi_subsysID[100];   //[muctpi_nHits]
   Int_t           muctpi_sectorID[100];   //[muctpi_nHits]
   Int_t           muctpi_thresholdNumber[100];   //[muctpi_nHits]
   Int_t           muctpi_thresholdValue[100];   //[muctpi_nHits]
   Int_t           muctpi_roINumber[100];   //[muctpi_nHits]
   Int_t           muctpi_overlap[100];   //[muctpi_nHits]
   Int_t           muctpi_firstCandidate[100];   //[muctpi_nHits]
   Int_t           muctpi_sectorOverflow[100];   //[muctpi_nHits]
   Int_t           muctpi_padOverflow[100];   //[muctpi_nHits]
   Float_t         muctpi_phi[100];   //[muctpi_nHits]
   Float_t         muctpi_eta[100];   //[muctpi_nHits]
   Int_t           mbts_nHits;
   Int_t           mbts_id[100];   //[mbts_nHits]
   Float_t         mbts_posX[100];   //[mbts_nHits]
   Float_t         mbts_posY[100];   //[mbts_nHits]
   Float_t         mbts_posZ[100];   //[mbts_nHits]
   Float_t         mbts_time[100];   //[mbts_nHits]
   Float_t         mbts_charge[100];   //[mbts_nHits]
   Int_t           calo_nHits;
   Int_t           calo_id[100];   //[calo_nHits]
   Float_t         calo_posX[100];   //[calo_nHits]
   Float_t         calo_posY[100];   //[calo_nHits]
   Float_t         calo_posZ[100];   //[calo_nHits]
   Float_t         calo_time[100];   //[calo_nHits]
   Float_t         calo_charge[100];   //[calo_nHits]
   Int_t           ctp_nTriggerInfo;
   UInt_t          ctp_numberBC;
   UInt_t          ctp_L1A_BC;
   UInt_t          ctp_timeNs;
   Int_t           ctp_randomTrig;
   UInt_t          ctp_firedItemsBeforePrescale;
   UInt_t          ctp_prescaledClock;
   Int_t           ctp_bcid[7680];   //[ctp_nTriggerInfo]
   Int_t           ctp_bcIndexTriggerItems[7680];   //[ctp_nTriggerInfo]
   Int_t           ctp_bcIndexPIT[7680];   //[ctp_nTriggerInfo]
   Int_t           ctp_pit[7680];   //[ctp_nTriggerInfo]
   Int_t           ctp_tbp[7680];   //[ctp_nTriggerInfo]
   Int_t           ctp_tap[7680];   //[ctp_nTriggerInfo]
   Int_t           ctp_tav[7680];   //[ctp_nTriggerInfo]
   Int_t           ctp_type[7680];   //[ctp_nTriggerInfo]
   Float_t         ctp_delay[7680];   //[ctp_nTriggerInfo]

   // List of branches
   TBranch        *b_event_nEvent;   //!
   TBranch        *b_event_eventNumber;   //!
   TBranch        *b_event_runNumber;   //!
   TBranch        *b_event_timeStamp;   //!
   TBranch        *b_event_lumiBlock;   //!
   TBranch        *b_event_bcId;   //!
   TBranch        *b_event_mbtsTimeDiff;   //!
   TBranch        *b_event_larTimeDiff;   //!
   TBranch        *b_event_eventTag;   //!
   TBranch        *b_event_n_on_TAV_bits;   //!
   TBranch        *b_event_on_TAV_bits;   //!
   TBranch        *b_event_n_on_TAP_bits;   //!
   TBranch        *b_event_on_TAP_bits;   //!
   TBranch        *b_pat_nPatterns;   //!
   TBranch        *b_pat_chi2;   //!
   TBranch        *b_pat_z0;   //!
   TBranch        *b_pat_r0;   //!
   TBranch        *b_pat_invP;   //!
   TBranch        *b_pat_phi;   //!
   TBranch        *b_pat_theta;   //!
   TBranch        *b_pat_nSegments;   //!
   TBranch        *b_pat_nmdt;   //!
   TBranch        *b_pat_nrpc;   //!
   TBranch        *b_pat_ncsc;   //!
   TBranch        *b_pat_ntgc;   //!
   TBranch        *b_seg_nSegments;   //!
   TBranch        *b_seg_patIndex;   //!
   TBranch        *b_seg_author;   //!
   TBranch        *b_seg_quality;   //!
   TBranch        *b_seg_chi2;   //!
   TBranch        *b_seg_fittedT0;   //!
   TBranch        *b_seg_gPosX;   //!
   TBranch        *b_seg_gPosY;   //!
   TBranch        *b_seg_gPosZ;   //!
   TBranch        *b_seg_gDirX;   //!
   TBranch        *b_seg_gDirY;   //!
   TBranch        *b_seg_gDirZ;   //!
   TBranch        *b_seg_posX;   //!
   TBranch        *b_seg_posY;   //!
   TBranch        *b_seg_posZ;   //!
   TBranch        *b_seg_dirX;   //!
   TBranch        *b_seg_dirY;   //!
   TBranch        *b_seg_dirZ;   //!
   TBranch        *b_seg_nHits;   //!
   TBranch        *b_seg_nMdtHits;   //!
   TBranch        *b_seg_nRpcHits;   //!
   TBranch        *b_seg_nTgcHits;   //!
   TBranch        *b_seg_nCscHits;   //!
   TBranch        *b_seg_transPhi;   //!
   TBranch        *b_seg_transTheta;   //!
   TBranch        *b_seg_transPsi;   //!
   TBranch        *b_seg_transX;   //!
   TBranch        *b_seg_transY;   //!
   TBranch        *b_seg_transZ;   //!
   TBranch        *b_mdt_nMdt;   //!
   TBranch        *b_mdt_segIndex;   //!
   TBranch        *b_mdt_id;   //!
   TBranch        *b_mdt_tdc;   //!
   TBranch        *b_mdt_adc;   //!
   TBranch        *b_mdt_t;   //!
   TBranch        *b_mdt_r;   //!
   TBranch        *b_mdt_dr;   //!
   TBranch        *b_mdt_rTrk;   //!
   TBranch        *b_mdt_drTrk;   //!
   TBranch        *b_mdt_resi;   //!
   TBranch        *b_mdt_distRO;   //!
   TBranch        *b_mdt_slewTime;   //!
   TBranch        *b_mdt_lorTime;   //!
   TBranch        *b_mdt_propTime;   //!
   TBranch        *b_mdt_tof;   //!
   TBranch        *b_mdt_bFieldPerp;   //!
   TBranch        *b_mdt_bFieldPara;   //!
   TBranch        *b_mdt_temperature;   //!
   TBranch        *b_mdt_projSag;   //!
   TBranch        *b_mdt_posX;   //!
   TBranch        *b_mdt_posY;   //!
   TBranch        *b_mdt_posZ;   //!
   TBranch        *b_mdt_gPosX;   //!
   TBranch        *b_mdt_gPosY;   //!
   TBranch        *b_mdt_gPosZ;   //!
   TBranch        *b_mdt_closesApproachX;   //!
   TBranch        *b_mdt_closesApproachY;   //!
   TBranch        *b_mdt_closesApproachZ;   //!
   TBranch        *b_mdt_gClosesApproachX;   //!
   TBranch        *b_mdt_gClosesApproachY;   //!
   TBranch        *b_mdt_gClosesApproachZ;   //!
   TBranch        *b_mdt_temperature_time;   //!
   TBranch        *b_mdt_wire_sag_time;   //!
   TBranch        *b_mdt_tube_t0;   //!
   TBranch        *b_mdt_xtwin;   //!
   TBranch        *b_mdt_segmentT0Applied;   //!
   TBranch        *b_rpcOs_nRpcHits;   //!
   TBranch        *b_rpcOs_segIndex;   //!
   TBranch        *b_rpcOs_id;   //!
   TBranch        *b_rpcOs_nStrips;   //!
   TBranch        *b_rpcOs_stripWidth;   //!
   TBranch        *b_rpcOs_stripLength;   //!
   TBranch        *b_rpcOs_error;   //!
   TBranch        *b_rpcOs_time;   //!
   TBranch        *b_rpcOs_posX;   //!
   TBranch        *b_rpcOs_posY;   //!
   TBranch        *b_rpcOs_posZ;   //!
   TBranch        *b_rpcOs_gPosX;   //!
   TBranch        *b_rpcOs_gPosY;   //!
   TBranch        *b_rpcOs_gPosZ;   //!
   TBranch        *b_rpcOs_distanceToRO;   //!
   TBranch        *b_tgc_nTgc;   //!
   TBranch        *b_tgc_segIndex;   //!
   TBranch        *b_tgc_id;   //!
   TBranch        *b_tgc_nStrips;   //!
   TBranch        *b_tgc_stripWidth;   //!
   TBranch        *b_tgc_stripLength;   //!
   TBranch        *b_tgc_error;   //!
   TBranch        *b_tgc_posX;   //!
   TBranch        *b_tgc_posY;   //!
   TBranch        *b_tgc_posZ;   //!
   TBranch        *b_tgc_gPosX;   //!
   TBranch        *b_tgc_gPosY;   //!
   TBranch        *b_tgc_gPosZ;   //!
   TBranch        *b_csc_nCsc;   //!
   TBranch        *b_csc_segIndex;   //!
   TBranch        *b_csc_id;   //!
   TBranch        *b_csc_nStrips;   //!
   TBranch        *b_csc_stripWidth;   //!
   TBranch        *b_csc_charge;   //!
   TBranch        *b_csc_error;   //!
   TBranch        *b_csc_posX;   //!
   TBranch        *b_csc_posY;   //!
   TBranch        *b_csc_posZ;   //!
   TBranch        *b_csc_gPosX;   //!
   TBranch        *b_csc_gPosY;   //!
   TBranch        *b_csc_gPosZ;   //!
   TBranch        *b_rawMdt_nRMdt;   //!
   TBranch        *b_rawMdt_id;   //!
   TBranch        *b_rawMdt_tdc;   //!
   TBranch        *b_rawMdt_adc;   //!
   TBranch        *b_rawMdt_gPosX;   //!
   TBranch        *b_rawMdt_gPosY;   //!
   TBranch        *b_rawMdt_gPosZ;   //!
   TBranch        *b_rawRpc_nRRpc;   //!
   TBranch        *b_rawRpc_occupancy;   //!
   TBranch        *b_rawRpc_id;   //!
   TBranch        *b_rawRpc_t;   //!
   TBranch        *b_rawRpc_width;   //!
   TBranch        *b_rawRpc_length;   //!
   TBranch        *b_rawRpc_gPosX;   //!
   TBranch        *b_rawRpc_gPosY;   //!
   TBranch        *b_rawRpc_gPosZ;   //!
   TBranch        *b_rawRpcCoin_nRpcCoin;   //!
   TBranch        *b_rawRpcCoin_occupancy;   //!
   TBranch        *b_rawRpcCoin_id;   //!
   TBranch        *b_rawRpcCoin_t;   //!
   TBranch        *b_rawRpcCoin_width;   //!
   TBranch        *b_rawRpcCoin_length;   //!
   TBranch        *b_rawRpcCoin_gPosX;   //!
   TBranch        *b_rawRpcCoin_gPosY;   //!
   TBranch        *b_rawRpcCoin_gPosZ;   //!
   TBranch        *b_rawRpcCoin_ijk;   //!
   TBranch        *b_rawRpcCoin_threshold;   //!
   TBranch        *b_rawRpcCoin_overlap;   //!
   TBranch        *b_rawRpcCoin_parentCmId;   //!
   TBranch        *b_rawRpcCoin_parentPadId;   //!
   TBranch        *b_rawRpcCoin_parentSectorId;   //!
   TBranch        *b_rawRpcCoin_lowPtCm;   //!
   TBranch        *b_rawCsc_nRCsc;   //!
   TBranch        *b_rawCsc_occupancy;   //!
   TBranch        *b_rawCsc_id;   //!
   TBranch        *b_rawCsc_width;   //!
   TBranch        *b_rawCsc_charge;   //!
   TBranch        *b_rawCsc_t;   //!
   TBranch        *b_rawCsc_gPosX;   //!
   TBranch        *b_rawCsc_gPosY;   //!
   TBranch        *b_rawCsc_gPosZ;   //!
   TBranch        *b_rpc_SL_nSectors;   //!
   TBranch        *b_rpc_SL_sectorId;   //!
   TBranch        *b_rpc_SL_lvl1id;   //!
   TBranch        *b_rpc_SL_bcid;   //!
   TBranch        *b_rpc_SL_errorCode;   //!
   TBranch        *b_rpc_SL_crc;   //!
   TBranch        *b_rpc_SL_hasMoreThan2TriggerCand;   //!
   TBranch        *b_rpc_SLnTriggerHits;   //!
   TBranch        *b_rpc_SLhit_sector;   //!
   TBranch        *b_rpc_SLhit_rowinBcid;   //!
   TBranch        *b_rpc_SLhit_padId;   //!
   TBranch        *b_rpc_SLhit_ptId;   //!
   TBranch        *b_rpc_SLhit_roi;   //!
   TBranch        *b_rpc_SLhit_outerPlane;   //!
   TBranch        *b_rpc_SLhit_overlapPhi;   //!
   TBranch        *b_rpc_SLhit_overlapEta;   //!
   TBranch        *b_rpc_SLhit_triggerBcid;   //!
   TBranch        *b_rpc_SLhit_isInput;   //!
   TBranch        *b_rpc_fchan;   //!
   TBranch        *b_rpc_sector;   //!
   TBranch        *b_rpc_padId;   //!
   TBranch        *b_rpc_status;   //!
   TBranch        *b_rpc_ercode;   //!
   TBranch        *b_rpc_cmaId;   //!
   TBranch        *b_rpc_fel1Id;   //!
   TBranch        *b_rpc_febcId;   //!
   TBranch        *b_rpc_crc;   //!
   TBranch        *b_rpc_bcId;   //!
   TBranch        *b_rpc_ticks;   //!
   TBranch        *b_rpc_ijk;   //!
   TBranch        *b_rpc_cmachan;   //!
   TBranch        *b_rpc_overlap;   //!
   TBranch        *b_rpc_threshold;   //!
   //   TBranch        *b_phi_nPhiHits;   //!
   //   TBranch        *b_phi_phiPatIndex;   //!
   //   TBranch        *b_phi_id;   //!
   //   TBranch        *b_phi_posX;   //!
   //   TBranch        *b_phi_posY;   //!
   //   TBranch        *b_phi_posZ;   //!
   //   TBranch        *b_phi_error;   //!
   TBranch        *b_trkHit_nHits;   //!
   TBranch        *b_trkHit_trackIndex;   //!
   TBranch        *b_trkHit_id;   //!
   TBranch        *b_trkHit_posX;   //!
   TBranch        *b_trkHit_posY;   //!
   TBranch        *b_trkHit_posZ;   //!
   TBranch        *b_trkHit_driftRadius;   //!
   TBranch        *b_trkHit_error;   //!
   TBranch        *b_trkHit_resi;   //!
   TBranch        *b_trkHit_pull;   //!
   TBranch        *b_trkHit_type;   //!
   //   TBranch        *b_trkHole_nHoles;   //!
   //   TBranch        *b_trkHole_trackIndex;   //!
   //   TBranch        *b_trkHole_id;   //!
   //   TBranch        *b_trkHole_posX;   //!
   //   TBranch        *b_trkHole_posY;   //!
   //   TBranch        *b_trkHole_posZ;   //!
   TBranch        *b_trk_nTracks;   //!
   TBranch        *b_trk_x0;   //!
   TBranch        *b_trk_y0;   //!
   TBranch        *b_trk_z0;   //!
   TBranch        *b_trk_phi;   //!
   TBranch        *b_trk_theta;   //!
   TBranch        *b_trk_qOverP;   //!
   TBranch        *b_trk_author;   //!
   TBranch        *b_trk_cov00;   //!
   TBranch        *b_trk_cov01;   //!
   TBranch        *b_trk_cov02;   //!
   TBranch        *b_trk_cov03;   //!
   TBranch        *b_trk_cov04;   //!
   TBranch        *b_trk_cov11;   //!
   TBranch        *b_trk_cov12;   //!
   TBranch        *b_trk_cov13;   //!
   TBranch        *b_trk_cov14;   //!
   TBranch        *b_trk_cov22;   //!
   TBranch        *b_trk_cov23;   //!
   TBranch        *b_trk_cov24;   //!
   TBranch        *b_trk_cov33;   //!
   TBranch        *b_trk_cov34;   //!
   TBranch        *b_trk_cov44;   //!
   TBranch        *b_trk_chi2;   //!
   TBranch        *b_trk_ndof;   //!
   TBranch        *b_trkSeg_nTrkSegs;   //!
   TBranch        *b_trkSeg_trkIndex;   //!
   TBranch        *b_trkSeg_segIndex;   //!
   TBranch        *b_muctpi_nHits;   //!
   TBranch        *b_muctpi_roiWord;   //!
   TBranch        *b_muctpi_bcID;   //!
   TBranch        *b_muctpi_sysID;   //!
   TBranch        *b_muctpi_subsysID;   //!
   TBranch        *b_muctpi_sectorID;   //!
   TBranch        *b_muctpi_thresholdNumber;   //!
   TBranch        *b_muctpi_thresholdValue;   //!
   TBranch        *b_muctpi_roINumber;   //!
   TBranch        *b_muctpi_overlap;   //!
   TBranch        *b_muctpi_firstCandidate;   //!
   TBranch        *b_muctpi_sectorOverflow;   //!
   TBranch        *b_muctpi_padOverflow;   //!
   TBranch        *b_muctpi_phi;   //!
   TBranch        *b_muctpi_eta;   //!
   TBranch        *b_mbts_nHits;   //!
   TBranch        *b_mbts_id;   //!
   TBranch        *b_mbts_posX;   //!
   TBranch        *b_mbts_posY;   //!
   TBranch        *b_mbts_posZ;   //!
   TBranch        *b_mbts_time;   //!
   TBranch        *b_mbts_charge;   //!
   TBranch        *b_calo_nHits;   //!
   TBranch        *b_calo_id;   //!
   TBranch        *b_calo_posX;   //!
   TBranch        *b_calo_posY;   //!
   TBranch        *b_calo_posZ;   //!
   TBranch        *b_calo_time;   //!
   TBranch        *b_calo_charge;   //!
   TBranch        *b_ctp_nTriggerInfo;   //!
   TBranch        *b_ctp_numberBC;   //!
   TBranch        *b_ctp_L1A_BC;   //!
   TBranch        *b_ctp_timeNs;   //!
   TBranch        *b_ctp_randomTrig;   //!
   TBranch        *b_ctp_firedItemsBeforePrescale;   //!
   TBranch        *b_ctp_prescaledClock;   //!
   TBranch        *b_ctp_bcid;   //!
   TBranch        *b_ctp_bcIndexTriggerItems;   //!
   TBranch        *b_ctp_bcIndexPIT;   //!
   TBranch        *b_ctp_pit;   //!
   TBranch        *b_ctp_tbp;   //!
   TBranch        *b_ctp_tap;   //!
   TBranch        *b_ctp_tav;   //!
   TBranch        *b_ctp_type;   //!
   TBranch        *b_ctp_delay;   //!

   selector(TTree * /*tree*/ =0) : fChain(0) { }
   virtual ~selector() { }
   virtual Int_t   Version() const { return 2; }
   virtual void    Begin(TTree *tree);
   virtual void    SlaveBegin(TTree *tree);
   virtual void    Init(TTree *tree);
   virtual Bool_t  Notify();
   virtual Bool_t  Process(Long64_t entry);
   virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
   virtual void    SetOption(const char *option) { fOption = option; }
   virtual void    SetObject(TObject *obj) { fObject = obj; }
   virtual void    SetInputList(TList *input) { fInput = input; }
   virtual TList  *GetOutputList() const { return fOutput; }
   virtual void    SlaveTerminate();
   virtual void    Terminate();
//  Extra methods for selector_resi
   virtual void    PrintSeg(int iseg=-1, int itrk=-1);      //print individual segment
   virtual int     MatchTrack(int alg, int ichamber);  //determine if chamber is on track

   ClassDef(selector,0);
};

#endif

#ifdef selector_cxx
void selector::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("event_nEvent", &event_nEvent, &b_event_nEvent);
   fChain->SetBranchAddress("event_eventNumber", &event_eventNumber, &b_event_eventNumber);
   fChain->SetBranchAddress("event_runNumber", &event_runNumber, &b_event_runNumber);
   fChain->SetBranchAddress("event_timeStamp", &event_timeStamp, &b_event_timeStamp);
   fChain->SetBranchAddress("event_lumiBlock", &event_lumiBlock, &b_event_lumiBlock);
   fChain->SetBranchAddress("event_bcId", &event_bcId, &b_event_bcId);
   fChain->SetBranchAddress("event_mbtsTimeDiff", &event_mbtsTimeDiff, &b_event_mbtsTimeDiff);
   fChain->SetBranchAddress("event_larTimeDiff", &event_larTimeDiff, &b_event_larTimeDiff);
   fChain->SetBranchAddress("event_eventTag", event_eventTag, &b_event_eventTag);
   fChain->SetBranchAddress("event_n_on_TAV_bits", &event_n_on_TAV_bits, &b_event_n_on_TAV_bits);
   fChain->SetBranchAddress("event_on_TAV_bits", &event_on_TAV_bits, &b_event_on_TAV_bits);
   fChain->SetBranchAddress("event_n_on_TAP_bits", &event_n_on_TAP_bits, &b_event_n_on_TAP_bits);
   fChain->SetBranchAddress("event_on_TAP_bits", &event_on_TAP_bits, &b_event_on_TAP_bits);
   fChain->SetBranchAddress("pat_nPatterns", &pat_nPatterns, &b_pat_nPatterns);
   fChain->SetBranchAddress("pat_chi2", pat_chi2, &b_pat_chi2);
   fChain->SetBranchAddress("pat_z0", pat_z0, &b_pat_z0);
   fChain->SetBranchAddress("pat_r0", pat_r0, &b_pat_r0);
   fChain->SetBranchAddress("pat_invP", pat_invP, &b_pat_invP);
   fChain->SetBranchAddress("pat_phi", pat_phi, &b_pat_phi);
   fChain->SetBranchAddress("pat_theta", pat_theta, &b_pat_theta);
   fChain->SetBranchAddress("pat_nSegments", pat_nSegments, &b_pat_nSegments);
   fChain->SetBranchAddress("pat_nmdt", pat_nmdt, &b_pat_nmdt);
   fChain->SetBranchAddress("pat_nrpc", pat_nrpc, &b_pat_nrpc);
   fChain->SetBranchAddress("pat_ncsc", pat_ncsc, &b_pat_ncsc);
   fChain->SetBranchAddress("pat_ntgc", pat_ntgc, &b_pat_ntgc);
   fChain->SetBranchAddress("seg_nSegments", &seg_nSegments, &b_seg_nSegments);
   fChain->SetBranchAddress("seg_patIndex", seg_patIndex, &b_seg_patIndex);
   fChain->SetBranchAddress("seg_author", seg_author, &b_seg_author);
   fChain->SetBranchAddress("seg_quality", seg_quality, &b_seg_quality);
   fChain->SetBranchAddress("seg_chi2", seg_chi2, &b_seg_chi2);
   fChain->SetBranchAddress("seg_fittedT0", seg_fittedT0, &b_seg_fittedT0);
   fChain->SetBranchAddress("seg_gPosX", seg_gPosX, &b_seg_gPosX);
   fChain->SetBranchAddress("seg_gPosY", seg_gPosY, &b_seg_gPosY);
   fChain->SetBranchAddress("seg_gPosZ", seg_gPosZ, &b_seg_gPosZ);
   fChain->SetBranchAddress("seg_gDirX", seg_gDirX, &b_seg_gDirX);
   fChain->SetBranchAddress("seg_gDirY", seg_gDirY, &b_seg_gDirY);
   fChain->SetBranchAddress("seg_gDirZ", seg_gDirZ, &b_seg_gDirZ);
   fChain->SetBranchAddress("seg_posX", seg_posX, &b_seg_posX);
   fChain->SetBranchAddress("seg_posY", seg_posY, &b_seg_posY);
   fChain->SetBranchAddress("seg_posZ", seg_posZ, &b_seg_posZ);
   fChain->SetBranchAddress("seg_dirX", seg_dirX, &b_seg_dirX);
   fChain->SetBranchAddress("seg_dirY", seg_dirY, &b_seg_dirY);
   fChain->SetBranchAddress("seg_dirZ", seg_dirZ, &b_seg_dirZ);
   fChain->SetBranchAddress("seg_nHits", seg_nHits, &b_seg_nHits);
   fChain->SetBranchAddress("seg_nMdtHits", seg_nMdtHits, &b_seg_nMdtHits);
   fChain->SetBranchAddress("seg_nRpcHits", seg_nRpcHits, &b_seg_nRpcHits);
   fChain->SetBranchAddress("seg_nTgcHits", seg_nTgcHits, &b_seg_nTgcHits);
   fChain->SetBranchAddress("seg_nCscHits", seg_nCscHits, &b_seg_nCscHits);
   fChain->SetBranchAddress("seg_transPhi", seg_transPhi, &b_seg_transPhi);
   fChain->SetBranchAddress("seg_transTheta", seg_transTheta, &b_seg_transTheta);
   fChain->SetBranchAddress("seg_transPsi", seg_transPsi, &b_seg_transPsi);
   fChain->SetBranchAddress("seg_transX", seg_transX, &b_seg_transX);
   fChain->SetBranchAddress("seg_transY", seg_transY, &b_seg_transY);
   fChain->SetBranchAddress("seg_transZ", seg_transZ, &b_seg_transZ);
   fChain->SetBranchAddress("mdt_nMdt", &mdt_nMdt, &b_mdt_nMdt);
   fChain->SetBranchAddress("mdt_segIndex", mdt_segIndex, &b_mdt_segIndex);
   fChain->SetBranchAddress("mdt_id", mdt_id, &b_mdt_id);
   fChain->SetBranchAddress("mdt_tdc", mdt_tdc, &b_mdt_tdc);
   fChain->SetBranchAddress("mdt_adc", mdt_adc, &b_mdt_adc);
   fChain->SetBranchAddress("mdt_t", mdt_t, &b_mdt_t);
   fChain->SetBranchAddress("mdt_r", mdt_r, &b_mdt_r);
   fChain->SetBranchAddress("mdt_dr", mdt_dr, &b_mdt_dr);
   fChain->SetBranchAddress("mdt_rTrk", mdt_rTrk, &b_mdt_rTrk);
   fChain->SetBranchAddress("mdt_drTrk", mdt_drTrk, &b_mdt_drTrk);
   fChain->SetBranchAddress("mdt_resi", mdt_resi, &b_mdt_resi);
   fChain->SetBranchAddress("mdt_distRO", mdt_distRO, &b_mdt_distRO);
   fChain->SetBranchAddress("mdt_slewTime", mdt_slewTime, &b_mdt_slewTime);
   fChain->SetBranchAddress("mdt_lorTime", mdt_lorTime, &b_mdt_lorTime);
   fChain->SetBranchAddress("mdt_propTime", mdt_propTime, &b_mdt_propTime);
   fChain->SetBranchAddress("mdt_tof", mdt_tof, &b_mdt_tof);
   fChain->SetBranchAddress("mdt_bFieldPerp", mdt_bFieldPerp, &b_mdt_bFieldPerp);
   fChain->SetBranchAddress("mdt_bFieldPara", mdt_bFieldPara, &b_mdt_bFieldPara);
   fChain->SetBranchAddress("mdt_temperature", mdt_temperature, &b_mdt_temperature);
   fChain->SetBranchAddress("mdt_projSag", mdt_projSag, &b_mdt_projSag);
   fChain->SetBranchAddress("mdt_posX", mdt_posX, &b_mdt_posX);
   fChain->SetBranchAddress("mdt_posY", mdt_posY, &b_mdt_posY);
   fChain->SetBranchAddress("mdt_posZ", mdt_posZ, &b_mdt_posZ);
   fChain->SetBranchAddress("mdt_gPosX", mdt_gPosX, &b_mdt_gPosX);
   fChain->SetBranchAddress("mdt_gPosY", mdt_gPosY, &b_mdt_gPosY);
   fChain->SetBranchAddress("mdt_gPosZ", mdt_gPosZ, &b_mdt_gPosZ);
   fChain->SetBranchAddress("mdt_closesApproachX", mdt_closesApproachX, &b_mdt_closesApproachX);
   fChain->SetBranchAddress("mdt_closesApproachY", mdt_closesApproachY, &b_mdt_closesApproachY);
   fChain->SetBranchAddress("mdt_closesApproachZ", mdt_closesApproachZ, &b_mdt_closesApproachZ);
   fChain->SetBranchAddress("mdt_gClosesApproachX", mdt_gClosesApproachX, &b_mdt_gClosesApproachX);
   fChain->SetBranchAddress("mdt_gClosesApproachY", mdt_gClosesApproachY, &b_mdt_gClosesApproachY);
   fChain->SetBranchAddress("mdt_gClosesApproachZ", mdt_gClosesApproachZ, &b_mdt_gClosesApproachZ);
   fChain->SetBranchAddress("mdt_temperature_time", mdt_temperature_time, &b_mdt_temperature_time);
   fChain->SetBranchAddress("mdt_wire_sag_time", mdt_wire_sag_time, &b_mdt_wire_sag_time);
   fChain->SetBranchAddress("mdt_tube_t0", mdt_tube_t0, &b_mdt_tube_t0);
   fChain->SetBranchAddress("mdt_xtwin", mdt_xtwin, &b_mdt_xtwin);
   fChain->SetBranchAddress("mdt_segmentT0Applied", mdt_segmentT0Applied, &b_mdt_segmentT0Applied);
   fChain->SetBranchAddress("rpcOs_nRpcHits", &rpcOs_nRpcHits, &b_rpcOs_nRpcHits);
   fChain->SetBranchAddress("rpcOs_segIndex", rpcOs_segIndex, &b_rpcOs_segIndex);
   fChain->SetBranchAddress("rpcOs_id", rpcOs_id, &b_rpcOs_id);
   fChain->SetBranchAddress("rpcOs_nStrips", rpcOs_nStrips, &b_rpcOs_nStrips);
   fChain->SetBranchAddress("rpcOs_stripWidth", rpcOs_stripWidth, &b_rpcOs_stripWidth);
   fChain->SetBranchAddress("rpcOs_stripLength", rpcOs_stripLength, &b_rpcOs_stripLength);
   fChain->SetBranchAddress("rpcOs_error", rpcOs_error, &b_rpcOs_error);
   fChain->SetBranchAddress("rpcOs_time", rpcOs_time, &b_rpcOs_time);
   fChain->SetBranchAddress("rpcOs_posX", rpcOs_posX, &b_rpcOs_posX);
   fChain->SetBranchAddress("rpcOs_posY", rpcOs_posY, &b_rpcOs_posY);
   fChain->SetBranchAddress("rpcOs_posZ", rpcOs_posZ, &b_rpcOs_posZ);
   fChain->SetBranchAddress("rpcOs_gPosX", rpcOs_gPosX, &b_rpcOs_gPosX);
   fChain->SetBranchAddress("rpcOs_gPosY", rpcOs_gPosY, &b_rpcOs_gPosY);
   fChain->SetBranchAddress("rpcOs_gPosZ", rpcOs_gPosZ, &b_rpcOs_gPosZ);
   fChain->SetBranchAddress("rpcOs_distanceToRO", rpcOs_distanceToRO, &b_rpcOs_distanceToRO);
   fChain->SetBranchAddress("tgc_nTgc", &tgc_nTgc, &b_tgc_nTgc);
   fChain->SetBranchAddress("tgc_segIndex", tgc_segIndex, &b_tgc_segIndex);
   fChain->SetBranchAddress("tgc_id", tgc_id, &b_tgc_id);
   fChain->SetBranchAddress("tgc_nStrips", tgc_nStrips, &b_tgc_nStrips);
   fChain->SetBranchAddress("tgc_stripWidth", tgc_stripWidth, &b_tgc_stripWidth);
   fChain->SetBranchAddress("tgc_stripLength", tgc_stripLength, &b_tgc_stripLength);
   fChain->SetBranchAddress("tgc_error", tgc_error, &b_tgc_error);
   fChain->SetBranchAddress("tgc_posX", tgc_posX, &b_tgc_posX);
   fChain->SetBranchAddress("tgc_posY", tgc_posY, &b_tgc_posY);
   fChain->SetBranchAddress("tgc_posZ", tgc_posZ, &b_tgc_posZ);
   fChain->SetBranchAddress("tgc_gPosX", tgc_gPosX, &b_tgc_gPosX);
   fChain->SetBranchAddress("tgc_gPosY", tgc_gPosY, &b_tgc_gPosY);
   fChain->SetBranchAddress("tgc_gPosZ", tgc_gPosZ, &b_tgc_gPosZ);
   fChain->SetBranchAddress("csc_nCsc", &csc_nCsc, &b_csc_nCsc);
   fChain->SetBranchAddress("csc_segIndex", csc_segIndex, &b_csc_segIndex);
   fChain->SetBranchAddress("csc_id", csc_id, &b_csc_id);
   fChain->SetBranchAddress("csc_nStrips", csc_nStrips, &b_csc_nStrips);
   fChain->SetBranchAddress("csc_stripWidth", csc_stripWidth, &b_csc_stripWidth);
   fChain->SetBranchAddress("csc_charge", csc_charge, &b_csc_charge);
   fChain->SetBranchAddress("csc_error", csc_error, &b_csc_error);
   fChain->SetBranchAddress("csc_posX", csc_posX, &b_csc_posX);
   fChain->SetBranchAddress("csc_posY", csc_posY, &b_csc_posY);
   fChain->SetBranchAddress("csc_posZ", csc_posZ, &b_csc_posZ);
   fChain->SetBranchAddress("csc_gPosX", csc_gPosX, &b_csc_gPosX);
   fChain->SetBranchAddress("csc_gPosY", csc_gPosY, &b_csc_gPosY);
   fChain->SetBranchAddress("csc_gPosZ", csc_gPosZ, &b_csc_gPosZ);
   fChain->SetBranchAddress("rawMdt_nRMdt", &rawMdt_nRMdt, &b_rawMdt_nRMdt);
   fChain->SetBranchAddress("rawMdt_id", rawMdt_id, &b_rawMdt_id);
   fChain->SetBranchAddress("rawMdt_tdc", rawMdt_tdc, &b_rawMdt_tdc);
   fChain->SetBranchAddress("rawMdt_adc", rawMdt_adc, &b_rawMdt_adc);
   fChain->SetBranchAddress("rawMdt_gPosX", rawMdt_gPosX, &b_rawMdt_gPosX);
   fChain->SetBranchAddress("rawMdt_gPosY", rawMdt_gPosY, &b_rawMdt_gPosY);
   fChain->SetBranchAddress("rawMdt_gPosZ", rawMdt_gPosZ, &b_rawMdt_gPosZ);
   fChain->SetBranchAddress("rawRpc_nRRpc", &rawRpc_nRRpc, &b_rawRpc_nRRpc);
   fChain->SetBranchAddress("rawRpc_occupancy", rawRpc_occupancy, &b_rawRpc_occupancy);
   fChain->SetBranchAddress("rawRpc_id", rawRpc_id, &b_rawRpc_id);
   fChain->SetBranchAddress("rawRpc_t", rawRpc_t, &b_rawRpc_t);
   fChain->SetBranchAddress("rawRpc_width", rawRpc_width, &b_rawRpc_width);
   fChain->SetBranchAddress("rawRpc_length", rawRpc_length, &b_rawRpc_length);
   fChain->SetBranchAddress("rawRpc_gPosX", rawRpc_gPosX, &b_rawRpc_gPosX);
   fChain->SetBranchAddress("rawRpc_gPosY", rawRpc_gPosY, &b_rawRpc_gPosY);
   fChain->SetBranchAddress("rawRpc_gPosZ", rawRpc_gPosZ, &b_rawRpc_gPosZ);
   fChain->SetBranchAddress("rawRpcCoin_nRpcCoin", &rawRpcCoin_nRpcCoin, &b_rawRpcCoin_nRpcCoin);
   fChain->SetBranchAddress("rawRpcCoin_occupancy", rawRpcCoin_occupancy, &b_rawRpcCoin_occupancy);
   fChain->SetBranchAddress("rawRpcCoin_id", rawRpcCoin_id, &b_rawRpcCoin_id);
   fChain->SetBranchAddress("rawRpcCoin_t", rawRpcCoin_t, &b_rawRpcCoin_t);
   fChain->SetBranchAddress("rawRpcCoin_width", rawRpcCoin_width, &b_rawRpcCoin_width);
   fChain->SetBranchAddress("rawRpcCoin_length", rawRpcCoin_length, &b_rawRpcCoin_length);
   fChain->SetBranchAddress("rawRpcCoin_gPosX", rawRpcCoin_gPosX, &b_rawRpcCoin_gPosX);
   fChain->SetBranchAddress("rawRpcCoin_gPosY", rawRpcCoin_gPosY, &b_rawRpcCoin_gPosY);
   fChain->SetBranchAddress("rawRpcCoin_gPosZ", rawRpcCoin_gPosZ, &b_rawRpcCoin_gPosZ);
   fChain->SetBranchAddress("rawRpcCoin_ijk", rawRpcCoin_ijk, &b_rawRpcCoin_ijk);
   fChain->SetBranchAddress("rawRpcCoin_threshold", rawRpcCoin_threshold, &b_rawRpcCoin_threshold);
   fChain->SetBranchAddress("rawRpcCoin_overlap", rawRpcCoin_overlap, &b_rawRpcCoin_overlap);
   fChain->SetBranchAddress("rawRpcCoin_parentCmId", rawRpcCoin_parentCmId, &b_rawRpcCoin_parentCmId);
   fChain->SetBranchAddress("rawRpcCoin_parentPadId", rawRpcCoin_parentPadId, &b_rawRpcCoin_parentPadId);
   fChain->SetBranchAddress("rawRpcCoin_parentSectorId", rawRpcCoin_parentSectorId, &b_rawRpcCoin_parentSectorId);
   fChain->SetBranchAddress("rawRpcCoin_lowPtCm", rawRpcCoin_lowPtCm, &b_rawRpcCoin_lowPtCm);
   fChain->SetBranchAddress("rawCsc_nRCsc", &rawCsc_nRCsc, &b_rawCsc_nRCsc);
   fChain->SetBranchAddress("rawCsc_occupancy", rawCsc_occupancy, &b_rawCsc_occupancy);
   fChain->SetBranchAddress("rawCsc_id", rawCsc_id, &b_rawCsc_id);
   fChain->SetBranchAddress("rawCsc_width", rawCsc_width, &b_rawCsc_width);
   fChain->SetBranchAddress("rawCsc_charge", rawCsc_charge, &b_rawCsc_charge);
   fChain->SetBranchAddress("rawCsc_t", rawCsc_t, &b_rawCsc_t);
   fChain->SetBranchAddress("rawCsc_gPosX", rawCsc_gPosX, &b_rawCsc_gPosX);
   fChain->SetBranchAddress("rawCsc_gPosY", rawCsc_gPosY, &b_rawCsc_gPosY);
   fChain->SetBranchAddress("rawCsc_gPosZ", rawCsc_gPosZ, &b_rawCsc_gPosZ);
   fChain->SetBranchAddress("rpc_SL_nSectors", &rpc_SL_nSectors, &b_rpc_SL_nSectors);
   fChain->SetBranchAddress("rpc_SL_sectorId", rpc_SL_sectorId, &b_rpc_SL_sectorId);
   fChain->SetBranchAddress("rpc_SL_lvl1id", rpc_SL_lvl1id, &b_rpc_SL_lvl1id);
   fChain->SetBranchAddress("rpc_SL_bcid", rpc_SL_bcid, &b_rpc_SL_bcid);
   fChain->SetBranchAddress("rpc_SL_errorCode", rpc_SL_errorCode, &b_rpc_SL_errorCode);
   fChain->SetBranchAddress("rpc_SL_crc", rpc_SL_crc, &b_rpc_SL_crc);
   fChain->SetBranchAddress("rpc_SL_hasMoreThan2TriggerCand", rpc_SL_hasMoreThan2TriggerCand, &b_rpc_SL_hasMoreThan2TriggerCand);
   fChain->SetBranchAddress("rpc_SLnTriggerHits", &rpc_SLnTriggerHits, &b_rpc_SLnTriggerHits);
   fChain->SetBranchAddress("rpc_SLhit_sector", rpc_SLhit_sector, &b_rpc_SLhit_sector);
   fChain->SetBranchAddress("rpc_SLhit_rowinBcid", rpc_SLhit_rowinBcid, &b_rpc_SLhit_rowinBcid);
   fChain->SetBranchAddress("rpc_SLhit_padId", rpc_SLhit_padId, &b_rpc_SLhit_padId);
   fChain->SetBranchAddress("rpc_SLhit_ptId", rpc_SLhit_ptId, &b_rpc_SLhit_ptId);
   fChain->SetBranchAddress("rpc_SLhit_roi", rpc_SLhit_roi, &b_rpc_SLhit_roi);
   fChain->SetBranchAddress("rpc_SLhit_outerPlane", rpc_SLhit_outerPlane, &b_rpc_SLhit_outerPlane);
   fChain->SetBranchAddress("rpc_SLhit_overlapPhi", rpc_SLhit_overlapPhi, &b_rpc_SLhit_overlapPhi);
   fChain->SetBranchAddress("rpc_SLhit_overlapEta", rpc_SLhit_overlapEta, &b_rpc_SLhit_overlapEta);
   fChain->SetBranchAddress("rpc_SLhit_triggerBcid", rpc_SLhit_triggerBcid, &b_rpc_SLhit_triggerBcid);
   fChain->SetBranchAddress("rpc_SLhit_isInput", rpc_SLhit_isInput, &b_rpc_SLhit_isInput);
   fChain->SetBranchAddress("rpc_fchan", &rpc_fchan, &b_rpc_fchan);
   fChain->SetBranchAddress("rpc_sector", rpc_sector, &b_rpc_sector);
   fChain->SetBranchAddress("rpc_padId", rpc_padId, &b_rpc_padId);
   fChain->SetBranchAddress("rpc_status", rpc_status, &b_rpc_status);
   fChain->SetBranchAddress("rpc_ercode", rpc_ercode, &b_rpc_ercode);
   fChain->SetBranchAddress("rpc_cmaId", rpc_cmaId, &b_rpc_cmaId);
   fChain->SetBranchAddress("rpc_fel1Id", rpc_fel1Id, &b_rpc_fel1Id);
   fChain->SetBranchAddress("rpc_febcId", rpc_febcId, &b_rpc_febcId);
   fChain->SetBranchAddress("rpc_crc", rpc_crc, &b_rpc_crc);
   fChain->SetBranchAddress("rpc_bcId", rpc_bcId, &b_rpc_bcId);
   fChain->SetBranchAddress("rpc_ticks", rpc_ticks, &b_rpc_ticks);
   fChain->SetBranchAddress("rpc_ijk", rpc_ijk, &b_rpc_ijk);
   fChain->SetBranchAddress("rpc_cmachan", rpc_cmachan, &b_rpc_cmachan);
   fChain->SetBranchAddress("rpc_overlap", rpc_overlap, &b_rpc_overlap);
   fChain->SetBranchAddress("rpc_threshold", rpc_threshold, &b_rpc_threshold);
   //   fChain->SetBranchAddress("phi_nPhiHits", &phi_nPhiHits, &b_phi_nPhiHits);
   //   fChain->SetBranchAddress("phi_phiPatIndex", &phi_phiPatIndex, &b_phi_phiPatIndex);
   //   fChain->SetBranchAddress("phi_id", &phi_id, &b_phi_id);
   //   fChain->SetBranchAddress("phi_posX", &phi_posX, &b_phi_posX);
   //   fChain->SetBranchAddress("phi_posY", &phi_posY, &b_phi_posY);
   //   fChain->SetBranchAddress("phi_posZ", &phi_posZ, &b_phi_posZ);
   //   fChain->SetBranchAddress("phi_error", &phi_error, &b_phi_error);
   fChain->SetBranchAddress("trkHit_nHits", &trkHit_nHits, &b_trkHit_nHits);
   fChain->SetBranchAddress("trkHit_trackIndex", trkHit_trackIndex, &b_trkHit_trackIndex);
   fChain->SetBranchAddress("trkHit_id", trkHit_id, &b_trkHit_id);
   fChain->SetBranchAddress("trkHit_posX", trkHit_posX, &b_trkHit_posX);
   fChain->SetBranchAddress("trkHit_posY", trkHit_posY, &b_trkHit_posY);
   fChain->SetBranchAddress("trkHit_posZ", trkHit_posZ, &b_trkHit_posZ);
   fChain->SetBranchAddress("trkHit_driftRadius", trkHit_driftRadius, &b_trkHit_driftRadius);
   fChain->SetBranchAddress("trkHit_error", trkHit_error, &b_trkHit_error);
   fChain->SetBranchAddress("trkHit_resi", trkHit_resi, &b_trkHit_resi);
   fChain->SetBranchAddress("trkHit_pull", trkHit_pull, &b_trkHit_pull);
   fChain->SetBranchAddress("trkHit_type", trkHit_type, &b_trkHit_type);
   //   fChain->SetBranchAddress("trkHole_nHoles", &trkHole_nHoles, &b_trkHole_nHoles);
   //   fChain->SetBranchAddress("trkHole_trackIndex", trkHole_trackIndex, &b_trkHole_trackIndex);
   //   fChain->SetBranchAddress("trkHole_id", trkHole_id, &b_trkHole_id);
   //   fChain->SetBranchAddress("trkHole_posX", trkHole_posX, &b_trkHole_posX);
   //   fChain->SetBranchAddress("trkHole_posY", trkHole_posY, &b_trkHole_posY);
   //   fChain->SetBranchAddress("trkHole_posZ", trkHole_posZ, &b_trkHole_posZ);
   fChain->SetBranchAddress("trk_nTracks", &trk_nTracks, &b_trk_nTracks);
   fChain->SetBranchAddress("trk_x0", trk_x0, &b_trk_x0);
   fChain->SetBranchAddress("trk_y0", trk_y0, &b_trk_y0);
   fChain->SetBranchAddress("trk_z0", trk_z0, &b_trk_z0);
   fChain->SetBranchAddress("trk_phi", trk_phi, &b_trk_phi);
   fChain->SetBranchAddress("trk_theta", trk_theta, &b_trk_theta);
   fChain->SetBranchAddress("trk_qOverP", trk_qOverP, &b_trk_qOverP);
   fChain->SetBranchAddress("trk_author", trk_author, &b_trk_author);
   fChain->SetBranchAddress("trk_cov00", trk_cov00, &b_trk_cov00);
   fChain->SetBranchAddress("trk_cov01", trk_cov01, &b_trk_cov01);
   fChain->SetBranchAddress("trk_cov02", trk_cov02, &b_trk_cov02);
   fChain->SetBranchAddress("trk_cov03", trk_cov03, &b_trk_cov03);
   fChain->SetBranchAddress("trk_cov04", trk_cov04, &b_trk_cov04);
   fChain->SetBranchAddress("trk_cov11", trk_cov11, &b_trk_cov11);
   fChain->SetBranchAddress("trk_cov12", trk_cov12, &b_trk_cov12);
   fChain->SetBranchAddress("trk_cov13", trk_cov13, &b_trk_cov13);
   fChain->SetBranchAddress("trk_cov14", trk_cov14, &b_trk_cov14);
   fChain->SetBranchAddress("trk_cov22", trk_cov22, &b_trk_cov22);
   fChain->SetBranchAddress("trk_cov23", trk_cov23, &b_trk_cov23);
   fChain->SetBranchAddress("trk_cov24", trk_cov24, &b_trk_cov24);
   fChain->SetBranchAddress("trk_cov33", trk_cov33, &b_trk_cov33);
   fChain->SetBranchAddress("trk_cov34", trk_cov34, &b_trk_cov34);
   fChain->SetBranchAddress("trk_cov44", trk_cov44, &b_trk_cov44);
   fChain->SetBranchAddress("trk_chi2", trk_chi2, &b_trk_chi2);
   fChain->SetBranchAddress("trk_ndof", trk_ndof, &b_trk_ndof);
   fChain->SetBranchAddress("trkSeg_nTrkSegs", &trkSeg_nTrkSegs, &b_trkSeg_nTrkSegs);
   fChain->SetBranchAddress("trkSeg_trkIndex", trkSeg_trkIndex, &b_trkSeg_trkIndex);
   fChain->SetBranchAddress("trkSeg_segIndex", trkSeg_segIndex, &b_trkSeg_segIndex);
   fChain->SetBranchAddress("muctpi_nHits", &muctpi_nHits, &b_muctpi_nHits);
   fChain->SetBranchAddress("muctpi_roiWord", muctpi_roiWord, &b_muctpi_roiWord);
   fChain->SetBranchAddress("muctpi_bcID", muctpi_bcID, &b_muctpi_bcID);
   fChain->SetBranchAddress("muctpi_sysID", muctpi_sysID, &b_muctpi_sysID);
   fChain->SetBranchAddress("muctpi_subsysID", muctpi_subsysID, &b_muctpi_subsysID);
   fChain->SetBranchAddress("muctpi_sectorID", muctpi_sectorID, &b_muctpi_sectorID);
   fChain->SetBranchAddress("muctpi_thresholdNumber", muctpi_thresholdNumber, &b_muctpi_thresholdNumber);
   fChain->SetBranchAddress("muctpi_thresholdValue", muctpi_thresholdValue, &b_muctpi_thresholdValue);
   fChain->SetBranchAddress("muctpi_roINumber", muctpi_roINumber, &b_muctpi_roINumber);
   fChain->SetBranchAddress("muctpi_overlap", muctpi_overlap, &b_muctpi_overlap);
   fChain->SetBranchAddress("muctpi_firstCandidate", muctpi_firstCandidate, &b_muctpi_firstCandidate);
   fChain->SetBranchAddress("muctpi_sectorOverflow", muctpi_sectorOverflow, &b_muctpi_sectorOverflow);
   fChain->SetBranchAddress("muctpi_padOverflow", muctpi_padOverflow, &b_muctpi_padOverflow);
   fChain->SetBranchAddress("muctpi_phi", muctpi_phi, &b_muctpi_phi);
   fChain->SetBranchAddress("muctpi_eta", muctpi_eta, &b_muctpi_eta);
   fChain->SetBranchAddress("mbts_nHits", &mbts_nHits, &b_mbts_nHits);
   fChain->SetBranchAddress("mbts_id", mbts_id, &b_mbts_id);
   fChain->SetBranchAddress("mbts_posX", mbts_posX, &b_mbts_posX);
   fChain->SetBranchAddress("mbts_posY", mbts_posY, &b_mbts_posY);
   fChain->SetBranchAddress("mbts_posZ", mbts_posZ, &b_mbts_posZ);
   fChain->SetBranchAddress("mbts_time", mbts_time, &b_mbts_time);
   fChain->SetBranchAddress("mbts_charge", mbts_charge, &b_mbts_charge);
   fChain->SetBranchAddress("calo_nHits", &calo_nHits, &b_calo_nHits);
   fChain->SetBranchAddress("calo_id", calo_id, &b_calo_id);
   fChain->SetBranchAddress("calo_posX", calo_posX, &b_calo_posX);
   fChain->SetBranchAddress("calo_posY", calo_posY, &b_calo_posY);
   fChain->SetBranchAddress("calo_posZ", calo_posZ, &b_calo_posZ);
   fChain->SetBranchAddress("calo_time", calo_time, &b_calo_time);
   fChain->SetBranchAddress("calo_charge", calo_charge, &b_calo_charge);
   fChain->SetBranchAddress("ctp_nTriggerInfo", &ctp_nTriggerInfo, &b_ctp_nTriggerInfo);
   fChain->SetBranchAddress("ctp_numberBC", &ctp_numberBC, &b_ctp_numberBC);
   fChain->SetBranchAddress("ctp_L1A_BC", &ctp_L1A_BC, &b_ctp_L1A_BC);
   fChain->SetBranchAddress("ctp_timeNs", &ctp_timeNs, &b_ctp_timeNs);
   fChain->SetBranchAddress("ctp_randomTrig", &ctp_randomTrig, &b_ctp_randomTrig);
   fChain->SetBranchAddress("ctp_firedItemsBeforePrescale", &ctp_firedItemsBeforePrescale, &b_ctp_firedItemsBeforePrescale);
   fChain->SetBranchAddress("ctp_prescaledClock", &ctp_prescaledClock, &b_ctp_prescaledClock);
   fChain->SetBranchAddress("ctp_bcid", ctp_bcid, &b_ctp_bcid);
   fChain->SetBranchAddress("ctp_bcIndexTriggerItems", ctp_bcIndexTriggerItems, &b_ctp_bcIndexTriggerItems);
   fChain->SetBranchAddress("ctp_bcIndexPIT", ctp_bcIndexPIT, &b_ctp_bcIndexPIT);
   fChain->SetBranchAddress("ctp_pit", ctp_pit, &b_ctp_pit);
   fChain->SetBranchAddress("ctp_tbp", ctp_tbp, &b_ctp_tbp);
   fChain->SetBranchAddress("ctp_tap", ctp_tap, &b_ctp_tap);
   fChain->SetBranchAddress("ctp_tav", ctp_tav, &b_ctp_tav);
   fChain->SetBranchAddress("ctp_type", ctp_type, &b_ctp_type);
   fChain->SetBranchAddress("ctp_delay", ctp_delay, &b_ctp_delay);
}

Bool_t selector::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

#endif // #ifdef selector_cxx
