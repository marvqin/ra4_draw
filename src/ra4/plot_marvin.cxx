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

  double lumi = 2.6;  

  string trig_skim_mc_sig = "/net/cms2/cms2r0/babymaker/babies/2016_06_14/mc/merged_hW_sig/";
  string trig_skim_mc_cr0b = "/net/cms2/cms2r0/babymaker/babies/2016_06_14/mc/merged_hW_cr0b/";
   
  Palette colors("txt/colors.txt", "default");
/*j
  auto tt1l = Process::MakeShared<Baby_full>("t#bar{t} (1l)", Process::Type::background, colors("tt_1l"),
    {trig_skim_mc_sig+"*_TTJets*Lept*.root", trig_skim_mc_sig+"*_TTJets_HT*.root"},
    "ntruleps<=1&&stitch");

  auto tt2l = Process::MakeShared<Baby_full>("t#bar{t} (2l)", Process::Type::background, colors("tt_2l"), 
  	{trig_skim_mc_sig+"*_TTJets*Lept*.root", trig_skim_mc_sig+"*TTJets_HT*.root"},
	"ntruleps>=2&&stitch");
*/
   auto tt1l = Process::MakeShared<Baby_full>("t#bar{t} (1l)", Process::Type::background, colors("tt_1l"),
    {trig_skim_mc_sig+"*_TTJets*Lept*.root"},
    "ntruleps<=1");
  auto tt2l = Process::MakeShared<Baby_full>("t#bar{t} (2l)", Process::Type::background, colors("tt_2l"), 
  	{trig_skim_mc_sig+"*_TTJets*Lept*.root"},
	"ntruleps>=2"); 

  auto wjets = Process::MakeShared<Baby_full>("W+jets", Process::Type::background, colors("wjets"),
  	{trig_skim_mc_sig+"*_WJetsToLNu*.root"}, "stitch");

  auto single_t = Process::MakeShared<Baby_full>("Single t", Process::Type::background, colors("single_t"),
  	{trig_skim_mc_sig+"*_ST_*.root"});

  auto ttv = Process::MakeShared<Baby_full>("t#bar{t}V", Process::Type::background, colors("ttv"),
  	{trig_skim_mc_sig+"*_TTWJets*.root", trig_skim_mc_sig+"*_TTZTo*.root"});

  auto ttH = Process::MakeShared<Baby_full>("t#bar{t}H", Process::Type::background, colors("t1tttt"),
  	{trig_skim_mc_sig+"*ttHJetTobb*.root"});

  auto WH  = Process::MakeShared<Baby_full>("WH", Process::Type::background, kOrange,
  	{trig_skim_mc_sig+"*_WH_HToBB*.root"});

  auto rare = Process::MakeShared<Baby_full>("Rare", Process::Type::background, colors("other"),
  	{trig_skim_mc_sig+"*_ZJet*.root", trig_skim_mc_sig+"_WWTo*.root", 
		trig_skim_mc_sig+"*_ZH_HToBB*.root", trig_skim_mc_sig+"*_ZZ_*.root",
		trig_skim_mc_sig+"*_WZTo*.root"});

// SIGNAL
//  auto TChiNeuWZ = Process::MakeShared<Baby_full>("ChiNeuWZ(150, 120)", Process::Type::signal, colors("t1tttt"),
//  	{"/net/cms2/cms2r0/marvinqi/out/renorm/baby_TChiNeuWZ_mCh150*.root"});
//  auto TChiNeuSlepSneu = Process::MakeShared<Baby_full>("ChiNeuSlepSneu(300,270)", Process::Type::signal, colors("t1tttt"), {"/net/cms2/cms2r0/marvinqi/out/renorm/baby_TChiNeuSlepSneu_mCh300*.root"});

// DATA
  auto data = Process::MakeShared<Baby_full>("Data", Process::Type::data, kBlack,
  	{"/net/cms2/cms2r0/babymaker/babies/2016_06_26/data/skim_hW_sig/*.root"},"pass&&json2p6");

// TChiNeuSlepSneu->SetLineStyle(2);

  vector<shared_ptr<Process> > full_trig_skim = {tt2l, tt1l, wjets, single_t, ttv, ttH, WH, rare};
  vector<shared_ptr<Process> > full_trig_skim_data = {data, tt2l, tt1l, wjets, single_t, ttv, ttH, WH, rare};

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

  PlotMaker pm;

  NamedFunc nleps_1p4e("nleps_1p4e", [](const Baby &b) -> NamedFunc::ScalarType{
	  int nleps = b.nleps();
	  for (unsigned iel=0; iel < b.els_pt()->size(); iel++) {
		if (!b.els_sig()->at(iel)) continue;
		if (abs(b.els_eta()->at(iel)) > 1.442) nleps-=1;
		else if (b.els_pt()->at(iel) < 30) nleps-=1;
		else if (b.els_reliso()->at(iel) > 5) nleps -= 1;
	  }
	  for (unsigned imu=0; imu < b.mus_pt()->size(); imu++) {
		if (!b.mus_sig()->at(imu)) continue;
		if (b.mus_pt()->at(imu) < 25) nleps-=1;
		else if (b.mus_reliso()->at(imu) > 5) nleps-=1;
	  }
	  return nleps;
  });

// TESTING nleps_1p4e
/*
  pm.Push<HistoStack>(HistoDef(50, -2.5, 2.5, "els_eta", "els_eta",
  						nleps_1p4e==1, "weight", {-1.4, 1.4}), full_trig_skim, just_lumi);

  pm.Push<HistoStack>(HistoDef(20, 0, 100, "els_pt", "els_pt", nleps_1p4e==1, "weight", {}),
  			full_trig_skim, just_lumi);
  pm.Push<HistoStack>(HistoDef(20, 0, 100, "mus_pt", "mus_pt", nleps_1p4e==1, "weight", {}),
  			full_trig_skim, just_lumi);

  pm.Push<HistoStack>(HistoDef(4, -0.5, 3.5, "nleps", "N_{leps}", nleps_1p4e==1, "weight", {}),
  			full_trig_skim, just_lumi);
  pm.Push<HistoStack>(HistoDef(5, -1.5, 3.5, nleps_1p4e, "N_{leps}", "nleps>=1", "weight", {}),
  			full_trig_skim, just_log_lumi);
*/
  NamedFunc baseline = nleps_1p4e==1&&"njets==2&&nbm==2&&met>100&&nveto==0&&mt>50"; 
  NamedFunc baseline_2l = ((nleps_1p4e==2&&"nveto==0")||(nleps_1p4e==1&&"nveto==1"))&&"njets==2&&nbm==2&&met>100&&mt>50";
 
 pm.Push<HistoStack>(HistoDef(7, -0.5, 6.5, "njets", "N_{jets}", 
 						"nleps==1&&nbm==2&&met>100&&nveto==0", "weight", {2}),
					full_trig_skim, just_lumi);
/*
  pm.Push<HistoStack>(HistoDef(20, 0, 500, "m_bb_2", "M_{bb}",
  								baseline_2l, "weight", {90, 150}),
						full_trig_skim_data, just_log_lumi); 

  pm.Push<HistoStack>(HistoDef(20, 0, 500, "met", "E^{miss}_{T}",
  								baseline_2l&&"m_bb_2>90&&m_bb_2<150", "weight", {100}),
						full_trig_skim_data, just_log_lumi);

  pm.Push<HistoStack>(HistoDef(20, 0, 500, "mt", "m_{T}",
  								baseline_2l&&"m_bb_2>90&&m_bb_2<150", "weight", {150}),
						full_trig_skim_data, just_log_lumi);
  pm.Push<HistoStack>(HistoDef(20, 0, 500, "leps_pt", "lep p_{T}",
  								baseline_2l&&"m_bb_2>90&&m_bb_2<150", "weight", {}),
						full_trig_skim_data, just_log_lumi);

  pm.Push<HistoStack>(HistoDef(20, 0, 500, b_pt[0.], "Leading b-jet p_{T}",
  								baseline_2l&&"m_bb_2>90&&m_bb_2<150", "weight", {}),
						full_trig_skim_data, just_log_lumi);
  pm.Push<HistoStack>(HistoDef(20, 0, 500, b_pt[1.], "Subleading b-jet p_{T}",
  								baseline_2l&&"m_bb_2>90&&m_bb_2<150", "weight", {}),
						full_trig_skim_data, just_log_lumi);
  
  pm.Push<HistoStack>(HistoDef(20, 0, 500, mct, "M_{CT}",
  								baseline_2l&&"m_bb_2>90&&m_bb_2<150", "weight", {150}),
						full_trig_skim_data, just_log_lumi);
*/

/* SECTION 3.1 - AGREEMENT IS GOOD
  pm.Push<HistoStack>(HistoDef(20, 0, 500, "m_bb_2", "M_{bb}",
  							baseline, "weight", {}),
						full_trig_skim_data, just_log_lumi);
  pm.Push<HistoStack>(HistoDef(20, 0, 500, "m_bb_2", "M_{bb}", 
  							baseline&&"(m_bb_2>150||m_bb_2<90)", "weight", {}),
						full_trig_skim_data, just_log_lumi);
*/

  pm.MakePlots(lumi);
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
