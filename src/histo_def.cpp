/*! \class HistoDef

  \brief Contains all information necessary to contruct and fill a histogram,
  independent of drawing style.

  HistoDef provides a means of concisely specifying to other functions and
  classes (for now, mainly HistoStack) the instructions for creating a
  histogram. It contains the binning, the variable to plot, the cut determing
  when to fill the histogram, the weight with which to fill the histogram, the
  x-axis title with units (HistoStack automatically determines the y-axis title,
  but support can be added for manual specification later), and a list of
  significant values of the plotted variable (used by HistoStack to plot
  vertical lines).
 */
#include "histo_def.hpp"

#include <algorithm>

using namespace std;

HistoDef::HistoDef(const Axis & x_axis,
                   const NamedFunc &cut,
                   const NamedFunc &weight):
  tag_(""),
  x_axis_(x_axis),
  cut_(cut),
  weight_(weight){
  }

HistoDef::HistoDef(const string &tag,
                   const Axis & x_axis,
                   const NamedFunc &cut,
                   const NamedFunc &weight):
  tag_(tag),
  x_axis_(x_axis),
  cut_(cut),
  weight_(weight){
  }

/*!\brief Get name of plot suitable for use in file name

  \return Output name specifying variable, cut, and weight
*/
string HistoDef::Name() const{
  if(tag_ == ""){
    return x_axis_.var_.PlainName() + "_CUT_" + cut_.PlainName() + "_WGT_" + weight_.PlainName();
  }else{
    return tag_+"_VAR_"+x_axis_.var_.PlainName() + "_CUT_" + cut_.PlainName() + "_WGT_" + weight_.PlainName();
  }
}

/*!\brief Get title of plot containing cut and weight

  Cut is omitted if set to "" or "1." Weight is omitted if set to "weight."

  \return TLatex formatted title with cut and weight
*/
string HistoDef::Title() const{
  bool cut = (cut_.Name() != "" && cut_.Name() != "1");
  bool weight = weight_.Name() != "weight";
  if(cut && weight){
    return cut_.PrettyName()+" (weight="+weight_.PrettyName()+")";
  }else if(cut){
    return cut_.PrettyName();
  }else if(weight){
    return "weight="+weight_.PrettyName();
  }else{
    return "";
  }
}
