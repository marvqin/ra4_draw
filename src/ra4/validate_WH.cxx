#include "core/test.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include <unistd.h>
#include <getopt.h>

#include "TError.h"
#include "TColor.h"

#include "core/baby.hpp"
#include "core/process.hpp"
#include "core/named_func.hpp"
#include "core/plot_maker.hpp"
#include "core/plot_opt.hpp"
#include "core/palette.hpp"
#include "core/table.hpp"
#include "core/histo_stack.hpp"
#include "core/functions.hpp"

using namespace std;
using namespace PlotOptTypes;
using namespace Functions;

namespace{
  bool single_thread = false;
}

int main (int argc, char *argv[]){
  gErrorIgnoreLevel = 6000;
  GetOptions(argc, argv);

//  double lumi = 10.0;  
  double data_lumi = 10.0;
  bool nob = false;

  string trig_skim_mc_sig = "/net/cms2/cms2r0/babymaker/babies/2016_06_14/mc/merged_hW_sig/";
  string trig_skim_mc_cr0b = "/net/cms2/cms2r0/babymaker/babies/2016_06_14/mc/merged_hW_cr0b/";
  string trig_skim_mc = nob ? trig_skim_mc_cr0b : trig_skim_mc_sig;
   
  Palette colors("txt/colors.txt", "default");
  auto tt1l = Process::MakeShared<Baby_full>("1l", Process::Type::background, colors("tt_1l"),
    {trig_skim_mc+"*_TTJets*Lept*.root", trig_skim_mc+"*_TTJets*HT*.root"},
    "ntruleps<=1&&stitch");
  auto tt2l = Process::MakeShared<Baby_full>("2l", Process::Type::background, colors("tt_2l"), 
  	{trig_skim_mc+"*_TTJets*Lept*.root", trig_skim_mc+"*_TTJets*HT*.root" },
	"ntruleps>=2&&stitch"); 
  auto st = Process::MakeShared<Baby_full>("Single t", Process::Type::background, colors("single_t"),
    {trig_skim_mc+"*_ST_*.root"});
  auto wjets = Process::MakeShared<Baby_full>("W+jets", Process::Type::background, colors("wjets"),
  	{trig_skim_mc+"*_WJetsToLNu*.root"}, "stitch");
  auto wz = Process::MakeShared<Baby_full>("WZ", Process::Type::background, colors("wz"),
    {trig_skim_mc+"*_WZTo*.root"});
  
/* moved to rare
  auto ttv = Process::MakeShared<Baby_full>("t#bar{t}V", Process::Type::background, colors("ttv"),
  	{trig_skim_mc+"*_TTWJets*.root", trig_skim_mc+"*_TTZTo*.root"});

  auto ttH = Process::MakeShared<Baby_full>("t#bar{t}H", Process::Type::background, colors("t1tttt"),
  	{trig_skim_mc+"*ttHJetTobb*.root"});

  auto WH  = Process::MakeShared<Baby_full>("WH", Process::Type::background, kOrange,
  	{trig_skim_mc+"*_WH_HToBB*.root"});
*/
  auto rare = Process::MakeShared<Baby_full>("Rare", Process::Type::background, colors("other"),
  	{trig_skim_mc+"*_ZJet*.root", trig_skim_mc+"_WWTo*.root", 
		trig_skim_mc+"*_ZH_HToBB*.root", trig_skim_mc+"*_ZZ_*.root",
		trig_skim_mc+"*_WZTo*.root", trig_skim_mc+"*_TTWJets*.root", 
		trig_skim_mc+"*_TTZTo*.root", trig_skim_mc+"*ttHJetTobb*.root",
		trig_skim_mc+"*WH_HToBB*.root"});

// DATA
  auto data = Process::MakeShared<Baby_full>("Data", Process::Type::data, kBlack,
  	{"/net/cms2/cms2r0/babymaker/babies/2016_06_26/data/skim_hW_incl/*.root"},"pass&&json2p6&&(trig[32]||trig[22])");

  auto sig_250_1 = Process::MakeShared<Baby_full>("(250,1)x50", Process::Type::signal, kGreen,
  	{"/net/cms2/cms2r0/marvinqi/out/*250_mLSP-1_*.root"});
  auto sig_300_75 = Process::MakeShared<Baby_full>("(300,75)x50", Process::Type::signal, kRed,
  	{"/net/cms2/cms2r0/marvinqi/out/*300_mLSP-75_*.root"});

// TChiNeuSlepSneu->SetLineStyle(2);

  vector<shared_ptr<Process> > full_trig_skim = {tt2l, tt1l, st, wjets, wz, rare, sig_250_1, sig_300_75 };
  vector<shared_ptr<Process> > full_trig_skim_data = {data, tt2l, tt1l, st, wjets, wz, rare};

  vector<shared_ptr<Process> > signal = {sig_250_1, sig_300_75};

  PlotOpt log_lumi("txt/plot_styles.txt", "CMSPaper");
  log_lumi.Title(TitleType::preliminary)
    .Bottom(BottomType::off)
    .YAxis(YAxisType::log)
    .Stack(StackType::data_norm);
  PlotOpt lin_lumi = log_lumi().YAxis(YAxisType::linear);
  PlotOpt log_shapes = log_lumi().Stack(StackType::shapes)
    .Bottom(BottomType::off)
    .ShowBackgroundError(false);
  PlotOpt lin_shapes = log_shapes().YAxis(YAxisType::linear);
  PlotOpt log_lumi_info = log_lumi().Title(TitleType::info);
  PlotOpt lin_lumi_info = lin_lumi().Title(TitleType::info);
  PlotOpt log_shapes_info = log_shapes().Title(TitleType::info);
  PlotOpt lin_shapes_info = lin_shapes().Title(TitleType::info);
  vector<PlotOpt> all_plot_types = {log_lumi, lin_lumi, log_shapes, lin_shapes,
                                    log_lumi_info, lin_lumi_info, log_shapes_info, lin_shapes_info};

  vector<PlotOpt> just_log_lumi = {log_lumi_info};
  vector<PlotOpt> just_lumi = {lin_lumi_info};

  vector<PlotOpt> lin_log = {lin_lumi_info, log_lumi_info};

  vector<PlotOpt> just_log_lumi_ratio = {log_lumi_info.Bottom(BottomType::ratio)};
  vector<PlotOpt> just_lumi_ratio = {lin_lumi_info.Bottom(BottomType::ratio)};


  PlotMaker pm;

  NamedFunc nleps_WH("nleps_WH", [](const Baby &b) -> NamedFunc::ScalarType{
      int nmus = 0; int nels = 0;
	  for (unsigned imu=0; imu < b.mus_pt()->size(); imu++) {
		if (b.mus_pt()->at(imu)>25 && b.mus_eta()->at(imu)<2.1 && b.mus_tight()->at(imu)
			&& b.mus_reliso()->at(imu)*b.mus_pt()->at(imu)<5)
			nmus++;
	  }
	  for (unsigned iel=0; iel < b.els_pt()->size(); iel++) {
		if (b.els_pt()->at(iel)>30 && b.els_eta()->at(iel)<1.442 && b.els_sigid()->at(iel)
			&& b.els_reliso()->at(iel)*b.els_pt()->at(iel)<5)
			nels++;
	  }
	  return nmus+nels;
  });

  NamedFunc nveto_WH("nveto_WH", [](const Baby &b) -> NamedFunc::ScalarType{
      int nvto = 0;
	  for (unsigned itk=0; itk<b.tks_pt()->size(); itk++) {
		if (b.tks_pt()->at(itk)>10 && b.tks_eta()->at(itk)<2.4 && b.tks_dz()->at(itk)<0.1
			&& (b.tks_pt()->at(itk)>60 ? b.tks_reliso()->at(itk)*b.tks_pt()->at(itk)<6 : 
			b.tks_miniso()->at(itk)<0.1))
			nvto++;
	  }
	  return nvto;
  });


  NamedFunc baseline = nleps_WH==1.&&nveto_WH==0.&&"njets==2&&nbm==2&&met>100&&mt>50"; 
  NamedFunc baseline_2l = ((nleps_WH==2.&&nveto_WH==0.)||(nleps_WH==1.&&nveto_WH==1.))&&"njets==2&&nbm==2&&met>100&&mt>50";

// key variable distributions are similar to WH analysis.
/*
  pm.Push<HistoStack>(HistoDef(50, 0, 500, "m_bb_2", "M_{bb}",
  								baseline, "weight", {90, 150}),
						full_trig_skim, just_lumi); 
  
  pm.Push<HistoStack>(HistoDef(50, 0, 500, m_bb, "MBB",
  								baseline, "weight", {90, 150}),
						full_trig_skim, lin_log);

  pm.Push<HistoStack>(HistoDef(50, 0, 500, "met", "E^{miss}_{T}",
  								baseline&&m_bb>90&&m_bb<150, "weight", {100}),
						full_trig_skim, lin_log);

  pm.Push<HistoStack>(HistoDef(50, 0, 500, "mt", "m_{T}",
  								baseline&&m_bb>90&&m_bb<150, "weight", {150}),
						full_trig_skim, lin_log);
  pm.Push<HistoStack>(HistoDef(20, 0, 500, "leps_pt", "lep p_{T}",
  								baseline&&m_bb>90&&m_bb<150, "weight", {}),
						full_trig_skim, lin_log);
  
  pm.Push<HistoStack>(HistoDef(50, 0, 500, mct, "M_{CT}",
  								baseline&&m_bb>90&&m_bb<150, "weight", {150}),
						full_trig_skim, lin_log);
*/

// Data/MC agreement is good, CR2l looks similar to paper
/*
  pm.Push<HistoStack>(HistoDef(20, 0, 500, m_bb, "M_{bb}",
  							baseline_2l, "weight", {}),
						full_trig_skim_data, just_log_lumi_ratio);
   pm.Push<HistoStack>(HistoDef(20, 0, 500, "mt", "m_T",
  							baseline_2l&&m_bb<150&&m_bb>90, "weight", {}),
						full_trig_skim_data, just_log_lumi_ratio);
  pm.Push<HistoStack>(HistoDef(20, 0, 500, mct, "m_{CT}",
  							baseline_2l&&m_bb<150&&m_bb>90, "weight", {}),
						full_trig_skim_data, just_log_lumi_ratio);
  pm.Push<HistoStack>(HistoDef(20, 0, 500, "met", "E^{miss}_{T}",
  							baseline_2l&&m_bb<150&&m_bb>90, "weight", {}),
						full_trig_skim_data, just_log_lumi_ratio);

  pm.Push<HistoStack>(HistoDef(20, 0, 500, m_bb, "M_{bb}", 
  							baseline&&(m_bb>150||m_bb<90), "weight", {}),
						full_trig_skim_data, just_log_lumi_ratio);
  pm.Push<HistoStack>(HistoDef(20, 0, 500, "mt", "m_{T}", 
  							baseline&&(m_bb>150||m_bb<90), "weight", {}),
						full_trig_skim_data, just_log_lumi_ratio);
  pm.Push<HistoStack>(HistoDef(20, 0, 500, mct, "m_{CT}", 
  							baseline&&(m_bb>150||m_bb<90), "weight", {}),
						full_trig_skim_data, just_log_lumi_ratio);	
  pm.Push<HistoStack>(HistoDef(20, 0, 500, "met", "E^{misS}_T",
  							baseline&&(m_bb>150||m_bb<90), "weight", {}),
						full_trig_skim_data, just_log_lumi_ratio);
*/ 
// CRbb control region
/*
  pm.Push<HistoStack>(HistoDef(20, 0, 500, "m_bb_2", "M_{bb}",
  							baseline&&"(m_bb_2>150||m_bb_2<90)", "weight", {}),
						full_trig_skim_data, just_log_lumi_ratio);
  pm.Push<HistoStack>(HistoDef(20, 0, 500, mct, "M_{CT}",
  							baseline&&"(m_bb_2>150||m_bb_2<90)", "weight", {}),
						full_trig_skim_data, just_log_lumi_ratio);
  pm.Push<HistoStack>(HistoDef(20, 0, 500, "met", "E^{miss}_{T}",
  							baseline&&"(m_bb_2>150||m_bb_2<90)", "weight", {}),
						full_trig_skim_data, just_log_lumi_ratio);
  pm.Push<HistoStack>(HistoDef(20, 0, 500, "mt", "m_{T}",
  							baseline&&"(m_bb_2>150||m_bb_2<90)", "weight", {}),
						full_trig_skim_data, just_log_lumi_ratio);

*/ 
/*
 pm.Push<HistoStack>(HistoDef(20, 0, 500, m_jj, "M_{jj}",
  							baseline, "weight", {90, 150}),
						full_trig_skim_data, just_log_lumi_ratio);
 pm.Push<HistoStack>(HistoDef(20, 0, 500, "met", "E^{miss}_{T}", 
 							baseline&&(m_jj>90&&m_jj<150), "weight", {100}),
						full_trig_skim_data, just_log_lumi_ratio);
 pm.Push<HistoStack>(HistoDef(20, 0, 500, "mt", "m_{T}",
  								baseline&&m_jj>90&&m_jj<150, "weight", {150}),
						full_trig_skim_data, just_log_lumi_ratio);
 pm.Push<HistoStack>(HistoDef(20, 0, 500, mct, "M_{CT}",
 							baseline&&m_jj>90&&m_jj<150, "weight", {150}),
						full_trig_skim_data, just_log_lumi_ratio);
*/ 
/*
  pm.Push<Table>("cutflow_sig", vector<TableRow>{
	  TableRow("Baseline"),
	  TableRow("$1\\ell$, $E_{\\text{T}}^{\\text{miss}}>100$, $N_{b}=2$", nleps_WH==1.&&"met>100&&nbm==2"),
	  TableRow("Track veto", nleps_WH==1.&&"met>100&&nbm==2"&&nveto_WH==0.),
	  TableRow("N_{\\text{jets}}=2", nleps_WH==1.&&"met>100&&njets==2&&nbm==2"&&nveto_WH==0.),
	  TableRow("M_{T}>150", nleps_WH==1.&&"met>100&&njets==2&&nbm==2&&mt>150"&&nveto_WH==0.),
	  TableRow("M_{CT}>150", nleps_WH==1.&&"met>100&&njets==2&&mt>150&&nbm==2"&&mct>150&&nveto_WH==0.),
	  TableRow("$90<M_{bb}<150", nleps_WH==1.&&"met>100&&njets==2&&mt>150&&nbm==2"&&mct>150&&nveto_WH==0.&&m_bb>90&&m_bb<150)
  }, full_trig_skim);

  pm.Push<Table>("cutflow_cr2l", vector<TableRow>{
	TableRow("Preselection"),
	TableRow("$2\\ell$, $E_{\\text{T}}^{\\text{miss}}>100$", ((nleps_WH==2.&&nveto_WH==0.)||(nleps_WH==1.&&nveto_WH==1.))&&"met>100"),
	TableRow("N_{b}=2", ((nleps_WH==2.&&nveto_WH==0.)||(nleps_WH==1.&&nveto_WH==1.))&&"met>100&&nbm==2"),
	TableRow("N_{\\text{jets}}=2", ((nleps_WH==2.&&nveto_WH==0.)||(nleps_WH==1.&&nveto_WH==1.))&&"met>100&&nbm==2&&njets==2"),
	TableRow("m_{T}>50",((nleps_WH==2.&&nveto_WH==0.)||(nleps_WH==1.&&nveto_WH==1.))&&"met>100&&nbm==2&&njets==2&&mt>50")
  }, full_trig_skim_data);
*/

  pm.Push<Table>("signal_eff", vector<TableRow>{
	TableRow("No selection", "1"),
	TableRow("$1\\ell$", nleps_WH>=1.),
	TableRow("2nd lepton veto", nleps_WH==1.),
	TableRow("track veto", nleps_WH==1.&&nveto_WH==0.),
	TableRow("$E_{\\text{T}}^{\\text{miss}}>100$", nleps_WH==1.&&nveto_WH==0.&&"met>100"),
	TableRow("m_{T}>150", nleps_WH==1.&&nveto_WH==0.&&"met>100&&mt>150"),
	TableRow("==2 jets", nleps_WH==1.&&nveto_WH==0.&&"met>100&&mt>150&&njets==2"),
	TableRow("==2 btags", nleps_WH==1.&&nveto_WH==0.&&"met>100&&mt>150&&njets==2&&nbm==2"),
	TableRow("90<M_{bb}<150", nleps_WH==1.&&nveto_WH==0.&&"met>100&&mt>150&&njets==2&&nbm==2"&&m_bb>90&&m_bb<150),
	TableRow("M_{CT}>150", nleps_WH==1.&&nveto_WH==0.&&"met>100&&mt>150&&njets==2&&nbm==2"&&m_bb>90.&&m_bb<150.&&mct>150.)
  }, signal);
  pm.MakePlots(data_lumi);
}

void GetOptions(int argc, char *argv[]){
  while(true){
    static struct option long_options[] = {
      {"single_thread", no_argument, 0, 's'},
      {0, 0, 0, 0}
    };

    char opt = -1;
    int option_index;
    opt = getopt_long(argc, argv, "s", long_options, &option_index);
    
    if( opt == -1) break;
    
    string optname;
    switch(opt){
    case 's':
      single_thread = true;
      break;
    case 0:
      optname = long_options[option_index].name;
      if(false){
      }else{
        printf("Bad option! Found option name %s\n", optname.c_str());
      }
      break;
    default:
      printf("Bad option! getopt_long returned character code 0%o\n", opt);
      break;
    }
  }
}
