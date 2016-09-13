
#include <memory>
#include <algorithm>


#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"

#include "DataFormats/JetReco/interface/BasicJet.h"
#include "DataFormats/JetReco/interface/BasicJetCollection.h"

#include "DataFormats/METReco/interface/MET.h"

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "CommonTools/Utils/interface/PtComparator.h"
#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"

#include "TLorentzVector.h"

class prodJets : public edm::EDFilter 
{
 public:

  explicit prodJets(const edm::ParameterSet & iConfig);
  ~prodJets();

 private:

  virtual bool filter(edm::Event & iEvent, const edm::EventSetup & iSetup);

  edm::InputTag jetSrc_, jetOtherSrc_;
// All have to be pat::Jet, otherwise cannot get b-tagging information!
  edm::Handle<std::vector<pat::Jet> > jets, otherjets; 
  std::string bTagKeyString_;
  edm::InputTag vtxSrc_;
  edm::InputTag metSrc_;
  bool isPatJet;
  bool debug_;

  bool isData_;

  double jetPtCut_miniAOD_, genMatch_dR_;
  double relPt_for_xCheck_, dR_for_xCheck_;

  edm::EDGetTokenT<std::vector<pat::Jet> >JetTok_;
  edm::EDGetTokenT<std::vector<pat::Jet> >OtherJetsTok_;
  edm::EDGetTokenT<std::vector<int> > W_EmuVec_Tok_;
  edm::EDGetTokenT<std::vector<int> >W_TauVec_Tok_;
  edm::EDGetTokenT<std::vector<int> >W_Tau_EmuVec_Tok_;
  edm::EDGetTokenT<std::vector<int> >W_Tau_ProngsVec_Tok_;
  edm::EDGetTokenT<std::vector<int> >W_Tau_NuVec_Tok_;
  edm::EDGetTokenT<std::vector<TLorentzVector> >GenDecayLVec_Tok_;
  edm::EDGetTokenT<std::vector<int> >GenDecayMomRefVec_Tok_;
  edm::EDGetTokenT<std::vector<TLorentzVector> >EleLVec_Tok_;
  edm::EDGetTokenT<std::vector<TLorentzVector> >MuLVec_Tok_;
  edm::EDGetTokenT<std::vector<TLorentzVector> >TrksForIsoVetolVec_Tok_;
  edm::EDGetTokenT<std::vector<TLorentzVector> >LooseIsoTrksVec_Tok_;
  edm::EDGetTokenT< std::vector<reco::Vertex> >VtxTok_;
  edm::EDGetTokenT<std::vector<pat::Jet>> PuppiJetsSrc_Tok_;
  edm::EDGetTokenT<std::vector<pat::Jet>> Ak8Jets_Tok_;

  edm::InputTag W_emuVec_Src_, W_tauVec_Src_, W_tau_emuVec_Src_, W_tau_prongsVec_Src_, W_tau_nuVec_Src_;
  edm::Handle<std::vector<int> > W_emuVec_, W_tauVec_, W_tau_emuVec_, W_tau_prongsVec_, W_tau_nuVec_;

  edm::InputTag genDecayLVec_Src_;
  edm::Handle<std::vector<TLorentzVector> > genDecayLVec_;

  edm::InputTag genDecayMomRefVec_Src_;
  edm::Handle<std::vector<int> > genDecayMomRefVec_;

  edm::InputTag eleLVec_Src_, muLVec_Src_;
  edm::Handle<std::vector<TLorentzVector> > eleLVec_, muLVec_;

  edm::InputTag trksForIsoVetoLVec_Src_, looseisoTrksLVec_Src_;
  edm::Handle<std::vector<TLorentzVector> > trksForIsoVetoLVec_, looseisoTrksLVec_;
  double deltaRcon_;

  //PUPPI sources
  edm::InputTag puppiJetsSrc_;
  edm::Handle<std::vector<pat::Jet> > puppiJets;
 
  //AK8 Jets
  edm::InputTag ak8JetsSrc_;
  edm::Handle<std::vector<pat::Jet> > ak8Jets;

  std::string jetType_;
};


prodJets::prodJets(const edm::ParameterSet & iConfig) 
{
  isData_ = true;

  jetSrc_      = iConfig.getParameter<edm::InputTag>("jetSrc");
  jetOtherSrc_ = iConfig.getParameter<edm::InputTag>("jetOtherSrc");
  vtxSrc_      = iConfig.getParameter<edm::InputTag>("vtxSrc");
//  metSrc_      = iConfig.getParameter<edm::InputTag>("metSrc");
  bTagKeyString_ = iConfig.getParameter<std::string>("bTagKeyString");

  debug_       = iConfig.getParameter<bool>("debug");

  jetPtCut_miniAOD_ = iConfig.getUntrackedParameter<double>("jetPtCut_miniAOD", 10);
  genMatch_dR_ = iConfig.getUntrackedParameter<double>("genMatch_dR", 1.0);
  dR_for_xCheck_ = iConfig.getUntrackedParameter<double>("dR_for_xCheck", 0.2);
  relPt_for_xCheck_ = iConfig.getUntrackedParameter<double>("relPt_for_xCheck", 1e-2);

  W_emuVec_Src_ = iConfig.getParameter<edm::InputTag>("W_emuVec");
  W_tauVec_Src_ = iConfig.getParameter<edm::InputTag>("W_tauVec");
  W_tau_emuVec_Src_ = iConfig.getParameter<edm::InputTag>("W_tau_emuVec");
  W_tau_prongsVec_Src_ = iConfig.getParameter<edm::InputTag>("W_tau_prongsVec");
  W_tau_nuVec_Src_ = iConfig.getParameter<edm::InputTag>("W_tau_nuVec");

  genDecayLVec_Src_ = iConfig.getParameter<edm::InputTag>("genDecayLVec");

  genDecayMomRefVec_Src_ = iConfig.getParameter<edm::InputTag>("genDecayMomRefVec");

  eleLVec_Src_ = iConfig.getParameter<edm::InputTag>("eleLVec");
  muLVec_Src_ = iConfig.getParameter<edm::InputTag>("muLVec");
  
  trksForIsoVetoLVec_Src_ = iConfig.getParameter<edm::InputTag>("trksForIsoVetoLVec");
  looseisoTrksLVec_Src_ = iConfig.getParameter<edm::InputTag>("looseisoTrksLVec");

  deltaRcon_ = iConfig.getUntrackedParameter<double>("deltaRcon", 0.01);

  jetType_ = iConfig.getParameter<std::string>("jetType");
  
  //Puppi source
  puppiJetsSrc_ = iConfig.getParameter<edm::InputTag>("puppiJetsSrc");
  //ak8 jets
  ak8JetsSrc_ = iConfig.getParameter<edm::InputTag>("ak8JetsSrc");

  JetTok_ = consumes<std::vector<pat::Jet> >(jetSrc_);
  OtherJetsTok_ = consumes<std::vector<pat::Jet> >(jetOtherSrc_);
  W_EmuVec_Tok_=consumes<std::vector<int> >(W_emuVec_Src_);
  W_TauVec_Tok_=consumes<std::vector<int> >(W_tauVec_Src_);
  W_Tau_EmuVec_Tok_=consumes<std::vector<int> >(W_tau_emuVec_Src_);
  W_Tau_ProngsVec_Tok_ = consumes<std::vector<int> >(W_tau_prongsVec_Src_);
  W_Tau_NuVec_Tok_ = consumes<std::vector<int> >(W_tau_nuVec_Src_);
  GenDecayLVec_Tok_=consumes<std::vector<TLorentzVector> >(genDecayLVec_Src_);
  GenDecayMomRefVec_Tok_=consumes<std::vector<int> >(genDecayMomRefVec_Src_);
  EleLVec_Tok_=consumes<std::vector<TLorentzVector> >(eleLVec_Src_);
  MuLVec_Tok_=consumes<std::vector<TLorentzVector> >(muLVec_Src_);
  TrksForIsoVetolVec_Tok_=consumes<std::vector<TLorentzVector> >(trksForIsoVetoLVec_Src_);
  LooseIsoTrksVec_Tok_=consumes<std::vector<TLorentzVector> >(looseisoTrksLVec_Src_);
  VtxTok_=consumes< std::vector<reco::Vertex> >(vtxSrc_);
  PuppiJetsSrc_Tok_ = consumes<std::vector<pat::Jet>>(puppiJetsSrc_);
  Ak8Jets_Tok_ = consumes<std::vector<pat::Jet>>(ak8JetsSrc_);

  //produces<std::vector<pat::Jet> >("");
  produces<std::vector<TLorentzVector> >("jetsLVec");
  produces<std::vector<int> >("recoJetsFlavor");
  produces<std::vector<double> >("recoJetsBtag");
  produces<std::vector<double> >("recoJetsJecUnc");
  produces<std::vector<double> >("recoJetsJecScaleRawToFull");
  produces<int>("nJets");

  //produce variables needed for Lost Lepton study, added by hua.wei@cern.ch
  produces<std::vector<double> >("recoJetschargedHadronEnergyFraction");
  produces<std::vector<double> >("recoJetschargedEmEnergyFraction");
  produces<std::vector<double> >("recoJetsneutralEmEnergyFraction");

  produces<std::vector<double> >("recoJetsmuonEnergyFraction");

  produces<std::vector<int> >("muMatchedJetIdx");
  produces<std::vector<int> >("eleMatchedJetIdx");

  produces<std::vector<int> >("trksForIsoVetoMatchedJetIdx");
  produces<std::vector<int> >("looseisoTrksMatchedJetIdx");

  //PUPPI
  produces<std::vector<TLorentzVector> >("puppiJetsLVec");
  //ak8
  produces<std::vector<TLorentzVector> >("ak8JetsLVec");
  
  produces<std::vector<double> >("tau1");
  produces<std::vector<double> >("subjetBdisc");
  produces<std::vector<double> >("subjetBdiscp");
  produces<std::vector<double> >("ak8pt");
  produces<std::vector<double> >("ak8mass");
  produces<std::vector<double> >("ak8rapidity");
  produces<std::vector<double> >("prunedMass");
  produces<std::vector<double> >("softDropMass");
  produces<std::vector<double> >("tau2");
  produces<std::vector<double> >("tau3");
  //produces<std::vector<double> >("tau21");
  //produces<std::vector<double> >("tau32");
  produces<std::vector<double> >("puppipt");
  produces<std::vector<double> >("puppimass");
  produces<std::vector<double> >("puppieta");
  produces<std::vector<double> >("puppiphi");
  produces<std::vector<double> >("puppitau1");
  produces<std::vector<double> >("puppitau2");
  produces<std::vector<double> >("puppitau3");
  produces<std::vector<double> >("deltaRpup");

}


prodJets::~prodJets() 
{
}


bool prodJets::filter(edm::Event& iEvent, const edm::EventSetup& iSetup) 
{
  if( !iEvent.isRealData() ) isData_ = false;

  iEvent.getByToken(JetTok_, jets);

  //get the JEC uncertainties
  edm::ESHandle<JetCorrectorParametersCollection> JetCorParColl;
  iSetup.get<JetCorrectionsRecord>().get(jetType_, JetCorParColl);
  JetCorrectorParameters const & JetCorPar = (*JetCorParColl)["Uncertainty"];
  std::auto_ptr<JetCorrectionUncertainty> jecUnc( new JetCorrectionUncertainty(JetCorPar) );

  if( !isData_ ){
     iEvent.getByToken(OtherJetsTok_, otherjets);
     iEvent.getByToken(W_EmuVec_Tok_, W_emuVec_);
     iEvent.getByToken(W_TauVec_Tok_, W_tauVec_);
     iEvent.getByToken(W_Tau_EmuVec_Tok_, W_tau_emuVec_);
     iEvent.getByToken(W_Tau_ProngsVec_Tok_, W_tau_prongsVec_);
     iEvent.getByToken(W_Tau_NuVec_Tok_, W_tau_nuVec_);

     iEvent.getByToken(GenDecayLVec_Tok_, genDecayLVec_);
     iEvent.getByToken(GenDecayMomRefVec_Tok_, genDecayMomRefVec_);
  }

  iEvent.getByToken(EleLVec_Tok_, eleLVec_);
  iEvent.getByToken(MuLVec_Tok_, muLVec_);

  iEvent.getByToken(TrksForIsoVetolVec_Tok_, trksForIsoVetoLVec_);
  iEvent.getByToken(LooseIsoTrksVec_Tok_,looseisoTrksLVec_);

  // read in the objects
  edm::Handle< std::vector<reco::Vertex> > vertices;
  iEvent.getByToken(VtxTok_, vertices);
  // reco::Vertex::Point vtxpos = (vertices->size() > 0 ? (*vertices)[0].position() : reco::Vertex::Point());
//  edm::Handle<edm::View<reco::MET> > met;
//  iEvent.getByLabel(metSrc_, met);

  std::vector<pat::Jet> extJets = (*jets);
  //PUPPI
  iEvent.getByToken(PuppiJetsSrc_Tok_, puppiJets);
  //ak8
  iEvent.getByToken(Ak8Jets_Tok_, ak8Jets);

  // check which ones to keep
  // std::auto_ptr<std::vector<pat::Jet> > prod(new std::vector<pat::Jet>());
  std::auto_ptr<std::vector<TLorentzVector> > jetsLVec(new std::vector<TLorentzVector>());
  std::auto_ptr<std::vector<int> > recoJetsFlavor(new std::vector<int>());
  std::auto_ptr<std::vector<double> > recoJetsBtag(new std::vector<double>());
  std::auto_ptr<std::vector<double> > recoJetsJecUnc(new std::vector<double>());
  std::auto_ptr<std::vector<double> > recoJetsJecScaleRawToFull(new std::vector<double>());

  std::auto_ptr<std::vector<double> > recoJetschargedHadronEnergyFraction(new std::vector<double>());
  std::auto_ptr<std::vector<double> > recoJetschargedEmEnergyFraction(new std::vector<double>());
  std::auto_ptr<std::vector<double> > recoJetsneutralEmEnergyFraction(new std::vector<double>());
  std::auto_ptr<std::vector<double> > recoJetsmuonEnergyFraction(new std::vector<double>());

  std::auto_ptr<std::vector<int> > muMatchedJetIdx(new std::vector<int>(muLVec_->size(), -1));
  std::auto_ptr<std::vector<int> > eleMatchedJetIdx(new std::vector<int>(eleLVec_->size(), -1));

  std::auto_ptr<std::vector<int> > trksForIsoVetoMatchedJetIdx(new std::vector<int>(trksForIsoVetoLVec_->size(), -1));
  std::auto_ptr<std::vector<int> > looseisoTrksMatchedJetIdx(new std::vector<int>(looseisoTrksLVec_->size(), -1));

  //PUPPI
  std::auto_ptr<std::vector<TLorentzVector> > puppiJetsLVec(new std::vector<TLorentzVector>());
  //ak8
  std::auto_ptr<std::vector<TLorentzVector> > ak8JetsLVec(new std::vector<TLorentzVector>());  

  std::auto_ptr<std::vector<double> > tau1(new std::vector<double>());
  std::auto_ptr<std::vector<double> >subjetBdisc(new std::vector<double>());
  std::auto_ptr<std::vector<double> >subjetBdiscp(new std::vector<double>());
  std::auto_ptr<std::vector<double> > ak8pt(new std::vector<double>());
  std::auto_ptr<std::vector<double> > ak8mass(new std::vector<double>());
  std::auto_ptr<std::vector<double> > ak8rapidity(new std::vector<double>());
  std::auto_ptr<std::vector<double> > prunedMass(new std::vector<double>());
  std::auto_ptr<std::vector<double> > softDropMass(new std::vector<double>());
  std::auto_ptr<std::vector<double> > tau2(new std::vector<double>());
  std::auto_ptr<std::vector<double> > tau3(new std::vector<double>());
  //std::auto_ptr<std::vector<double> > tau21(new std::vector<double>());
  //std::auto_ptr<std::vector<double> > tau32(new std::vector<double>());
  std::auto_ptr<std::vector<double> > puppitau1(new std::vector<double>());
  std::auto_ptr<std::vector<double> > puppipt(new std::vector<double>());
  std::auto_ptr<std::vector<double> > puppimass(new std::vector<double>());
  std::auto_ptr<std::vector<double> > puppieta(new std::vector<double>());
  std::auto_ptr<std::vector<double> > puppiphi(new std::vector<double>());
  std::auto_ptr<std::vector<double> > puppitau2(new std::vector<double>());
  std::auto_ptr<std::vector<double> > puppitau3(new std::vector<double>());
  std::auto_ptr<std::vector<double> > deltaRpup(new std::vector<double>());

  if( !isData_ ){
     int cntJetPassPtCut = 0;
     for(unsigned int io=0; io < otherjets->size(); io++){
        const double otjet_pt = otherjets->at(io).pt(), otjet_eta = otherjets->at(io).eta(), otjet_phi = otherjets->at(io).phi();
        TLorentzVector perLVec; perLVec.SetPtEtaPhiE(otjet_pt, otjet_eta, otjet_phi, otherjets->at(io).energy());
        int cntFound = 0, matchedIdx = -1;
        double minDR = 999.0;
        for(unsigned int ij=0; ij< jets->size(); ij++){
           const double jet_eta = jets->at(ij).eta(), jet_phi = jets->at(ij).phi();
           const double dR = reco::deltaR(otjet_eta, otjet_phi, jet_eta, jet_phi);
           if( minDR > dR ){
              minDR = dR; matchedIdx = ij;
           }
        }
        if( matchedIdx != -1 ){
           if( minDR < dR_for_xCheck_ && std::abs(otjet_pt - jets->at(matchedIdx).pt())/jets->at(matchedIdx).pt() < relPt_for_xCheck_ ){
              cntFound ++;
           }
        }
        if( otjet_pt >= jetPtCut_miniAOD_ ){
           cntJetPassPtCut ++;
           if( cntFound != 1 && debug_ ){
              std::cout<<"WARNING ... jet mis-matching between otherjets and jets for pt > "<<jetPtCut_miniAOD_<<"  matchedIdx : "<<matchedIdx<<"  cntFound : "<<cntFound<<"  minDR : "<<minDR<<std::endl;
              std::cout<<"otjet_pt : "<<otjet_pt<<"  otjet_eta : "<<otjet_eta<<"  otjet_phi : "<<otjet_phi<<std::endl;
              if( matchedIdx != -1 ) std::cout<<"  jet_pt : "<<jets->at(matchedIdx).pt()<<"    jet_eta : "<<jets->at(matchedIdx).eta()<<"    jet_phi : "<<jets->at(matchedIdx).phi()<<std::endl;
           }
        }else{
           if( cntFound && debug_ ){
              std::cout<<"WARNING ... otjet with pt : "<<otjet_pt<<"  matching to one of the jets for pt > "<<jetPtCut_miniAOD_<<" ?!"<<std::endl;
              std::cout<<"otjet_pt : "<<otjet_pt<<"  otjet_eta : "<<otjet_eta<<"  otjet_phi : "<<otjet_phi<<std::endl;
              std::cout<<"  jet_pt : "<<jets->at(matchedIdx).pt()<<"    jet_eta : "<<jets->at(matchedIdx).eta()<<"    jet_phi : "<<jets->at(matchedIdx).phi()<<std::endl;
           }else{
              int cntgenMatch = 0;
              for(unsigned int ig=0; ig<W_emuVec_->size(); ig++){
                 int perIdx = W_emuVec_->at(ig);
                 TLorentzVector genLVec = genDecayLVec_->at(perIdx);
                 double perdeltaR = perLVec.DeltaR(genLVec);
                 if( perdeltaR < genMatch_dR_ ) cntgenMatch ++;
              }
              for(unsigned int ig=0; ig<W_tauVec_->size(); ig++){
                 int perIdx = W_tauVec_->at(ig);
                 TLorentzVector genLVec = genDecayLVec_->at(perIdx);
                 double perdeltaR = perLVec.DeltaR(genLVec);
                 if( perdeltaR < genMatch_dR_ ) cntgenMatch ++;
              }
              for(unsigned int ig=0; ig<W_tau_emuVec_->size(); ig++){
                 int perIdx = W_tau_emuVec_->at(ig);
                 TLorentzVector genLVec = genDecayLVec_->at(perIdx);
                 double perdeltaR = perLVec.DeltaR(genLVec);
                 if( perdeltaR < genMatch_dR_ ) cntgenMatch ++;
              }
              for(unsigned int ig=0; ig<W_tau_prongsVec_->size(); ig++){
                 int perIdx = W_tau_prongsVec_->at(ig);
                 TLorentzVector genLVec = genDecayLVec_->at(perIdx);
                 double perdeltaR = perLVec.DeltaR(genLVec);
                 if( perdeltaR < genMatch_dR_ ) cntgenMatch ++;
              }
              for(unsigned int ig=0; ig<W_tauVec_->size(); ig++){
                 int perIdx = W_tauVec_->at(ig);
                 TLorentzVector genLVec = genDecayLVec_->at(perIdx);
                 for(unsigned int in=0; in<W_tau_nuVec_->size(); in++){
                    int perJdx = W_tau_nuVec_->at(in);
                    TLorentzVector gennuLVec = genDecayLVec_->at(perJdx);
   
                    int momIdx = perJdx;
                    bool isFound = false;
                    while( momIdx != -1 ){
                       momIdx = genDecayMomRefVec_->at(momIdx);
                       if( momIdx == perIdx ){ isFound = true; break; }
                    }
                    if( isFound ) genLVec -= gennuLVec;
                 }
                 double perdeltaR = perLVec.DeltaR(genLVec);
                 if( perdeltaR < genMatch_dR_ ) cntgenMatch ++;
              }
              for(unsigned int im=0; im<muLVec_->size(); im++){
                 double perdeltaR = perLVec.DeltaR(muLVec_->at(im));
                 if( perdeltaR < genMatch_dR_ ) cntgenMatch ++;
              }
              for(unsigned int ie=0; ie<eleLVec_->size(); ie++){
                 double perdeltaR = perLVec.DeltaR(eleLVec_->at(ie));
                 if( perdeltaR < genMatch_dR_ ) cntgenMatch ++;
              }
              if( cntgenMatch ){
                 extJets.push_back(otherjets->at(io));
              }
           }
        }
     } 
   
     if( cntJetPassPtCut != (int)jets->size() && debug_ ) std::cout<<"WARNING ... cntJetPassPtCut : "<<cntJetPassPtCut<<"  NOT EQUAL jets->size : "<<jets->size()<<std::endl;
     if( (int)jets->size() >= 4 && std::abs(1.0*cntJetPassPtCut - 1.0*jets->size())/(1.0*jets->size()) > 0.1 && debug_ ){
        std::cout<<"\nWARNING ... cntJetPassPtCut : "<<cntJetPassPtCut<<"  slimmedJets.size : "<<jets->size()<<std::endl;
        std::cout<<"Please checking if global tag used for re-clustering the jets is the same as used to produce the miniAOD!"<<std::endl<<std::endl;
     }
  }
  //PUPPI

  for(unsigned int ip = 0; ip < puppiJets->size(); ip++)
    {

      TLorentzVector perPuppiJetLVec;
      perPuppiJetLVec.SetPtEtaPhiE( puppiJets->at(ip).pt(), puppiJets->at(ip).eta(), puppiJets->at(ip).phi(), puppiJets->at(ip).energy() );
      puppiJetsLVec->push_back(perPuppiJetLVec);

    }
  //Puppi End ************
  //
  //  //ak8

 
    double tau21        = 99;
    double tau32        = 99;
    double puppi_tau21        = 99;
    double puppi_tau32        = 99;
    bool SoftDropTau32Tagged = false;
    //double tau1 = 999;
     
    for(unsigned int ak = 0; ak < ak8Jets->size(); ak++)
    {

      TLorentzVector perak8JetLVec;
      perak8JetLVec.SetPtEtaPhiE( ak8Jets->at(ak).pt(), ak8Jets->at(ak).eta(), ak8Jets->at(ak).phi(), ak8Jets->at(ak).energy() );
      ak8JetsLVec->push_back(perak8JetLVec);
 
   //trying to add Tua tua
    double pt_uf           = ak8Jets->at(ak).pt();
    ak8pt->push_back(pt_uf);
    double mass_uf         = ak8Jets->at(ak).mass();
    ak8mass->push_back(mass_uf);
    double rapidity_uf     = ak8Jets->at(ak).rapidity();
    ak8rapidity->push_back(rapidity_uf);
    //double ndau         = ijet.numberOfDaughters();
    double prunedMass_uf   = ak8Jets->at(ak).userFloat("ak8PFJetsCHSPrunedMass");
    prunedMass->push_back(prunedMass_uf);
    double softDropMass_uf = ak8Jets->at(ak).userFloat("ak8PFJetsCHSSoftDropMass");
    softDropMass->push_back(softDropMass_uf);
    double tau1_uf = ak8Jets->at(ak).userFloat("NjettinessAK8:tau1");
    tau1->push_back(tau1_uf);
    double tau2_uf         = ak8Jets->at(ak).userFloat("NjettinessAK8:tau2");
    tau2->push_back(tau2_uf);
    double tau3_uf         = ak8Jets->at(ak).userFloat("NjettinessAK8:tau3");
    tau3->push_back(tau3_uf);
    //double tau21        = 99;
    //double tau32        = 99;

    double puppi_pt_uf           = ak8Jets->at(ak).userFloat("ak8PFJetsPuppiValueMap:pt");
    puppipt->push_back(puppi_pt_uf);
    double puppi_mass_uf         = ak8Jets->at(ak).userFloat("ak8PFJetsPuppiValueMap:mass");
    puppimass->push_back(puppi_mass_uf);
    double puppi_eta_uf          = ak8Jets->at(ak).userFloat("ak8PFJetsPuppiValueMap:eta");
    puppieta->push_back(puppi_eta_uf);
    double puppi_phi_uf          = ak8Jets->at(ak).userFloat("ak8PFJetsPuppiValueMap:phi");
    puppiphi->push_back(puppi_phi_uf);
    double puppi_tau1_uf         = ak8Jets->at(ak).userFloat("ak8PFJetsPuppiValueMap:NjettinessAK8PuppiTau1");
    puppitau1->push_back(puppi_tau1_uf);
    double puppi_tau2_uf         = ak8Jets->at(ak).userFloat("ak8PFJetsPuppiValueMap:NjettinessAK8PuppiTau2");
    puppitau2->push_back(puppi_tau2_uf);
    double puppi_tau3_uf         = ak8Jets->at(ak).userFloat("ak8PFJetsPuppiValueMap:NjettinessAK8PuppiTau3");
    puppitau3->push_back(puppi_tau3_uf);
    //double puppi_tau21        = 99;
    //double puppi_tau32        = 99;

    //double deltaRpup_uf = deltaR(ak8Jets->at(ak).eta(), ak8Jets->at(ak).phi(), puppi_eta, puppi_phi );
    //deltaRpup->push_back(deltaRpup_uf);
    //if (tau1!=0) double tau21_uf = tau2_uf/tau1_uf;
    //tau21->push_back(tau21_uf);
    //if (tau2!=0) double tau32_uf = tau3_uf/tau2_uf;
    //tau32->push_back(tau32_uf);

    //if (puppi_tau1!=0) double puppi_tau21 = puppi_tau2/puppi_tau1;
    //if (puppi_tau2!=0) double puppi_tau32 = puppi_tau3/puppi_tau2;

    auto const & sdSubjets = ak8Jets->at(ak).subjets("SoftDrop");
    for ( auto const & it : sdSubjets ) {
    double subjetBdisc_uf    = it->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");
    subjetBdisc->push_back(subjetBdisc_uf);
    }   

    auto const & sdSubjetsPuppi = ak8Jets->at(ak).subjets("SoftDropPuppi");
    int count_pup=0;
    for ( auto const & it : sdSubjetsPuppi ) {
      double subjetBdiscp_uf    = it->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");
      subjetBdiscp->push_back(subjetBdiscp_uf);
    }
  
    // Soft Drop + Nsubjettiness tagger
    //bool SoftDropTau32Tagged = false;
    //if (softDropMass<230 && softDropMass>140 && tau32 <0.65) bool SoftDropTau32Tagged = true;

   }

  int cntJetLowPt = 0;
  for(unsigned int ij=0; ij < extJets.size(); ij++)
  {
    const pat::Jet& jet = extJets[ij];

    TLorentzVector perJetLVec;
    perJetLVec.SetPtEtaPhiE( jet.pt(), jet.eta(), jet.phi(), jet.energy() );
    jetsLVec->push_back(perJetLVec);

// Additional jec qualities
    std::vector<std::string> availableJECSets   = jet.availableJECSets();
    std::vector<std::string> availableJECLevels = jet.availableJECLevels(jet.currentJECSet());
    double scaleRawToFull = jet.jecFactor(jet.currentJECLevel(), "none", jet.currentJECSet())/jet.jecFactor("Uncorrected", "none", jet.currentJECSet());
//    double scaleRawToFull = jet.jecFactor(availableJECLevels.back())/jet.jecFactor("Uncorrected");
    recoJetsJecScaleRawToFull->push_back(scaleRawToFull);
    if( debug_ && ij==0 ){
       std::cout<<"\nAvailable JEC sets:"<<"   current : "<<jet.currentJECSet().c_str()<<std::endl;
       for(unsigned int ia=0; ia<availableJECSets.size(); ia++){
          std::cout<<"ia : "<<ia<<"  --> "<<availableJECSets[ia].c_str()<<std::endl;
       }
       std::cout<<"\nAvailable JEC levels:"<<"   current : "<<jet.currentJECLevel().c_str()<<std::endl;
       for(unsigned int ia=0; ia<availableJECLevels.size(); ia++){
          std::cout<<"ia : "<<ia<<"  --> "<<availableJECLevels[ia].c_str()<<std::endl;
       }
       std::cout<<"scaleRawToFull : "<<scaleRawToFull<<"  current : "<<jet.jecFactor(availableJECLevels.back())<<"  uncor : "<<jet.jecFactor("Uncorrected")<<std::endl;
    }

//get JEC unc for this jet, using corrected pT
    jecUnc->setJetEta(jet.eta());
    jecUnc->setJetPt(jet.pt());

    double uncertainty = jecUnc->getUncertainty(true);
//safety check if uncertainty is not available for a jet
    if( uncertainty==-999. ) uncertainty = 0;
    recoJetsJecUnc->push_back(uncertainty);

    if( perJetLVec.Pt() < jetPtCut_miniAOD_ && ij < jets->size() ) cntJetLowPt ++;

    int flavor = jet.partonFlavour();
    recoJetsFlavor->push_back(flavor);

    double btag = jet.bDiscriminator(bTagKeyString_.c_str());
    recoJetsBtag->push_back(btag);

    double chargedHadronEnergyFraction = jet.chargedHadronEnergyFraction();
    recoJetschargedHadronEnergyFraction->push_back( chargedHadronEnergyFraction );

    double chargedEmEnergyFraction = jet.chargedEmEnergyFraction();
    recoJetschargedEmEnergyFraction->push_back( chargedEmEnergyFraction );

    double neutralEmEnergyFraction = jet.neutralEmEnergyFraction();
    recoJetsneutralEmEnergyFraction->push_back( neutralEmEnergyFraction );

    double muonEnergyFraction = jet.muonEnergyFraction();
    recoJetsmuonEnergyFraction->push_back( muonEnergyFraction );

    //std::cout << chargedEmEnergyFraction << std::endl;

//    const std::vector<reco::PFCandidatePtr> & constituents = jet.getPFConstituents();
//    const unsigned int numConstituents = constituents.size();
    const unsigned int numConstituents = jet.numberOfDaughters();

    for(unsigned int im=0; im < muLVec_->size(); im++){
       double muEta = muLVec_->at(im).Eta(), muPhi = muLVec_->at(im).Phi();
       double mindRmuonCon = 999.;
       for (unsigned int iCon = 0; iCon < numConstituents; ++iCon){
//          const reco::PFCandidatePtr& constituent = constituents[iCon];
          const reco::Candidate * constituent = jet.daughter(iCon);
          const double dRmuonCon = reco::deltaR(constituent->eta(), constituent->phi(), muEta, muPhi);
          if( mindRmuonCon > dRmuonCon ){
             mindRmuonCon = dRmuonCon;
          }
       }
       if( mindRmuonCon < deltaRcon_ ) (*muMatchedJetIdx)[im] = ij;
    }

    for(unsigned int ie=0; ie < eleLVec_->size(); ie++){
       double eleEta = eleLVec_->at(ie).Eta(), elePhi = eleLVec_->at(ie).Phi();
       double mindReleCon = 999.;
       for (unsigned int iCon = 0; iCon < numConstituents; ++iCon){
//          const reco::PFCandidatePtr& constituent = constituents[iCon];
          const reco::Candidate * constituent = jet.daughter(iCon);
          const double dReleCon = reco::deltaR(constituent->eta(), constituent->phi(), eleEta, elePhi);
          if( mindReleCon > dReleCon ){
             mindReleCon = dReleCon;
          }
       }
       if( mindReleCon < deltaRcon_ ) (*eleMatchedJetIdx)[ie] = ij;
    }

    for(unsigned int it=0; it < trksForIsoVetoLVec_->size(); it++){
      double trkEta = trksForIsoVetoLVec_->at(it).Eta(), trkPhi = trksForIsoVetoLVec_->at(it).Phi();
      double mindRtrkCon = 999.;
      for (unsigned int iCon = 0; iCon < numConstituents; ++iCon){
	//          const reco::PFCandidatePtr& constituent = constituents[iCon];
	const reco::Candidate * constituent = jet.daughter(iCon);
	const double dRtrkCon = reco::deltaR(constituent->eta(), constituent->phi(), trkEta, trkPhi);
	if( mindRtrkCon > dRtrkCon ){
	  mindRtrkCon = dRtrkCon;
	}
      }
      if( mindRtrkCon < deltaRcon_ ) (*trksForIsoVetoMatchedJetIdx)[it] = ij;
    }

    for(unsigned int ist=0; ist < looseisoTrksLVec_->size(); ist++){
      double isotrkEta = looseisoTrksLVec_->at(ist).Eta(), isotrkPhi = looseisoTrksLVec_->at(ist).Phi();
      double mindRisotrkCon = 999.;
      for (unsigned int iCon = 0; iCon < numConstituents; ++iCon){
        //          const reco::PFCandidatePtr& constituent = constituents[iCon];
        const reco::Candidate * constituent = jet.daughter(iCon);
        const double dRisotrkCon = reco::deltaR(constituent->eta(), constituent->phi(), isotrkEta, isotrkPhi);
        if( mindRisotrkCon > dRisotrkCon ){
          mindRisotrkCon = dRisotrkCon;
        }
      }
      if( mindRisotrkCon < deltaRcon_ ) (*looseisoTrksMatchedJetIdx)[ist] = ij;
    }

  }

  if( cntJetLowPt ) std::cout<<"WARNING ... NON ZERO ("<<cntJetLowPt<<") number of jets with pt < "<<jetPtCut_miniAOD_<<std::endl;

  std::auto_ptr<int> nJets (new int);

  *nJets = jetsLVec->size();

  // store in the event
  // iEvent.put(prod);
  iEvent.put(jetsLVec, "jetsLVec");
  iEvent.put(recoJetsFlavor, "recoJetsFlavor");
  iEvent.put(recoJetsBtag, "recoJetsBtag");
  iEvent.put(recoJetsJecUnc, "recoJetsJecUnc");
  iEvent.put(recoJetsJecScaleRawToFull, "recoJetsJecScaleRawToFull");
  iEvent.put(nJets, "nJets");

  iEvent.put(recoJetschargedHadronEnergyFraction, "recoJetschargedHadronEnergyFraction");
  iEvent.put(recoJetschargedEmEnergyFraction, "recoJetschargedEmEnergyFraction");
  iEvent.put(recoJetsneutralEmEnergyFraction, "recoJetsneutralEmEnergyFraction");

  iEvent.put(recoJetsmuonEnergyFraction, "recoJetsmuonEnergyFraction");

  iEvent.put(muMatchedJetIdx, "muMatchedJetIdx");
  iEvent.put(eleMatchedJetIdx, "eleMatchedJetIdx");

  iEvent.put(trksForIsoVetoMatchedJetIdx, "trksForIsoVetoMatchedJetIdx");
  iEvent.put(looseisoTrksMatchedJetIdx, "looseisoTrksMatchedJetIdx");
  //PUPPI
  iEvent.put(puppiJetsLVec, "puppiJetsLVec");
  //ak8
  iEvent.put(ak8JetsLVec, "ak8JetsLVec");
  iEvent.put(tau1, "tau1");
  iEvent.put(subjetBdisc, "subjetBdisc");
  iEvent.put(subjetBdiscp, "subjetBdiscp");
  //trying
  iEvent.put(ak8pt, "ak8pt");
  iEvent.put(ak8mass,"ak8mass");
  iEvent.put(ak8rapidity,"ak8rapidity");
  iEvent.put(softDropMass,"softDropMass");
  iEvent.put(prunedMass,"prunedMass");
  iEvent.put(tau2, "tau2");
  iEvent.put(tau3,"tau3");
  //iEvent.put(tau21,"tau21");
  //iEvent.put(tau32,"tau32");
  iEvent.put(puppipt,"puppipt");
  iEvent.put(puppimass,"puppimass");
  iEvent.put(puppieta,"puppieta");
  iEvent.put(puppiphi,"puppiphi");
  iEvent.put(puppitau1,"puppitau1");
  iEvent.put(puppitau2,"puppitau2");
  iEvent.put(puppitau3,"puppitau3");
  //iEvent.put(puppi_tau21,"puppi_tau21");
  //iEvent.put(puppi_tau32,"puppi_tau32"); 
  iEvent.put(deltaRpup,"deltaRpup");  

  return true;
}


#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(prodJets);
