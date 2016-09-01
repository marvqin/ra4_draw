#include "core/test.hpp"

#include <cstdlib>
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
#include "core/event_scan.hpp"
#include "core/utilities.hpp"

using namespace std;
using namespace PlotOptTypes;

namespace{
  bool single_thread = false;
}

int main(int argc, char *argv[]){
  gErrorIgnoreLevel = 6000;
  GetOptions(argc, argv);

  double lumi = 1.0;

  string trig_skim_mc = "/net/cms2/cms2r0/babymaker/babies/2016_08_10/data/uncombined/skim_standard/";

  Palette colors("txt/colors.txt", "default");
/*
  auto unskim = Process::MakeShared<Baby_full>("unskimmed", Process::Type::data, kBlack,
  	{trig_skim_mc+"baby_0_Run2016_HTMHT_MET_JetHT_SingleElectron_SingleMuon_runs27*.root", trig_skim_mc+"baby_1_Run2016_HTMHT_MET_JetHT_SingleElectron_SingleMuon_runs27*.root"});
*/

  auto unproc = Process::MakeShared<Baby_full>("unprocessed", Process::Type::signal, kYellow,
  	{trig_skim_mc+"fullbaby_MET_Run2016*.root"});

 vector<shared_ptr<Process> > full_trig_skim = {unproc};

  PlotOpt log_lumi("txt/plot_styles.txt", "CMSPaper");
  log_lumi.Title(TitleType::preliminary)
    .Bottom(BottomType::ratio)
    .YAxis(YAxisType::log)
    .Stack(StackType::data_norm);
  PlotOpt lin_lumi = log_lumi().YAxis(YAxisType::linear);
  PlotOpt log_shapes = log_lumi().Stack(StackType::shapes)
    .ShowBackgroundError(false);
  PlotOpt lin_shapes = log_shapes().YAxis(YAxisType::linear);
  PlotOpt log_lumi_info = log_lumi().Title(TitleType::info);
  PlotOpt lin_lumi_info = lin_lumi().Title(TitleType::info);
  PlotOpt log_shapes_info = log_shapes().Title(TitleType::info);
  PlotOpt lin_shapes_info = lin_shapes().Title(TitleType::info);
  vector<PlotOpt> all_plot_types = {log_lumi, lin_lumi, log_shapes, lin_shapes,
                                    log_lumi_info, lin_lumi_info, log_shapes_info, lin_shapes_info};
  vector<PlotOpt> lin_lumi_dif = {lin_lumi.Bottom(BottomType::diff).Stack(StackType::signal_overlay)};

  PlotMaker pm;
/*
  pm.Push<HistoStack>(HistoDef(5000, 100, 200, "met", "MET",
  								"trig[14]&&met>100&&met<200", "1", {}),
						full_trig_skim, lin_lumi_dif); 
  pm.Push<HistoStack>(HistoDef(5000, 200, 300, "met", "MET",
  								"trig[14]&&met>200&&met<300", "1", {}),
						full_trig_skim, lin_lumi_dif); 
  pm.Push<HistoStack>(HistoDef(5000, 300, 400, "met", "MET",
  								"trig[14]&&met>300&&met<400", "1", {}),
						full_trig_skim, lin_lumi_dif); 
  pm.Push<HistoStack>(HistoDef(1000, 400, 500, "met", "MET",
  								"trig[14]&&met>400&&met<500", "1", {}),
						full_trig_skim, lin_lumi_dif); 
  pm.Push<HistoStack>(HistoDef(500, 500, 600, "met", "MET",
  								"trig[14]&&met>500&&met<600", "1", {}),
						full_trig_skim, lin_lumi_dif); 
*/

  pm.Push<Table>("cutflow", vector<TableRow>{
      TableRow("Baseline"),
        TableRow("trig 14", "trig[14]"),
		TableRow("590, 600", "trig[14]&&met>590&&met<600"),
		TableRow("600, 602", "trig[14]&&met>600&&met<602"),
		TableRow("602, 604", "trig[14]&&met>602&&met<604"),
		TableRow("604, 606", "trig[14]&&met>604&&met<606"),
		TableRow("606, 608", "trig[14]&&met>606&&met<608"),
		TableRow("608, 610", "trig[14]&&met>608&&met<610"),
        }, full_trig_skim);

//  pm.Push<EventScan>("scan", true, vector<NamedFunc>{"weight", "met"}, vector<shared_ptr<Process> >{tt1l});

  if(single_thread) pm.multithreaded_ = false;
  pm.multithreaded_ = false;
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
