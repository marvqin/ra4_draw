#ifndef H_HISTO_DEF
#define H_HISTO_DEF

#include <vector>
#include <set>
#include <string>

#include "axis.hpp"
#include "named_func.hpp"

class HistoDef{
public:
  HistoDef(const Axis &x_axis,
           const NamedFunc &cut = 1.,
           const NamedFunc &weight = "weight");
  HistoDef(const std::string &tag,
           const Axis &x_axis,
           const NamedFunc &cut = 1.,
           const NamedFunc &weight = "weight");
  HistoDef(const HistoDef &) = default;
  HistoDef& operator=(const HistoDef &) = default;
  HistoDef(HistoDef &&) = default;
  HistoDef& operator=(HistoDef &&) = default;
  ~HistoDef() = default;

  std::string Name() const;
  std::string Title() const;

  std::string tag_;//!< Unique identifier for disambiguating plots
  Axis x_axis_;//!< Title and variable to plot on X-axis
  NamedFunc cut_;//!< Cut determining whether histogram is filled
  NamedFunc weight_;//!< Weight with which to fill histogram
};

#endif
