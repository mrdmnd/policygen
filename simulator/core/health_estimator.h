#pragma once

#include <map>

#include "simulator/util/map_interpolate.h"

#include "glog/logging.h"

namespace simulator {
namespace core {

// A class that supports arbitrary pre-determined enemy health scheduling.
class HealthEstimator {
 public:
  // The public interface for this class is a method that can be called to get
  // the current health percentage of this enemy, given the time progression
  // through the overall encounter. We implement this as linear interpolation
  // between (construction-time) specified control points.
  HealthEstimator(const std::map<double, double>& control_points) :
    control_points_(control_points){};

  // Encounter progress is measured between 0.0 and 1.0, and can be computed as
  // current_time / total_combat_length in calling code.
  inline double InterpolateHealthPercentage(const double time_progress) {
    DCHECK(time_progress >= 0.0);
    DCHECK(time_progress <= 1.0);
    return simulator::util::MapInterpolate(time_progress, control_points_);
  }

  // Convenience functions for some common preconfigured estimators.
  static HealthEstimator UniformHealthEstimator() {
    return HealthEstimator({{0.0, 1.0}, {1.0, 0.0}});
  }

 private:
  // Control_points is a map from time_progress to health_progress values, i.e.
  // a uniform estimator has values that look like this:
  // { { 0.0, 1.0 }, { 0.05, 0.95}, ... {0.95, 0.05}, {1.0, 0.0} }
  const std::map<double, double> control_points_;
};
}  // namespace core
}  // namespace simulator