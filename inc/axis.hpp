#ifndef H_AXIS
#define H_AXIS

#include <vector>
#include <set>
#include <string>

#include "named_func.hpp"

class Axis{
public:
  Axis(const std::vector<double> &bins,
       const NamedFunc &var,
       const std::string &title = "",
       const std::set<double> &cut_vals = {});
  Axis(std::size_t nbins, double axis_min, double axis_max,
       const NamedFunc &var,
       const std::string &title = "",
       const std::set<double> &cut_vals = {});
  Axis(const Axis &) = default;
  Axis& operator=(const Axis &) = default;
  Axis(Axis &&) = default;
  Axis& operator=(Axis &&) = default;
  ~Axis() = default;

  Axis & Bins(const std::vector<double> &bins);
  Axis & Bins(std::size_t nbins,
              double axis_min,
              double axis_max);
  const std::vector<double> & Bins() const;

  NamedFunc var_;//!< Variable to be plotted
  std::string title_;//!< Title of axis
  std::string units_;//!< Units of Axis::var_
  std::set<double> cut_vals_;//!< Values for which to plot divider line

private:
  std::vector<double> bins_;//!< List of bin edges
  static std::vector<double> GetEdges(std::size_t nbins,
                                      double axis_min,
                                      double axis_max);
  void ParseUnits();
};

#endif
