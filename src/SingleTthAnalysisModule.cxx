#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/CommonModules.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/common/include/JetCorrections.h"
#include "UHH2/common/include/EventHists.h"
#include "UHH2/common/include/JetHists.h"
#include "UHH2/common/include/JetIds.h"
#include "UHH2/common/include/TopJetIds.h"
#include "UHH2/common/include/ElectronHists.h"
#include "UHH2/common/include/TopPtReweight.h"
#include "UHH2/common/include/LuminosityHists.h"
#include "UHH2/common/include/ElectronIds.h"
#include "UHH2/common/include/MuonHists.h"
#include "UHH2/common/include/MuonIds.h"
#include "UHH2/common/include/TauHists.h"
#include "UHH2/common/include/NSelections.h"
#include "UHH2/common/include/TriggerSelection.h"
#include "UHH2/SingleTth/include/SingleTthSelections.h"
#include "UHH2/SingleTth/include/SingleTthHists.h"
#include "UHH2/SingleTth/include/SingleTthModules.h"
#include "UHH2/common/include/PrintingModules.h"
#include "UHH2/SingleTth/include/SingleTthReconstructionHypothesisDiscriminators.h"
#include "UHH2/SingleTth/include/SingleTthReconstruction.h"
#include "UHH2/common/include/TTbarGen.h"
#include "UHH2/SingleTth/include/SingleTthGen.h"
#include "UHH2/common/include/MCWeight.h"
#include "UHH2/common/include/AdditionalSelections.h"
#include "TFile.h"
#include "TGraphAsymmErrors.h"
#include "LHAPDF/LHAPDF.h"


using namespace std;
using namespace uhh2;

namespace uhh2examples {

  class SingleTthAnalysisModule: public AnalysisModule {
  public:

    explicit SingleTthAnalysisModule(Context & ctx);
    virtual bool process(Event & event) override;

  private:

    unique_ptr<CommonModules> common;
    unique_ptr<AnalysisModule> SF_muonID, SF_muonIso, SF_eleReco, SF_eleID, SF_btag, scale_variation_module;
    unique_ptr<MCMuonScaleFactor> SF_muonTrigger;
    unique_ptr<ElectronTriggerWeights> SF_eleTrigger;

    // declare the Selections to use.
    unique_ptr<Selection>  btag_loose_sel, btag_2medium_sel, btag_3medium_sel, btag_2tight_sel, btag_3tight_sel, trigger1_much_sel, trigger2_much_sel, trigger1_ech_sel, trigger2_ech_sel, muon_sel_much, ele_sel_much, muon_sel_ech, ele_sel_ech;

    unique_ptr<HighMassSingleTthReconstruction> tprime_reco;
    unique_ptr<SingleTthChi2Discriminator> tprime_chi2;

    // store the Hists collection as member variables.
    unique_ptr<Hists> h_nocuts, h_jets_nocuts, h_ele_nocuts, h_mu_nocuts, h_event_nocuts, h_lumi_nocuts;
    unique_ptr<Hists> h_cleaner, h_jets_cleaner, h_ele_cleaner, h_mu_cleaner, h_event_cleaner, h_lumi_cleaner;
    unique_ptr<Hists> h_trigger, h_jets_trigger, h_ele_trigger, h_mu_trigger, h_event_trigger, h_lumi_trigger, h_btageff_trigger;
    unique_ptr<Hists> h_btag, h_jets_btag, h_ele_btag, h_mu_btag, h_event_btag, h_lumi_btag;
    unique_ptr<Hists> h_btag_2m, h_jets_btag_2m, h_ele_btag_2m, h_mu_btag_2m, h_event_btag_2m, h_lumi_btag_2m;
    unique_ptr<Hists> h_btag_3m, h_jets_btag_3m, h_ele_btag_3m, h_mu_btag_3m, h_event_btag_3m, h_lumi_btag_3m;
    unique_ptr<Hists> h_btag_2t, h_jets_btag_2t, h_ele_btag_2t, h_mu_btag_2t, h_event_btag_2t, h_lumi_btag_2t;
    unique_ptr<Hists> h_btag_3t, h_jets_btag_3t, h_ele_btag_3t, h_mu_btag_3t, h_event_btag_3t, h_lumi_btag_3t;
    unique_ptr<Hists> h_reco_much_sr, h_jets_reco_much_sr, h_ele_reco_much_sr, h_mu_reco_much_sr, h_event_reco_much_sr, h_lumi_reco_much_sr;
    unique_ptr<Hists> h_reco_ech_sr, h_jets_reco_ech_sr, h_ele_reco_ech_sr, h_mu_reco_ech_sr, h_event_reco_ech_sr, h_lumi_reco_ech_sr;
    unique_ptr<Hists> h_reco_much_cr, h_jets_reco_much_cr, h_ele_reco_much_cr, h_mu_reco_much_cr, h_event_reco_much_cr, h_lumi_reco_much_cr;
    unique_ptr<Hists> h_reco_ech_cr, h_jets_reco_ech_cr, h_ele_reco_ech_cr, h_mu_reco_ech_cr, h_event_reco_ech_cr, h_lumi_reco_ech_cr;
    unique_ptr<Hists> h_chi2_10_much_sr, h_jets_chi2_10_much_sr, h_ele_chi2_10_much_sr, h_mu_chi2_10_much_sr, h_event_chi2_10_much_sr, h_lumi_chi2_10_much_sr;
    unique_ptr<Hists> h_chi2_10_ech_sr, h_jets_chi2_10_ech_sr, h_ele_chi2_10_ech_sr, h_mu_chi2_10_ech_sr, h_event_chi2_10_ech_sr, h_lumi_chi2_10_ech_sr;
    unique_ptr<Hists> h_chi2_10_much_cr, h_jets_chi2_10_much_cr, h_ele_chi2_10_much_cr, h_mu_chi2_10_much_cr, h_event_chi2_10_much_cr, h_lumi_chi2_10_much_cr;
    unique_ptr<Hists> h_chi2_10_ech_cr, h_jets_chi2_10_ech_cr, h_ele_chi2_10_ech_cr, h_mu_chi2_10_ech_cr, h_event_chi2_10_ech_cr, h_lumi_chi2_10_ech_cr;
    unique_ptr<Hists> h_chi2h_5_much_sr, h_jets_chi2h_5_much_sr, h_ele_chi2h_5_much_sr, h_mu_chi2h_5_much_sr, h_event_chi2h_5_much_sr, h_lumi_chi2h_5_much_sr;
    unique_ptr<Hists> h_chi2h_5_ech_sr, h_jets_chi2h_5_ech_sr, h_ele_chi2h_5_ech_sr, h_mu_chi2h_5_ech_sr, h_event_chi2h_5_ech_sr, h_lumi_chi2h_5_ech_sr;
    unique_ptr<Hists> h_chi2h_5_much_cr, h_jets_chi2h_5_much_cr, h_ele_chi2h_5_much_cr, h_mu_chi2h_5_much_cr, h_event_chi2h_5_much_cr, h_lumi_chi2h_5_much_cr;
    unique_ptr<Hists> h_chi2h_5_ech_cr, h_jets_chi2h_5_ech_cr, h_ele_chi2h_5_ech_cr, h_mu_chi2h_5_ech_cr, h_event_chi2h_5_ech_cr, h_lumi_chi2h_5_ech_cr;
    unique_ptr<Hists> h_chi2h_2_much_sr, h_jets_chi2h_2_much_sr, h_ele_chi2h_2_much_sr, h_mu_chi2h_2_much_sr, h_event_chi2h_2_much_sr, h_lumi_chi2h_2_much_sr;
    unique_ptr<Hists> h_chi2h_2_ech_sr, h_jets_chi2h_2_ech_sr, h_ele_chi2h_2_ech_sr, h_mu_chi2h_2_ech_sr, h_event_chi2h_2_ech_sr, h_lumi_chi2h_2_ech_sr;
    unique_ptr<Hists> h_chi2h_2_much_cr, h_jets_chi2h_2_much_cr, h_ele_chi2h_2_much_cr, h_mu_chi2h_2_much_cr, h_event_chi2h_2_much_cr, h_lumi_chi2h_2_much_cr;
    unique_ptr<Hists> h_chi2h_2_ech_cr, h_jets_chi2h_2_ech_cr, h_ele_chi2h_2_ech_cr, h_mu_chi2h_2_ech_cr, h_event_chi2h_2_ech_cr, h_lumi_chi2h_2_ech_cr;
    unique_ptr<Hists> h_reco_lowchi2_much_sr, h_jets_reco_lowchi2_much_sr, h_ele_reco_lowchi2_much_sr, h_mu_reco_lowchi2_much_sr, h_event_reco_lowchi2_much_sr, h_lumi_reco_lowchi2_much_sr;
    unique_ptr<Hists> h_reco_lowchi2_ech_sr, h_jets_reco_lowchi2_ech_sr, h_ele_reco_lowchi2_ech_sr, h_mu_reco_lowchi2_ech_sr, h_event_reco_lowchi2_ech_sr, h_lumi_reco_lowchi2_ech_sr;
    unique_ptr<Hists> h_reco_highchi2_much_sr, h_jets_reco_highchi2_much_sr, h_ele_reco_highchi2_much_sr, h_mu_reco_highchi2_much_sr, h_event_reco_highchi2_much_sr, h_lumi_reco_highchi2_much_sr;
    unique_ptr<Hists> h_reco_highchi2_ech_sr, h_jets_reco_highchi2_ech_sr, h_ele_reco_highchi2_ech_sr, h_mu_reco_highchi2_ech_sr, h_event_reco_highchi2_ech_sr, h_lumi_reco_highchi2_ech_sr;
    unique_ptr<Hists> h_dRbb_much_sr, h_jets_dRbb_much_sr, h_ele_dRbb_much_sr, h_mu_dRbb_much_sr, h_event_dRbb_much_sr, h_lumi_dRbb_much_sr;
    unique_ptr<Hists> h_dRbb_ech_sr, h_jets_dRbb_ech_sr, h_ele_dRbb_ech_sr, h_mu_dRbb_ech_sr, h_event_dRbb_ech_sr, h_lumi_dRbb_ech_sr;
    unique_ptr<Hists> h_dRbb_much_cr, h_jets_dRbb_much_cr, h_ele_dRbb_much_cr, h_mu_dRbb_much_cr, h_event_dRbb_much_cr, h_lumi_dRbb_much_cr;
    unique_ptr<Hists> h_dRbb_ech_cr, h_jets_dRbb_ech_cr, h_ele_dRbb_ech_cr, h_mu_dRbb_ech_cr, h_event_dRbb_ech_cr, h_lumi_dRbb_ech_cr;
    unique_ptr<Hists> h_dRbb_11_much_sr, h_jets_dRbb_11_much_sr, h_ele_dRbb_11_much_sr, h_mu_dRbb_11_much_sr, h_event_dRbb_11_much_sr, h_lumi_dRbb_11_much_sr;
    unique_ptr<Hists> h_dRbb_11_ech_sr, h_jets_dRbb_11_ech_sr, h_ele_dRbb_11_ech_sr, h_mu_dRbb_11_ech_sr, h_event_dRbb_11_ech_sr, h_lumi_dRbb_11_ech_sr;
    unique_ptr<Hists> h_dRbb_11_much_cr, h_jets_dRbb_11_much_cr, h_ele_dRbb_11_much_cr, h_mu_dRbb_11_much_cr, h_event_dRbb_11_much_cr, h_lumi_dRbb_11_much_cr;
    unique_ptr<Hists> h_dRbb_11_ech_cr, h_jets_dRbb_11_ech_cr, h_ele_dRbb_11_ech_cr, h_mu_dRbb_11_ech_cr, h_event_dRbb_11_ech_cr, h_lumi_dRbb_11_ech_cr;
    unique_ptr<Hists> h_dRbb_10_much_sr, h_jets_dRbb_10_much_sr, h_ele_dRbb_10_much_sr, h_mu_dRbb_10_much_sr, h_event_dRbb_10_much_sr, h_lumi_dRbb_10_much_sr;
    unique_ptr<Hists> h_dRbb_10_ech_sr, h_jets_dRbb_10_ech_sr, h_ele_dRbb_10_ech_sr, h_mu_dRbb_10_ech_sr, h_event_dRbb_10_ech_sr, h_lumi_dRbb_10_ech_sr;
    unique_ptr<Hists> h_dRbb_10_much_cr, h_jets_dRbb_10_much_cr, h_ele_dRbb_10_much_cr, h_mu_dRbb_10_much_cr, h_event_dRbb_10_much_cr, h_lumi_dRbb_10_much_cr;
    unique_ptr<Hists> h_dRbb_10_ech_cr, h_jets_dRbb_10_ech_cr, h_ele_dRbb_10_ech_cr, h_mu_dRbb_10_ech_cr, h_event_dRbb_10_ech_cr, h_lumi_dRbb_10_ech_cr;
    unique_ptr<Hists> h_dRbw_much_sr, h_jets_dRbw_much_sr, h_ele_dRbw_much_sr, h_mu_dRbw_much_sr, h_event_dRbw_much_sr, h_lumi_dRbw_much_sr;
    unique_ptr<Hists> h_dRbw_ech_sr, h_jets_dRbw_ech_sr, h_ele_dRbw_ech_sr, h_mu_dRbw_ech_sr, h_event_dRbw_ech_sr, h_lumi_dRbw_ech_sr;
    unique_ptr<Hists> h_dRbw_much_cr, h_jets_dRbw_much_cr, h_ele_dRbw_much_cr, h_mu_dRbw_much_cr, h_event_dRbw_much_cr, h_lumi_dRbw_much_cr;
    unique_ptr<Hists> h_dRbw_ech_cr, h_jets_dRbw_ech_cr, h_ele_dRbw_ech_cr, h_mu_dRbw_ech_cr, h_event_dRbw_ech_cr, h_lumi_dRbw_ech_cr;
    unique_ptr<Hists> h_dRbw_15_much_sr, h_jets_dRbw_15_much_sr, h_ele_dRbw_15_much_sr, h_mu_dRbw_15_much_sr, h_event_dRbw_15_much_sr, h_lumi_dRbw_15_much_sr;
    unique_ptr<Hists> h_dRbw_15_ech_sr, h_jets_dRbw_15_ech_sr, h_ele_dRbw_15_ech_sr, h_mu_dRbw_15_ech_sr, h_event_dRbw_15_ech_sr, h_lumi_dRbw_15_ech_sr;
    unique_ptr<Hists> h_dRbw_15_much_cr, h_jets_dRbw_15_much_cr, h_ele_dRbw_15_much_cr, h_mu_dRbw_15_much_cr, h_event_dRbw_15_much_cr, h_lumi_dRbw_15_much_cr;
    unique_ptr<Hists> h_dRbw_15_ech_cr, h_jets_dRbw_15_ech_cr, h_ele_dRbw_15_ech_cr, h_mu_dRbw_15_ech_cr, h_event_dRbw_15_ech_cr, h_lumi_dRbw_15_ech_cr;

    BTag::algo btag_algo;
    BTag::wp wp_loose, wp_medium, wp_tight;

    bool is_mc, do_scale_variation, is_much;
    TString dataset_version, scalevariation_process, region;


    uhh2::Event::Handle<float> h_muon_triggerweight;
    uhh2::Event::Handle<float> h_muon_triggerweight_up;
    uhh2::Event::Handle<float> h_muon_triggerweight_down;

    uhh2::Event::Handle<bool> h_is_tprime_reco;
    uhh2::Event::Handle<std::vector<SingleTthReconstructionHypothesis>> h_hyps;
  };


  SingleTthAnalysisModule::SingleTthAnalysisModule(Context & ctx){

    cout << "Hello World from SingleTthAnalysisModule!" << endl;

    for(auto & kv : ctx.get_all()) cout << " " << kv.first << " = " << kv.second << endl;

    is_mc = ctx.get("dataset_type") == "MC";
    dataset_version = ctx.get("dataset_version");
    // region = ctx.get("Region");
    // if(region != "SR" && region != "HighChi2") throw runtime_error("Invalid value of 'Region' in xml file.");
    scalevariation_process   = ctx.get("ScaleVariationProcess");
    do_scale_variation       = (ctx.get("ScaleVariationMuR") == "up" || ctx.get("ScaleVariationMuR") == "down") || (ctx.get("ScaleVariationMuF") == "up" || ctx.get("ScaleVariationMuF") == "down");
    if(do_scale_variation && (scalevariation_process != "ttbar") && (scalevariation_process != "dy") && (scalevariation_process != "st") && (scalevariation_process != "wj") && (scalevariation_process != "db") && (scalevariation_process != "ttv")) throw runtime_error("In SingleTthAnalysisModule.cxx: Invalid process specified for 'ScaleVariationProcess'.");

    btag_algo = BTag::DEEPJET;
    wp_loose = BTag::WP_LOOSE;
    wp_medium = BTag::WP_MEDIUM;
    wp_tight = BTag::WP_TIGHT;

    JetId DeepjetLoose = BTag(btag_algo, wp_loose);
    JetId DeepjetMedium = BTag(btag_algo, wp_medium);
    JetId DeepjetTight = BTag(btag_algo, wp_tight);

    common.reset(new CommonModules());
    common->disable_lumisel();
    common->disable_jersmear();
    common->disable_jec();
    common->init(ctx);

    SF_muonID.reset(new MCMuonScaleFactor(ctx, "/nfs/dust/cms/user/wilhelml/CMSSW_102/CMSSW_10_2_10/src/UHH2/common/data/2016/MuonID_EfficienciesAndSF_average_RunBtoH.root", "NUM_TightID_DEN_genTracks_eta_pt", 0., "id", false, "nominal"));
    SF_muonTrigger.reset(new MCMuonScaleFactor(ctx, "/nfs/dust/cms/user/wilhelml/CMSSW_102/CMSSW_10_2_10/src/UHH2/common/data/2016/MuonTrigger_EfficienciesAndSF_average_RunBtoH.root", "IsoMu24_OR_IsoTkMu24_PtEtaBins", 2.5, "trigger", false,"nominal" ));
    SF_muonIso.reset(new MCMuonScaleFactor(ctx, "/nfs/dust/cms/user/wilhelml/CMSSW_102/CMSSW_10_2_10/src/UHH2/common/data/2016/MuonIso_EfficienciesAndSF_average_RunBtoH.root", "NUM_TightRelIso_DEN_TightIDandIPCut_eta_pt", 0., "iso", false,"nominal" ));

    SF_eleReco.reset(new MCElecScaleFactor(ctx, "/nfs/dust/cms/user/wilhelml/CMSSW_102/CMSSW_10_2_10/src/UHH2/common/data/2016/egammaEffi.txt_EGM2D_RecEff_Moriond17.root", 1, "reco", "nominal"));
    SF_eleID.reset(new MCElecScaleFactor(ctx, "/nfs/dust/cms/user/wilhelml/CMSSW_102/CMSSW_10_2_10/src/UHH2/common/data/2016/egammaEffi.txt_EGM2D_CutBased_Tight_ID.root", 1, "id", "nominal"));
    SF_eleTrigger.reset(new ElectronTriggerWeights(ctx, "/nfs/dust/cms/user/reimersa/LQToTopMu/Run2_80X_v3/TagProbe/Optimization/35867fb_Iso27_NonIso115/ElectronEfficiencies.root", "nominal"));

    SF_btag.reset(new MCBTagScaleFactor(ctx, btag_algo, wp_tight));

    h_muon_triggerweight = ctx.declare_event_output<float>("weight_sfmu_trigger");
    h_muon_triggerweight_up = ctx.declare_event_output<float>("weight_sfmu_trigger_up");
    h_muon_triggerweight_down = ctx.declare_event_output<float>("weight_sfmu_trigger_down");

    h_is_tprime_reco = ctx.get_handle<bool>("is_tprime_reco");
    h_hyps = ctx.get_handle<vector<SingleTthReconstructionHypothesis>>("TprimeHypotheses");

    // 2. set up selections
    //Selection


    muon_sel_much.reset(new NMuonSelection(1, 1));
    ele_sel_much.reset(new NElectronSelection(0, 0));
    muon_sel_ech.reset(new NMuonSelection(0, 0));
    ele_sel_ech.reset(new NElectronSelection(1, 1));

    btag_loose_sel.reset(new NJetSelection(3, -1, DeepjetLoose));
    btag_2medium_sel.reset(new NJetSelection(2,-1,DeepjetMedium));
    btag_3medium_sel.reset(new NJetSelection(3,-1,DeepjetMedium));
    btag_2tight_sel.reset(new NJetSelection(2,-1,DeepjetTight));
    btag_3tight_sel.reset(new NJetSelection(3,-1,DeepjetTight));
    trigger1_much_sel.reset(new TriggerSelection("HLT_IsoMu24_v*"));
    trigger2_much_sel.reset(new TriggerSelection("HLT_IsoTkMu24_v*"));
    trigger1_ech_sel.reset(new TriggerSelection("HLT_Ele27_WPTight_Gsf_v*"));
    trigger2_ech_sel.reset(new TriggerSelection("HLT_Ele105_CaloIdVT_GsfTrkIdT_v*"));

    tprime_reco.reset(new HighMassSingleTthReconstruction(ctx, SingleTthNeutrinoReconstruction, DeepjetTight));
    tprime_chi2.reset(new SingleTthChi2Discriminator(ctx));

    scale_variation_module.reset(new MCScaleVariation(ctx));

    // 3. Set up Hists classes:
    h_nocuts.reset(new SingleTthHists(ctx, "nocuts"));
    h_jets_nocuts.reset(new JetHists(ctx, "Jets_nocuts"));
    h_ele_nocuts.reset(new ElectronHists(ctx, "Ele_nocuts"));
    h_mu_nocuts.reset(new MuonHists(ctx, "Mu_nocuts"));
    h_event_nocuts.reset(new EventHists(ctx, "Event_nocuts"));
    h_lumi_nocuts.reset(new LuminosityHists(ctx, "Lumi_nocuts"));

    h_cleaner.reset(new SingleTthHists(ctx, "cleaner"));
    h_jets_cleaner.reset(new JetHists(ctx, "Jets_cleaner"));
    h_ele_cleaner.reset(new ElectronHists(ctx, "Ele_cleaner"));
    h_mu_cleaner.reset(new MuonHists(ctx, "Mu_cleaner"));
    h_event_cleaner.reset(new EventHists(ctx, "Event_cleaner"));
    h_lumi_cleaner.reset(new LuminosityHists(ctx, "Lumi_cleaner"));

    h_trigger.reset(new SingleTthHists(ctx, "trigger"));
    h_jets_trigger.reset(new JetHists(ctx, "Jets_trigger"));
    h_ele_trigger.reset(new ElectronHists(ctx, "Ele_trigger"));
    h_mu_trigger.reset(new MuonHists(ctx, "Mu_trigger"));
    h_event_trigger.reset(new EventHists(ctx, "Event_trigger"));
    h_lumi_trigger.reset(new LuminosityHists(ctx, "Lumi_trigger"));

    h_btageff_trigger.reset(new BTagMCEfficiencyHists(ctx, "btageff_trigger", DeepjetTight));

    h_btag.reset(new SingleTthHists(ctx, "btag"));
    h_jets_btag.reset(new JetHists(ctx, "Jets_btag"));
    h_ele_btag.reset(new ElectronHists(ctx, "Ele_btag"));
    h_mu_btag.reset(new MuonHists(ctx, "Mu_btag"));
    h_event_btag.reset(new EventHists(ctx, "Event_btag"));
    h_lumi_btag.reset(new LuminosityHists(ctx, "Lumi_btag"));

    h_btag_2m.reset(new SingleTthHists(ctx, "btag_2m"));
    h_jets_btag_2m.reset(new JetHists(ctx, "Jets_btag_2m"));
    h_ele_btag_2m.reset(new ElectronHists(ctx, "Ele_btag_2m"));
    h_mu_btag_2m.reset(new MuonHists(ctx, "Mu_btag_2m"));
    h_event_btag_2m.reset(new EventHists(ctx, "Event_btag_2m"));
    h_lumi_btag_2m.reset(new LuminosityHists(ctx, "Lumi_btag_2m"));

    h_btag_3m.reset(new SingleTthHists(ctx, "btag_3m"));
    h_jets_btag_3m.reset(new JetHists(ctx, "Jets_btag_3m"));
    h_ele_btag_3m.reset(new ElectronHists(ctx, "Ele_btag_3m"));
    h_mu_btag_3m.reset(new MuonHists(ctx, "Mu_btag_3m"));
    h_event_btag_3m.reset(new EventHists(ctx, "Event_btag_3m"));
    h_lumi_btag_3m.reset(new LuminosityHists(ctx, "Lumi_btag_3m"));

    h_btag_2t.reset(new SingleTthHists(ctx, "btag_2t"));
    h_jets_btag_2t.reset(new JetHists(ctx, "Jets_btag_2t"));
    h_ele_btag_2t.reset(new ElectronHists(ctx, "Ele_btag_2t"));
    h_mu_btag_2t.reset(new MuonHists(ctx, "Mu_btag_2t"));
    h_event_btag_2t.reset(new EventHists(ctx, "Event_btag_2t"));
    h_lumi_btag_2t.reset(new LuminosityHists(ctx, "Lumi_btag_2t"));

    h_btag_3t.reset(new SingleTthHists(ctx, "btag_3t"));
    h_jets_btag_3t.reset(new JetHists(ctx, "Jets_btag_3t"));
    h_ele_btag_3t.reset(new ElectronHists(ctx, "Ele_btag_3t"));
    h_mu_btag_3t.reset(new MuonHists(ctx, "Mu_btag_3t"));
    h_event_btag_3t.reset(new EventHists(ctx, "Event_btag_3t"));
    h_lumi_btag_3t.reset(new LuminosityHists(ctx, "Lumi_btag_3t"));

    h_reco_much_sr.reset(new SingleTthHists(ctx, "reco_much_sr"));
    h_jets_reco_much_sr.reset(new JetHists(ctx, "Jets_reco_much_sr"));
    h_ele_reco_much_sr.reset(new ElectronHists(ctx, "Ele_reco_much_sr"));
    h_mu_reco_much_sr.reset(new MuonHists(ctx, "Mu_reco_much_sr"));
    h_event_reco_much_sr.reset(new EventHists(ctx, "Event_reco_much_sr"));
    h_lumi_reco_much_sr.reset(new LuminosityHists(ctx, "Lumi_reco_much_sr"));

    h_reco_ech_sr.reset(new SingleTthHists(ctx, "reco_ech_sr"));
    h_jets_reco_ech_sr.reset(new JetHists(ctx, "Jets_reco_ech_sr"));
    h_ele_reco_ech_sr.reset(new ElectronHists(ctx, "Ele_reco_ech_sr"));
    h_mu_reco_ech_sr.reset(new MuonHists(ctx, "Mu_reco_ech_sr"));
    h_event_reco_ech_sr.reset(new EventHists(ctx, "Event_reco_ech_sr"));
    h_lumi_reco_ech_sr.reset(new LuminosityHists(ctx, "Lumi_reco_ech_sr"));

    h_reco_much_cr.reset(new SingleTthHists(ctx, "reco_much_cr"));
    h_jets_reco_much_cr.reset(new JetHists(ctx, "Jets_reco_much_cr"));
    h_ele_reco_much_cr.reset(new ElectronHists(ctx, "Ele_reco_much_cr"));
    h_mu_reco_much_cr.reset(new MuonHists(ctx, "Mu_reco_much_cr"));
    h_event_reco_much_cr.reset(new EventHists(ctx, "Event_reco_much_cr"));
    h_lumi_reco_much_cr.reset(new LuminosityHists(ctx, "Lumi_reco_much_cr"));

    h_reco_ech_cr.reset(new SingleTthHists(ctx, "reco_ech_cr"));
    h_jets_reco_ech_cr.reset(new JetHists(ctx, "Jets_reco_ech_cr"));
    h_ele_reco_ech_cr.reset(new ElectronHists(ctx, "Ele_reco_ech_cr"));
    h_mu_reco_ech_cr.reset(new MuonHists(ctx, "Mu_reco_ech_cr"));
    h_event_reco_ech_cr.reset(new EventHists(ctx, "Event_reco_ech_cr"));
    h_lumi_reco_ech_cr.reset(new LuminosityHists(ctx, "Lumi_reco_ech_cr"));



    h_reco_lowchi2_much_sr.reset(new SingleTthHists(ctx, "reco_lowchi2_much_sr"));
    h_jets_reco_lowchi2_much_sr.reset(new JetHists(ctx, "Jets_reco_lowchi2_much_sr"));
    h_ele_reco_lowchi2_much_sr.reset(new ElectronHists(ctx, "Ele_reco_lowchi2_much_sr"));
    h_mu_reco_lowchi2_much_sr.reset(new MuonHists(ctx, "Mu_reco_lowchi2_much_sr"));
    h_event_reco_lowchi2_much_sr.reset(new EventHists(ctx, "Event_reco_lowchi2_much_sr"));
    h_lumi_reco_lowchi2_much_sr.reset(new LuminosityHists(ctx, "Lumi_reco_lowchi2_much_sr"));

    h_reco_lowchi2_ech_sr.reset(new SingleTthHists(ctx, "reco_lowchi2_ech_sr"));
    h_jets_reco_lowchi2_ech_sr.reset(new JetHists(ctx, "Jets_reco_lowchi2_ech_sr"));
    h_ele_reco_lowchi2_ech_sr.reset(new ElectronHists(ctx, "Ele_reco_lowchi2_ech_sr"));
    h_mu_reco_lowchi2_ech_sr.reset(new MuonHists(ctx, "Mu_reco_lowchi2_ech_sr"));
    h_event_reco_lowchi2_ech_sr.reset(new EventHists(ctx, "Event_reco_lowchi2_ech_sr"));
    h_lumi_reco_lowchi2_ech_sr.reset(new LuminosityHists(ctx, "Lumi_reco_lowchi2_ech_sr"));

    h_reco_highchi2_much_sr.reset(new SingleTthHists(ctx, "reco_highchi2_much_sr"));
    h_jets_reco_highchi2_much_sr.reset(new JetHists(ctx, "Jets_reco_highchi2_much_sr"));
    h_ele_reco_highchi2_much_sr.reset(new ElectronHists(ctx, "Ele_reco_highchi2_much_sr"));
    h_mu_reco_highchi2_much_sr.reset(new MuonHists(ctx, "Mu_reco_highchi2_much_sr"));
    h_event_reco_highchi2_much_sr.reset(new EventHists(ctx, "Event_reco_highchi2_much_sr"));
    h_lumi_reco_highchi2_much_sr.reset(new LuminosityHists(ctx, "Lumi_reco_highchi2_much_sr"));

    h_reco_highchi2_ech_sr.reset(new SingleTthHists(ctx, "reco_highchi2_ech_sr"));
    h_jets_reco_highchi2_ech_sr.reset(new JetHists(ctx, "Jets_reco_highchi2_ech_sr"));
    h_ele_reco_highchi2_ech_sr.reset(new ElectronHists(ctx, "Ele_reco_highchi2_ech_sr"));
    h_mu_reco_highchi2_ech_sr.reset(new MuonHists(ctx, "Mu_reco_highchi2_ech_sr"));
    h_event_reco_highchi2_ech_sr.reset(new EventHists(ctx, "Event_reco_highchi2_ech_sr"));
    h_lumi_reco_highchi2_ech_sr.reset(new LuminosityHists(ctx, "Lumi_reco_highchi2_ech_sr"));



    h_chi2_10_much_sr.reset(new SingleTthHists(ctx, "chi2_10_much_sr"));
    h_jets_chi2_10_much_sr.reset(new JetHists(ctx, "Jets_chi2_10_much_sr"));
    h_ele_chi2_10_much_sr.reset(new ElectronHists(ctx, "Ele_chi2_10_much_sr"));
    h_mu_chi2_10_much_sr.reset(new MuonHists(ctx, "Mu_chi2_10_much_sr"));
    h_event_chi2_10_much_sr.reset(new EventHists(ctx, "Event_chi2_10_much_sr"));
    h_lumi_chi2_10_much_sr.reset(new LuminosityHists(ctx, "Lumi_chi2_10_much_sr"));

    h_chi2_10_ech_sr.reset(new SingleTthHists(ctx, "chi2_10_ech_sr"));
    h_jets_chi2_10_ech_sr.reset(new JetHists(ctx, "Jets_chi2_10_ech_sr"));
    h_ele_chi2_10_ech_sr.reset(new ElectronHists(ctx, "Ele_chi2_10_ech_sr"));
    h_mu_chi2_10_ech_sr.reset(new MuonHists(ctx, "Mu_chi2_10_ech_sr"));
    h_event_chi2_10_ech_sr.reset(new EventHists(ctx, "Event_chi2_10_ech_sr"));
    h_lumi_chi2_10_ech_sr.reset(new LuminosityHists(ctx, "Lumi_chi2_10_ech_sr"));

    h_chi2_10_much_cr.reset(new SingleTthHists(ctx, "chi2_10_much_cr"));
    h_jets_chi2_10_much_cr.reset(new JetHists(ctx, "Jets_chi2_10_much_cr"));
    h_ele_chi2_10_much_cr.reset(new ElectronHists(ctx, "Ele_chi2_10_much_cr"));
    h_mu_chi2_10_much_cr.reset(new MuonHists(ctx, "Mu_chi2_10_much_cr"));
    h_event_chi2_10_much_cr.reset(new EventHists(ctx, "Event_chi2_10_much_cr"));
    h_lumi_chi2_10_much_cr.reset(new LuminosityHists(ctx, "Lumi_chi2_10_much_cr"));

    h_chi2_10_ech_cr.reset(new SingleTthHists(ctx, "chi2_10_ech_cr"));
    h_jets_chi2_10_ech_cr.reset(new JetHists(ctx, "Jets_chi2_10_ech_cr"));
    h_ele_chi2_10_ech_cr.reset(new ElectronHists(ctx, "Ele_chi2_10_ech_cr"));
    h_mu_chi2_10_ech_cr.reset(new MuonHists(ctx, "Mu_chi2_10_ech_cr"));
    h_event_chi2_10_ech_cr.reset(new EventHists(ctx, "Event_chi2_10_ech_cr"));
    h_lumi_chi2_10_ech_cr.reset(new LuminosityHists(ctx, "Lumi_chi2_10_ech_cr"));

    h_chi2h_5_much_sr.reset(new SingleTthHists(ctx, "chi2h_5_much_sr"));
    h_jets_chi2h_5_much_sr.reset(new JetHists(ctx, "Jets_chi2h_5_much_sr"));
    h_ele_chi2h_5_much_sr.reset(new ElectronHists(ctx, "Ele_chi2h_5_much_sr"));
    h_mu_chi2h_5_much_sr.reset(new MuonHists(ctx, "Mu_chi2h_5_much_sr"));
    h_event_chi2h_5_much_sr.reset(new EventHists(ctx, "Event_chi2h_5_much_sr"));
    h_lumi_chi2h_5_much_sr.reset(new LuminosityHists(ctx, "Lumi_chi2h_5_much_sr"));

    h_chi2h_5_ech_sr.reset(new SingleTthHists(ctx, "chi2h_5_ech_sr"));
    h_jets_chi2h_5_ech_sr.reset(new JetHists(ctx, "Jets_chi2h_5_ech_sr"));
    h_ele_chi2h_5_ech_sr.reset(new ElectronHists(ctx, "Ele_chi2h_5_ech_sr"));
    h_mu_chi2h_5_ech_sr.reset(new MuonHists(ctx, "Mu_chi2h_5_ech_sr"));
    h_event_chi2h_5_ech_sr.reset(new EventHists(ctx, "Event_chi2h_5_ech_sr"));
    h_lumi_chi2h_5_ech_sr.reset(new LuminosityHists(ctx, "Lumi_chi2h_5_ech_sr"));

    h_chi2h_5_much_cr.reset(new SingleTthHists(ctx, "chi2h_5_much_cr"));
    h_jets_chi2h_5_much_cr.reset(new JetHists(ctx, "Jets_chi2h_5_much_cr"));
    h_ele_chi2h_5_much_cr.reset(new ElectronHists(ctx, "Ele_chi2h_5_much_cr"));
    h_mu_chi2h_5_much_cr.reset(new MuonHists(ctx, "Mu_chi2h_5_much_cr"));
    h_event_chi2h_5_much_cr.reset(new EventHists(ctx, "Event_chi2h_5_much_cr"));
    h_lumi_chi2h_5_much_cr.reset(new LuminosityHists(ctx, "Lumi_chi2h_5_much_cr"));

    h_chi2h_5_ech_cr.reset(new SingleTthHists(ctx, "chi2h_5_ech_cr"));
    h_jets_chi2h_5_ech_cr.reset(new JetHists(ctx, "Jets_chi2h_5_ech_cr"));
    h_ele_chi2h_5_ech_cr.reset(new ElectronHists(ctx, "Ele_chi2h_5_ech_cr"));
    h_mu_chi2h_5_ech_cr.reset(new MuonHists(ctx, "Mu_chi2h_5_ech_cr"));
    h_event_chi2h_5_ech_cr.reset(new EventHists(ctx, "Event_chi2h_5_ech_cr"));
    h_lumi_chi2h_5_ech_cr.reset(new LuminosityHists(ctx, "Lumi_chi2h_5_ech_cr"));

    h_chi2h_2_much_sr.reset(new SingleTthHists(ctx, "chi2h_2_much_sr"));
    h_jets_chi2h_2_much_sr.reset(new JetHists(ctx, "Jets_chi2h_2_much_sr"));
    h_ele_chi2h_2_much_sr.reset(new ElectronHists(ctx, "Ele_chi2h_2_much_sr"));
    h_mu_chi2h_2_much_sr.reset(new MuonHists(ctx, "Mu_chi2h_2_much_sr"));
    h_event_chi2h_2_much_sr.reset(new EventHists(ctx, "Event_chi2h_2_much_sr"));
    h_lumi_chi2h_2_much_sr.reset(new LuminosityHists(ctx, "Lumi_chi2h_2_much_sr"));

    h_chi2h_2_ech_sr.reset(new SingleTthHists(ctx, "chi2h_2_ech_sr"));
    h_jets_chi2h_2_ech_sr.reset(new JetHists(ctx, "Jets_chi2h_2_ech_sr"));
    h_ele_chi2h_2_ech_sr.reset(new ElectronHists(ctx, "Ele_chi2h_2_ech_sr"));
    h_mu_chi2h_2_ech_sr.reset(new MuonHists(ctx, "Mu_chi2h_2_ech_sr"));
    h_event_chi2h_2_ech_sr.reset(new EventHists(ctx, "Event_chi2h_2_ech_sr"));
    h_lumi_chi2h_2_ech_sr.reset(new LuminosityHists(ctx, "Lumi_chi2h_2_ech_sr"));

    h_chi2h_2_much_cr.reset(new SingleTthHists(ctx, "chi2h_2_much_cr"));
    h_jets_chi2h_2_much_cr.reset(new JetHists(ctx, "Jets_chi2h_2_much_cr"));
    h_ele_chi2h_2_much_cr.reset(new ElectronHists(ctx, "Ele_chi2h_2_much_cr"));
    h_mu_chi2h_2_much_cr.reset(new MuonHists(ctx, "Mu_chi2h_2_much_cr"));
    h_event_chi2h_2_much_cr.reset(new EventHists(ctx, "Event_chi2h_2_much_cr"));
    h_lumi_chi2h_2_much_cr.reset(new LuminosityHists(ctx, "Lumi_chi2h_2_much_cr"));

    h_chi2h_2_ech_cr.reset(new SingleTthHists(ctx, "chi2h_2_ech_cr"));
    h_jets_chi2h_2_ech_cr.reset(new JetHists(ctx, "Jets_chi2h_2_ech_cr"));
    h_ele_chi2h_2_ech_cr.reset(new ElectronHists(ctx, "Ele_chi2h_2_ech_cr"));
    h_mu_chi2h_2_ech_cr.reset(new MuonHists(ctx, "Mu_chi2h_2_ech_cr"));
    h_event_chi2h_2_ech_cr.reset(new EventHists(ctx, "Event_chi2h_2_ech_cr"));
    h_lumi_chi2h_2_ech_cr.reset(new LuminosityHists(ctx, "Lumi_chi2h_2_ech_cr"));

    h_dRbb_much_sr.reset(new SingleTthHists(ctx, "dRbb_much_sr"));
    h_jets_dRbb_much_sr.reset(new JetHists(ctx, "Jets_dRbb_much_sr"));
    h_ele_dRbb_much_sr.reset(new ElectronHists(ctx, "Ele_dRbb_much_sr"));
    h_mu_dRbb_much_sr.reset(new MuonHists(ctx, "Mu_dRbb_much_sr"));
    h_event_dRbb_much_sr.reset(new EventHists(ctx, "Event_dRbb_much_sr"));
    h_lumi_dRbb_much_sr.reset(new LuminosityHists(ctx, "Lumi_dRbb_much_sr"));

    h_dRbb_ech_sr.reset(new SingleTthHists(ctx, "dRbb_ech_sr"));
    h_jets_dRbb_ech_sr.reset(new JetHists(ctx, "Jets_dRbb_ech_sr"));
    h_ele_dRbb_ech_sr.reset(new ElectronHists(ctx, "Ele_dRbb_ech_sr"));
    h_mu_dRbb_ech_sr.reset(new MuonHists(ctx, "Mu_dRbb_ech_sr"));
    h_event_dRbb_ech_sr.reset(new EventHists(ctx, "Event_dRbb_ech_sr"));
    h_lumi_dRbb_ech_sr.reset(new LuminosityHists(ctx, "Lumi_dRbb_ech_sr"));

    h_dRbb_much_cr.reset(new SingleTthHists(ctx, "dRbb_much_cr"));
    h_jets_dRbb_much_cr.reset(new JetHists(ctx, "Jets_dRbb_much_cr"));
    h_ele_dRbb_much_cr.reset(new ElectronHists(ctx, "Ele_dRbb_much_cr"));
    h_mu_dRbb_much_cr.reset(new MuonHists(ctx, "Mu_dRbb_much_cr"));
    h_event_dRbb_much_cr.reset(new EventHists(ctx, "Event_dRbb_much_cr"));
    h_lumi_dRbb_much_cr.reset(new LuminosityHists(ctx, "Lumi_dRbb_much_cr"));

    h_dRbb_ech_cr.reset(new SingleTthHists(ctx, "dRbb_ech_cr"));
    h_jets_dRbb_ech_cr.reset(new JetHists(ctx, "Jets_dRbb_ech_cr"));
    h_ele_dRbb_ech_cr.reset(new ElectronHists(ctx, "Ele_dRbb_ech_cr"));
    h_mu_dRbb_ech_cr.reset(new MuonHists(ctx, "Mu_dRbb_ech_cr"));
    h_event_dRbb_ech_cr.reset(new EventHists(ctx, "Event_dRbb_ech_cr"));
    h_lumi_dRbb_ech_cr.reset(new LuminosityHists(ctx, "Lumi_dRbb_ech_cr"));

    h_dRbb_11_much_sr.reset(new SingleTthHists(ctx, "dRbb_11_much_sr"));
    h_jets_dRbb_11_much_sr.reset(new JetHists(ctx, "Jets_dRbb_11_much_sr"));
    h_ele_dRbb_11_much_sr.reset(new ElectronHists(ctx, "Ele_dRbb_11_much_sr"));
    h_mu_dRbb_11_much_sr.reset(new MuonHists(ctx, "Mu_dRbb_11_much_sr"));
    h_event_dRbb_11_much_sr.reset(new EventHists(ctx, "Event_dRbb_11_much_sr"));
    h_lumi_dRbb_11_much_sr.reset(new LuminosityHists(ctx, "Lumi_dRbb_11_much_sr"));

    h_dRbb_11_ech_sr.reset(new SingleTthHists(ctx, "dRbb_11_ech_sr"));
    h_jets_dRbb_11_ech_sr.reset(new JetHists(ctx, "Jets_dRbb_11_ech_sr"));
    h_ele_dRbb_11_ech_sr.reset(new ElectronHists(ctx, "Ele_dRbb_11_ech_sr"));
    h_mu_dRbb_11_ech_sr.reset(new MuonHists(ctx, "Mu_dRbb_11_ech_sr"));
    h_event_dRbb_11_ech_sr.reset(new EventHists(ctx, "Event_dRbb_11_ech_sr"));
    h_lumi_dRbb_11_ech_sr.reset(new LuminosityHists(ctx, "Lumi_dRbb_11_ech_sr"));

    h_dRbb_11_much_cr.reset(new SingleTthHists(ctx, "dRbb_11_much_cr"));
    h_jets_dRbb_11_much_cr.reset(new JetHists(ctx, "Jets_dRbb_11_much_cr"));
    h_ele_dRbb_11_much_cr.reset(new ElectronHists(ctx, "Ele_dRbb_11_much_cr"));
    h_mu_dRbb_11_much_cr.reset(new MuonHists(ctx, "Mu_dRbb_11_much_cr"));
    h_event_dRbb_11_much_cr.reset(new EventHists(ctx, "Event_dRbb_11_much_cr"));
    h_lumi_dRbb_11_much_cr.reset(new LuminosityHists(ctx, "Lumi_dRbb_11_much_cr"));

    h_dRbb_11_ech_cr.reset(new SingleTthHists(ctx, "dRbb_11_ech_cr"));
    h_jets_dRbb_11_ech_cr.reset(new JetHists(ctx, "Jets_dRbb_11_ech_cr"));
    h_ele_dRbb_11_ech_cr.reset(new ElectronHists(ctx, "Ele_dRbb_11_ech_cr"));
    h_mu_dRbb_11_ech_cr.reset(new MuonHists(ctx, "Mu_dRbb_11_ech_cr"));
    h_event_dRbb_11_ech_cr.reset(new EventHists(ctx, "Event_dRbb_11_ech_cr"));
    h_lumi_dRbb_11_ech_cr.reset(new LuminosityHists(ctx, "Lumi_dRbb_11_ech_cr"));

    h_dRbb_10_much_sr.reset(new SingleTthHists(ctx, "dRbb_10_much_sr"));
    h_jets_dRbb_10_much_sr.reset(new JetHists(ctx, "Jets_dRbb_10_much_sr"));
    h_ele_dRbb_10_much_sr.reset(new ElectronHists(ctx, "Ele_dRbb_10_much_sr"));
    h_mu_dRbb_10_much_sr.reset(new MuonHists(ctx, "Mu_dRbb_10_much_sr"));
    h_event_dRbb_10_much_sr.reset(new EventHists(ctx, "Event_dRbb_10_much_sr"));
    h_lumi_dRbb_10_much_sr.reset(new LuminosityHists(ctx, "Lumi_dRbb_10_much_sr"));

    h_dRbb_10_ech_sr.reset(new SingleTthHists(ctx, "dRbb_10_ech_sr"));
    h_jets_dRbb_10_ech_sr.reset(new JetHists(ctx, "Jets_dRbb_10_ech_sr"));
    h_ele_dRbb_10_ech_sr.reset(new ElectronHists(ctx, "Ele_dRbb_10_ech_sr"));
    h_mu_dRbb_10_ech_sr.reset(new MuonHists(ctx, "Mu_dRbb_10_ech_sr"));
    h_event_dRbb_10_ech_sr.reset(new EventHists(ctx, "Event_dRbb_10_ech_sr"));
    h_lumi_dRbb_10_ech_sr.reset(new LuminosityHists(ctx, "Lumi_dRbb_10_ech_sr"));

    h_dRbb_10_much_cr.reset(new SingleTthHists(ctx, "dRbb_10_much_cr"));
    h_jets_dRbb_10_much_cr.reset(new JetHists(ctx, "Jets_dRbb_10_much_cr"));
    h_ele_dRbb_10_much_cr.reset(new ElectronHists(ctx, "Ele_dRbb_10_much_cr"));
    h_mu_dRbb_10_much_cr.reset(new MuonHists(ctx, "Mu_dRbb_10_much_cr"));
    h_event_dRbb_10_much_cr.reset(new EventHists(ctx, "Event_dRbb_10_much_cr"));
    h_lumi_dRbb_10_much_cr.reset(new LuminosityHists(ctx, "Lumi_dRbb_10_much_cr"));

    h_dRbb_10_ech_cr.reset(new SingleTthHists(ctx, "dRbb_10_ech_cr"));
    h_jets_dRbb_10_ech_cr.reset(new JetHists(ctx, "Jets_dRbb_10_ech_cr"));
    h_ele_dRbb_10_ech_cr.reset(new ElectronHists(ctx, "Ele_dRbb_10_ech_cr"));
    h_mu_dRbb_10_ech_cr.reset(new MuonHists(ctx, "Mu_dRbb_10_ech_cr"));
    h_event_dRbb_10_ech_cr.reset(new EventHists(ctx, "Event_dRbb_10_ech_cr"));
    h_lumi_dRbb_10_ech_cr.reset(new LuminosityHists(ctx, "Lumi_dRbb_10_ech_cr"));

    h_dRbw_much_sr.reset(new SingleTthHists(ctx, "dRbw_much_sr"));
    h_jets_dRbw_much_sr.reset(new JetHists(ctx, "Jets_dRbw_much_sr"));
    h_ele_dRbw_much_sr.reset(new ElectronHists(ctx, "Ele_dRbw_much_sr"));
    h_mu_dRbw_much_sr.reset(new MuonHists(ctx, "Mu_dRbw_much_sr"));
    h_event_dRbw_much_sr.reset(new EventHists(ctx, "Event_dRbw_much_sr"));
    h_lumi_dRbw_much_sr.reset(new LuminosityHists(ctx, "Lumi_dRbw_much_sr"));

    h_dRbw_ech_sr.reset(new SingleTthHists(ctx, "dRbw_ech_sr"));
    h_jets_dRbw_ech_sr.reset(new JetHists(ctx, "Jets_dRbw_ech_sr"));
    h_ele_dRbw_ech_sr.reset(new ElectronHists(ctx, "Ele_dRbw_ech_sr"));
    h_mu_dRbw_ech_sr.reset(new MuonHists(ctx, "Mu_dRbw_ech_sr"));
    h_event_dRbw_ech_sr.reset(new EventHists(ctx, "Event_dRbw_ech_sr"));
    h_lumi_dRbw_ech_sr.reset(new LuminosityHists(ctx, "Lumi_dRbw_ech_sr"));

    h_dRbw_much_cr.reset(new SingleTthHists(ctx, "dRbw_much_cr"));
    h_jets_dRbw_much_cr.reset(new JetHists(ctx, "Jets_dRbw_much_cr"));
    h_ele_dRbw_much_cr.reset(new ElectronHists(ctx, "Ele_dRbw_much_cr"));
    h_mu_dRbw_much_cr.reset(new MuonHists(ctx, "Mu_dRbw_much_cr"));
    h_event_dRbw_much_cr.reset(new EventHists(ctx, "Event_dRbw_much_cr"));
    h_lumi_dRbw_much_cr.reset(new LuminosityHists(ctx, "Lumi_dRbw_much_cr"));

    h_dRbw_ech_cr.reset(new SingleTthHists(ctx, "dRbw_ech_cr"));
    h_jets_dRbw_ech_cr.reset(new JetHists(ctx, "Jets_dRbw_ech_cr"));
    h_ele_dRbw_ech_cr.reset(new ElectronHists(ctx, "Ele_dRbw_ech_cr"));
    h_mu_dRbw_ech_cr.reset(new MuonHists(ctx, "Mu_dRbw_ech_cr"));
    h_event_dRbw_ech_cr.reset(new EventHists(ctx, "Event_dRbw_ech_cr"));
    h_lumi_dRbw_ech_cr.reset(new LuminosityHists(ctx, "Lumi_dRbw_ech_cr"));

    h_dRbw_15_much_sr.reset(new SingleTthHists(ctx, "dRbw_15_much_sr"));
    h_jets_dRbw_15_much_sr.reset(new JetHists(ctx, "Jets_dRbw_15_much_sr"));
    h_ele_dRbw_15_much_sr.reset(new ElectronHists(ctx, "Ele_dRbw_15_much_sr"));
    h_mu_dRbw_15_much_sr.reset(new MuonHists(ctx, "Mu_dRbw_15_much_sr"));
    h_event_dRbw_15_much_sr.reset(new EventHists(ctx, "Event_dRbw_15_much_sr"));
    h_lumi_dRbw_15_much_sr.reset(new LuminosityHists(ctx, "Lumi_dRbw_15_much_sr"));

    h_dRbw_15_ech_sr.reset(new SingleTthHists(ctx, "dRbw_15_ech_sr"));
    h_jets_dRbw_15_ech_sr.reset(new JetHists(ctx, "Jets_dRbw_15_ech_sr"));
    h_ele_dRbw_15_ech_sr.reset(new ElectronHists(ctx, "Ele_dRbw_15_ech_sr"));
    h_mu_dRbw_15_ech_sr.reset(new MuonHists(ctx, "Mu_dRbw_15_ech_sr"));
    h_event_dRbw_15_ech_sr.reset(new EventHists(ctx, "Event_dRbw_15_ech_sr"));
    h_lumi_dRbw_15_ech_sr.reset(new LuminosityHists(ctx, "Lumi_dRbw_15_ech_sr"));

    h_dRbw_15_much_cr.reset(new SingleTthHists(ctx, "dRbw_15_much_cr"));
    h_jets_dRbw_15_much_cr.reset(new JetHists(ctx, "Jets_dRbw_15_much_cr"));
    h_ele_dRbw_15_much_cr.reset(new ElectronHists(ctx, "Ele_dRbw_15_much_cr"));
    h_mu_dRbw_15_much_cr.reset(new MuonHists(ctx, "Mu_dRbw_15_much_cr"));
    h_event_dRbw_15_much_cr.reset(new EventHists(ctx, "Event_dRbw_15_much_cr"));
    h_lumi_dRbw_15_much_cr.reset(new LuminosityHists(ctx, "Lumi_dRbw_15_much_cr"));

    h_dRbw_15_ech_cr.reset(new SingleTthHists(ctx, "dRbw_15_ech_cr"));
    h_jets_dRbw_15_ech_cr.reset(new JetHists(ctx, "Jets_dRbw_15_ech_cr"));
    h_ele_dRbw_15_ech_cr.reset(new ElectronHists(ctx, "Ele_dRbw_15_ech_cr"));
    h_mu_dRbw_15_ech_cr.reset(new MuonHists(ctx, "Mu_dRbw_15_ech_cr"));
    h_event_dRbw_15_ech_cr.reset(new EventHists(ctx, "Event_dRbw_15_ech_cr"));
    h_lumi_dRbw_15_ech_cr.reset(new LuminosityHists(ctx, "Lumi_dRbw_15_ech_cr"));

  }


  bool SingleTthAnalysisModule::process(Event & event) {

    event.set(h_is_tprime_reco, false);

    is_much = (muon_sel_much->passes(event) && ele_sel_much->passes(event));
    if(!is_much){
      // if this is not the muon channel, it should be the electron channel satisfying the condition below. If not, abort!
      if(!(muon_sel_ech->passes(event) && ele_sel_ech->passes(event))) throw runtime_error("This appears to be neither the muon nor the electron channel. Abort.");
    }

    //now also check if it's not both, electron AND muon channel at the same time
    if(is_much && (muon_sel_ech->passes(event) && ele_sel_ech->passes(event))) throw runtime_error("This appears to be noth the muon AND the electron channel. Abort.");

    if(do_scale_variation){
      if((dataset_version.Contains("TTbar") && scalevariation_process == "ttbar") || (dataset_version.Contains("DYJets") && scalevariation_process == "dy") || (dataset_version.Contains("SingleTop") && scalevariation_process == "st") || (dataset_version.Contains("WJets") && scalevariation_process == "wj") || (dataset_version.Contains("Diboson") && scalevariation_process == "db") || (dataset_version.Contains("TTV") && scalevariation_process == "ttv")){
        if(event.genInfo->systweights().size() < 10 && dataset_version.Contains("Diboson")) cout << "SystWeight size: " << event.genInfo->systweights().size() << endl;
        else{
          scale_variation_module->process(event);
        }
      }
    }

    if(is_much){
      h_nocuts->fill(event);
      h_jets_nocuts->fill(event);
      h_ele_nocuts->fill(event);
      h_mu_nocuts->fill(event);
      h_event_nocuts->fill(event);
      h_lumi_nocuts->fill(event);
    }

    bool pass_common = common->process(event);
    if(!pass_common) return false;
    SF_muonID->process(event);
    SF_muonIso->process(event);
    SF_eleReco->process(event);
    SF_eleID->process(event);

    if(is_much){
      h_cleaner->fill(event);
      h_jets_cleaner->fill(event);
      h_ele_cleaner->fill(event);
      h_mu_cleaner->fill(event);
      h_event_cleaner->fill(event);
      h_lumi_cleaner->fill(event);
    }

    // Trigger
    if(is_much){
      if(!(trigger1_much_sel->passes(event) || trigger2_much_sel->passes(event)) ) return false;
      SF_muonTrigger->process_onemuon(event,0);
    }
    else{
      if(!(trigger1_ech_sel->passes(event) || trigger2_ech_sel->passes(event)) ) return false;
      SF_eleTrigger->process(event);
      event.set(h_muon_triggerweight, 1.);
      event.set(h_muon_triggerweight_up, 1.);
      event.set(h_muon_triggerweight_down, 1.);
    }
    SF_btag->process(event);

    if(is_much){
      h_trigger->fill(event);
      h_jets_trigger->fill(event);
      h_ele_trigger->fill(event);
      h_mu_trigger->fill(event);
      h_event_trigger->fill(event);
      h_lumi_trigger->fill(event);

      h_btageff_trigger->fill(event);
    }

    // 3 b-tags
    if(!btag_loose_sel->passes(event)) return false;
    if(is_much){
      h_btag->fill(event);
      h_jets_btag->fill(event);
      h_ele_btag->fill(event);
      h_mu_btag->fill(event);
      h_event_btag->fill(event);
      h_lumi_btag->fill(event);
    }

    if(!(btag_2medium_sel->passes(event))) return false;
    if(is_much){
      h_btag_2m->fill(event);
      h_jets_btag_2m->fill(event);
      h_ele_btag_2m->fill(event);
      h_mu_btag_2m->fill(event);
      h_event_btag_2m->fill(event);
      h_lumi_btag_2m->fill(event);
    }

    if(!(btag_3medium_sel->passes(event))) return false;
    if(is_much){
      h_btag_3m->fill(event);
      h_jets_btag_3m->fill(event);
      h_ele_btag_3m->fill(event);
      h_mu_btag_3m->fill(event);
      h_event_btag_3m->fill(event);
      h_lumi_btag_3m->fill(event);
    }

    if(!(btag_2tight_sel->passes(event))) return false;
    if(is_much){
      h_btag_2t->fill(event);
      h_jets_btag_2t->fill(event);
      h_ele_btag_2t->fill(event);
      h_mu_btag_2t->fill(event);
      h_event_btag_2t->fill(event);
      h_lumi_btag_2t->fill(event);
    }


    if(!(btag_3tight_sel->passes(event))) return false;
    if(is_much){
      h_btag_3t->fill(event);
      h_jets_btag_3t->fill(event);
      h_ele_btag_3t->fill(event);
      h_mu_btag_3t->fill(event);
      h_event_btag_3t->fill(event);
      h_lumi_btag_3t->fill(event);
    }

    //TODO after each step fill 2 sets of histos instead of 1, one for ech and one for much
    //TODO what about SR/CR? This happens already at chi2 cuts, but probably we don't want to check for chi2 every time we apply a new cut? (once for SR, once for CR)

    tprime_reco->process(event);
    tprime_chi2->process(event);
    bool is_tprime_reco = event.get(h_is_tprime_reco);
    if(!is_tprime_reco) throw runtime_error("After reconstruction, the T still isn't reconstructed. How?");

    if(is_much){
      h_reco_much_sr->fill(event);
      h_jets_reco_much_sr->fill(event);
      h_ele_reco_much_sr->fill(event);
      h_mu_reco_much_sr->fill(event);
      h_event_reco_much_sr->fill(event);
      h_lumi_reco_much_sr->fill(event);

      h_reco_much_cr->fill(event);
      h_jets_reco_much_cr->fill(event);
      h_ele_reco_much_cr->fill(event);
      h_mu_reco_much_cr->fill(event);
      h_event_reco_much_cr->fill(event);
      h_lumi_reco_much_cr->fill(event);
    }
    else{
      h_reco_ech_sr->fill(event);
      h_jets_reco_ech_sr->fill(event);
      h_ele_reco_ech_sr->fill(event);
      h_mu_reco_ech_sr->fill(event);
      h_event_reco_ech_sr->fill(event);
      h_lumi_reco_ech_sr->fill(event);

      h_reco_ech_cr->fill(event);
      h_jets_reco_ech_cr->fill(event);
      h_ele_reco_ech_cr->fill(event);
      h_mu_reco_ech_cr->fill(event);
      h_event_reco_ech_cr->fill(event);
      h_lumi_reco_ech_cr->fill(event);
    }

    std::vector<SingleTthReconstructionHypothesis> hyps = event.get(h_hyps);
    const SingleTthReconstructionHypothesis* hyp = get_best_hypothesis( hyps, "Chi2" );

    float chi2 = hyp->discriminator("Chi2");
    float chi2h = hyp->discriminator("Chi2_higgs");
    float dR_bH_bH = deltaR(hyp->higgs_jets().at(0),hyp->higgs_jets().at(1));
    LorentzVector W = (hyp->lepton_v4() + hyp->neutrino_v4());
    float dR_bt_Wt = deltaR(W, hyp->toplep_jets().at(0));
    float dR_t_H = deltaR(hyp->toplep_v4(), hyp->higgs_v4());

    if(chi2h < 0.75){
      if(is_much){
        h_reco_lowchi2_much_sr->fill(event);
        h_jets_reco_lowchi2_much_sr->fill(event);
        h_ele_reco_lowchi2_much_sr->fill(event);
        h_mu_reco_lowchi2_much_sr->fill(event);
        h_event_reco_lowchi2_much_sr->fill(event);
        h_lumi_reco_lowchi2_much_sr->fill(event);
      }
      else{
        h_reco_lowchi2_ech_sr->fill(event);
        h_jets_reco_lowchi2_ech_sr->fill(event);
        h_ele_reco_lowchi2_ech_sr->fill(event);
        h_mu_reco_lowchi2_ech_sr->fill(event);
        h_event_reco_lowchi2_ech_sr->fill(event);
        h_lumi_reco_lowchi2_ech_sr->fill(event);
      }
    }
    else if(chi2h < 2.){
      if(is_much){
        h_reco_highchi2_much_sr->fill(event);
        h_jets_reco_highchi2_much_sr->fill(event);
        h_ele_reco_highchi2_much_sr->fill(event);
        h_mu_reco_highchi2_much_sr->fill(event);
        h_event_reco_highchi2_much_sr->fill(event);
        h_lumi_reco_highchi2_much_sr->fill(event);
      }
      else{
        h_reco_highchi2_ech_sr->fill(event);
        h_jets_reco_highchi2_ech_sr->fill(event);
        h_ele_reco_highchi2_ech_sr->fill(event);
        h_mu_reco_highchi2_ech_sr->fill(event);
        h_event_reco_highchi2_ech_sr->fill(event);
        h_lumi_reco_highchi2_ech_sr->fill(event);
      }
    }

    bool chi2_10_sr = (chi2 < 10.);
    bool chi2h_05_sr = (chi2h < 5.);
    bool chi2h_02_sr = (chi2h < 2.);

    if(chi2_10_sr){
      if(is_much){
        h_chi2_10_much_sr->fill(event);
        h_jets_chi2_10_much_sr->fill(event);
        h_ele_chi2_10_much_sr->fill(event);
        h_mu_chi2_10_much_sr->fill(event);
        h_event_chi2_10_much_sr->fill(event);
        h_lumi_chi2_10_much_sr->fill(event);
      }
      else{
        h_chi2_10_ech_sr->fill(event);
        h_jets_chi2_10_ech_sr->fill(event);
        h_ele_chi2_10_ech_sr->fill(event);
        h_mu_chi2_10_ech_sr->fill(event);
        h_event_chi2_10_ech_sr->fill(event);
        h_lumi_chi2_10_ech_sr->fill(event);
      }
    }
    else{
      if(is_much){
        h_chi2_10_much_cr->fill(event);
        h_jets_chi2_10_much_cr->fill(event);
        h_ele_chi2_10_much_cr->fill(event);
        h_mu_chi2_10_much_cr->fill(event);
        h_event_chi2_10_much_cr->fill(event);
        h_lumi_chi2_10_much_cr->fill(event);
      }
      else{
        h_chi2_10_ech_cr->fill(event);
        h_jets_chi2_10_ech_cr->fill(event);
        h_ele_chi2_10_ech_cr->fill(event);
        h_mu_chi2_10_ech_cr->fill(event);
        h_event_chi2_10_ech_cr->fill(event);
        h_lumi_chi2_10_ech_cr->fill(event);
      }
    }

    if(chi2_10_sr && chi2h_05_sr){
      if(is_much){
        h_chi2h_5_much_sr->fill(event);
        h_jets_chi2h_5_much_sr->fill(event);
        h_ele_chi2h_5_much_sr->fill(event);
        h_mu_chi2h_5_much_sr->fill(event);
        h_event_chi2h_5_much_sr->fill(event);
        h_lumi_chi2h_5_much_sr->fill(event);
      }
      else{
        h_chi2h_5_ech_sr->fill(event);
        h_jets_chi2h_5_ech_sr->fill(event);
        h_ele_chi2h_5_ech_sr->fill(event);
        h_mu_chi2h_5_ech_sr->fill(event);
        h_event_chi2h_5_ech_sr->fill(event);
        h_lumi_chi2h_5_ech_sr->fill(event);
      }
    }
    else{
      if(is_much){
        h_chi2h_5_much_cr->fill(event);
        h_jets_chi2h_5_much_cr->fill(event);
        h_ele_chi2h_5_much_cr->fill(event);
        h_mu_chi2h_5_much_cr->fill(event);
        h_event_chi2h_5_much_cr->fill(event);
        h_lumi_chi2h_5_much_cr->fill(event);
      }
      else{
        h_chi2h_5_ech_cr->fill(event);
        h_jets_chi2h_5_ech_cr->fill(event);
        h_ele_chi2h_5_ech_cr->fill(event);
        h_mu_chi2h_5_ech_cr->fill(event);
        h_event_chi2h_5_ech_cr->fill(event);
        h_lumi_chi2h_5_ech_cr->fill(event);
      }

    }


    if(chi2_10_sr && chi2h_02_sr){
      if(is_much){
        h_chi2h_2_much_sr->fill(event);
        h_jets_chi2h_2_much_sr->fill(event);
        h_ele_chi2h_2_much_sr->fill(event);
        h_mu_chi2h_2_much_sr->fill(event);
        h_event_chi2h_2_much_sr->fill(event);
        h_lumi_chi2h_2_much_sr->fill(event);
      }
      else{
        h_chi2h_2_ech_sr->fill(event);
        h_jets_chi2h_2_ech_sr->fill(event);
        h_ele_chi2h_2_ech_sr->fill(event);
        h_mu_chi2h_2_ech_sr->fill(event);
        h_event_chi2h_2_ech_sr->fill(event);
        h_lumi_chi2h_2_ech_sr->fill(event);
      }
    }
    else{
      if(is_much){
        h_chi2h_2_much_cr->fill(event);
        h_jets_chi2h_2_much_cr->fill(event);
        h_ele_chi2h_2_much_cr->fill(event);
        h_mu_chi2h_2_much_cr->fill(event);
        h_event_chi2h_2_much_cr->fill(event);
        h_lumi_chi2h_2_much_cr->fill(event);
      }
      else{
        h_chi2h_2_ech_cr->fill(event);
        h_jets_chi2h_2_ech_cr->fill(event);
        h_ele_chi2h_2_ech_cr->fill(event);
        h_mu_chi2h_2_ech_cr->fill(event);
        h_event_chi2h_2_ech_cr->fill(event);
        h_lumi_chi2h_2_ech_cr->fill(event);
      }
    }

    bool is_sr = chi2_10_sr && chi2h_02_sr;


    if(!(dR_bH_bH < 1.5)) return false;

    if(is_sr){
      if(is_much){
        h_dRbb_much_sr->fill(event);
        h_jets_dRbb_much_sr->fill(event);
        h_ele_dRbb_much_sr->fill(event);
        h_mu_dRbb_much_sr->fill(event);
        h_event_dRbb_much_sr->fill(event);
        h_lumi_dRbb_much_sr->fill(event);
      }
      else{
        h_dRbb_ech_sr->fill(event);
        h_jets_dRbb_ech_sr->fill(event);
        h_ele_dRbb_ech_sr->fill(event);
        h_mu_dRbb_ech_sr->fill(event);
        h_event_dRbb_ech_sr->fill(event);
        h_lumi_dRbb_ech_sr->fill(event);
      }
    }
    else{
      if(is_much){
        h_dRbb_much_cr->fill(event);
        h_jets_dRbb_much_cr->fill(event);
        h_ele_dRbb_much_cr->fill(event);
        h_mu_dRbb_much_cr->fill(event);
        h_event_dRbb_much_cr->fill(event);
        h_lumi_dRbb_much_cr->fill(event);
      }
      else{
        h_dRbb_ech_cr->fill(event);
        h_jets_dRbb_ech_cr->fill(event);
        h_ele_dRbb_ech_cr->fill(event);
        h_mu_dRbb_ech_cr->fill(event);
        h_event_dRbb_ech_cr->fill(event);
        h_lumi_dRbb_ech_cr->fill(event);
      }
    }

    if(!(dR_bH_bH < 1.1)) return false;

    if(is_sr){
      if(is_much){
        h_dRbb_11_much_sr->fill(event);
        h_jets_dRbb_11_much_sr->fill(event);
        h_ele_dRbb_11_much_sr->fill(event);
        h_mu_dRbb_11_much_sr->fill(event);
        h_event_dRbb_11_much_sr->fill(event);
        h_lumi_dRbb_11_much_sr->fill(event);
      }
      else{
        h_dRbb_11_ech_sr->fill(event);
        h_jets_dRbb_11_ech_sr->fill(event);
        h_ele_dRbb_11_ech_sr->fill(event);
        h_mu_dRbb_11_ech_sr->fill(event);
        h_event_dRbb_11_ech_sr->fill(event);
        h_lumi_dRbb_11_ech_sr->fill(event);
      }
    }
    else{
      if(is_much){
        h_dRbb_11_much_cr->fill(event);
        h_jets_dRbb_11_much_cr->fill(event);
        h_ele_dRbb_11_much_cr->fill(event);
        h_mu_dRbb_11_much_cr->fill(event);
        h_event_dRbb_11_much_cr->fill(event);
        h_lumi_dRbb_11_much_cr->fill(event);
      }
      else{
        h_dRbb_11_ech_cr->fill(event);
        h_jets_dRbb_11_ech_cr->fill(event);
        h_ele_dRbb_11_ech_cr->fill(event);
        h_mu_dRbb_11_ech_cr->fill(event);
        h_event_dRbb_11_ech_cr->fill(event);
        h_lumi_dRbb_11_ech_cr->fill(event);
      }
    }

    if(!(dR_bH_bH < 1.0)) return false;

    if(is_sr){
      if(is_much){
        h_dRbb_10_much_sr->fill(event);
        h_jets_dRbb_10_much_sr->fill(event);
        h_ele_dRbb_10_much_sr->fill(event);
        h_mu_dRbb_10_much_sr->fill(event);
        h_event_dRbb_10_much_sr->fill(event);
        h_lumi_dRbb_10_much_sr->fill(event);
      }
      else{
        h_dRbb_10_ech_sr->fill(event);
        h_jets_dRbb_10_ech_sr->fill(event);
        h_ele_dRbb_10_ech_sr->fill(event);
        h_mu_dRbb_10_ech_sr->fill(event);
        h_event_dRbb_10_ech_sr->fill(event);
        h_lumi_dRbb_10_ech_sr->fill(event);
      }
    }
    else{
      if(is_much){
        h_dRbb_10_much_cr->fill(event);
        h_jets_dRbb_10_much_cr->fill(event);
        h_ele_dRbb_10_much_cr->fill(event);
        h_mu_dRbb_10_much_cr->fill(event);
        h_event_dRbb_10_much_cr->fill(event);
        h_lumi_dRbb_10_much_cr->fill(event);
      }
      else{
        h_dRbb_10_ech_cr->fill(event);
        h_jets_dRbb_10_ech_cr->fill(event);
        h_ele_dRbb_10_ech_cr->fill(event);
        h_mu_dRbb_10_ech_cr->fill(event);
        h_event_dRbb_10_ech_cr->fill(event);
        h_lumi_dRbb_10_ech_cr->fill(event);
      }
    }

    if(!(dR_bt_Wt < 2.)) return false;
    if(is_sr){
      if(is_much){
        h_dRbw_much_sr->fill(event);
        h_jets_dRbw_much_sr->fill(event);
        h_ele_dRbw_much_sr->fill(event);
        h_mu_dRbw_much_sr->fill(event);
        h_event_dRbw_much_sr->fill(event);
        h_lumi_dRbw_much_sr->fill(event);
      }
      else{
        h_dRbw_ech_sr->fill(event);
        h_jets_dRbw_ech_sr->fill(event);
        h_ele_dRbw_ech_sr->fill(event);
        h_mu_dRbw_ech_sr->fill(event);
        h_event_dRbw_ech_sr->fill(event);
        h_lumi_dRbw_ech_sr->fill(event);
      }
    }
    else{
      if(is_much){
        h_dRbw_much_cr->fill(event);
        h_jets_dRbw_much_cr->fill(event);
        h_ele_dRbw_much_cr->fill(event);
        h_mu_dRbw_much_cr->fill(event);
        h_event_dRbw_much_cr->fill(event);
        h_lumi_dRbw_much_cr->fill(event);
      }
      else{
        h_dRbw_ech_cr->fill(event);
        h_jets_dRbw_ech_cr->fill(event);
        h_ele_dRbw_ech_cr->fill(event);
        h_mu_dRbw_ech_cr->fill(event);
        h_event_dRbw_ech_cr->fill(event);
        h_lumi_dRbw_ech_cr->fill(event);
      }
    }

    if(!(dR_bt_Wt < 1.5)) return false;
    if(is_sr){
      if(is_much){
        h_dRbw_15_much_sr->fill(event);
        h_jets_dRbw_15_much_sr->fill(event);
        h_ele_dRbw_15_much_sr->fill(event);
        h_mu_dRbw_15_much_sr->fill(event);
        h_event_dRbw_15_much_sr->fill(event);
        h_lumi_dRbw_15_much_sr->fill(event);
      }
      else{
        h_dRbw_15_ech_sr->fill(event);
        h_jets_dRbw_15_ech_sr->fill(event);
        h_ele_dRbw_15_ech_sr->fill(event);
        h_mu_dRbw_15_ech_sr->fill(event);
        h_event_dRbw_15_ech_sr->fill(event);
        h_lumi_dRbw_15_ech_sr->fill(event);
      }
    }
    else{
      if(is_much){
        h_dRbw_15_much_cr->fill(event);
        h_jets_dRbw_15_much_cr->fill(event);
        h_ele_dRbw_15_much_cr->fill(event);
        h_mu_dRbw_15_much_cr->fill(event);
        h_event_dRbw_15_much_cr->fill(event);
        h_lumi_dRbw_15_much_cr->fill(event);
      }
      else{
        h_dRbw_15_ech_cr->fill(event);
        h_jets_dRbw_15_ech_cr->fill(event);
        h_ele_dRbw_15_ech_cr->fill(event);
        h_mu_dRbw_15_ech_cr->fill(event);
        h_event_dRbw_15_ech_cr->fill(event);
        h_lumi_dRbw_15_ech_cr->fill(event);
      }
    }


    return true;
  }


  UHH2_REGISTER_ANALYSIS_MODULE(SingleTthAnalysisModule)
}
