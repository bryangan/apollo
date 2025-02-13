/******************************************************************************
 * Copyright 2019 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

/**
 *   @file
 **/

#pragma once

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "modules/common/configs/proto/vehicle_config.pb.h"
#include "modules/planning/proto/st_bounds_decider_config.pb.h"

#include "modules/common/status/status.h"
#include "modules/planning/common/obstacle.h"
#include "modules/planning/common/path/path_data.h"
#include "modules/planning/common/path_decision.h"
#include "modules/planning/common/speed/st_boundary.h"
#include "modules/planning/common/speed_limit.h"
#include "modules/planning/reference_line/reference_line.h"

namespace apollo {
namespace planning {

constexpr double kADCSafetyLBuffer = 0.1;
constexpr double kSIgnoreThreshold = 0.01;

class STObstaclesProcessor {
 public:
  STObstaclesProcessor(const double planning_distance,
                       const double planning_time, const PathData& path_data);

  virtual ~STObstaclesProcessor() = default;

  common::Status MapObstaclesToSTBoundaries(PathDecision* const path_decision);

  std::unordered_map<std::string, STBoundary> GetAllSTBoundaries();

  std::pair<double, double> GetRegularBoundaryFromObstacles(double t);

  std::pair<double, double> GetFallbackBoundaryFromObstacles(double t);

 private:
  /** @brief Given a single obstacle, compute its ST-boundary.
    * @param An obstacle (if moving, should contain predicted trajectory).
    * @param A vector to be filled with lower edge of ST-polygon.
    * @param A vector to be filled with upper edge of ST-polygon.
    * @return If appears on ST-graph, return true; otherwise, false.
    */
  bool ComputeObstacleSTBoundary(const Obstacle& obstacle,
      std::vector<STPoint>* const lower_points,
      std::vector<STPoint>* const upper_points);

  /** @brief Given ADC's path and an obstacle instance at a certain timestep,
    * get the upper and lower s that ADC might overlap with the obs instance.
    * @param A vector of ADC planned path points.
    * @param A obstacle at a certain timestep.
    * @param ADC lateral buffer for safety consideration.
    * @param The overlapping upper and lower s to be updated.
    * @return Whether there is an overlap or not.
    */
  bool GetOverlappingS(const std::vector<common::PathPoint>& adc_path_points,
                       const common::math::Box2d& obstacle_instance,
                       const double adc_l_buffer,
                       std::pair<double, double>* const overlapping_s);

  /** @brief Over the s-dimension, find the last point that is before the
   * obstacle instance of the first point that is after the obstacle.
   * If there exists no such point within the given range, return -1.
   * @param ADC path points
   * @param The obstacle box
   * @param The s threshold, must be non-negative.
   * @param The direction
   * @param The start-idx
   * @param The end-idx
   * @return Whether there is overlapping or not.
   */
  int GetSBoundingPathPointIndex(
      const std::vector<common::PathPoint>& adc_path_points,
      const common::math::Box2d& obstacle_instance, const double s_thresh,
      const bool is_before, const int start_idx, const int end_idx);

  /** @brief Over the s-dimension, check if the path-point is away
   * from the projected obstacle in the given direction.
   * @param A certain path-point.
   * @param The next path-point indicating path direction.
   * @param The obstacle bounding box.
   * @param The threshold s to tell if path point is far away.
   * @param Direction indicator. True if we want the path-point to be
   *        before the obstacle.
   * @return whether the path-point is away in the indicated direction.
   */
  bool IsPathPointAwayFromObstacle(const common::PathPoint& path_point,
                                   const common::PathPoint& direction_point,
                                   const common::math::Box2d& obs_box,
                                   const double s_thresh, const bool is_before);

  /** @brief Check if ADC is overlapping with the given obstacle box.
    * @param ADC's position.
    * @param Obstacle's box.
    * @param ADC's lateral buffer.
    * @return Whether ADC at that position is overlapping with the given
    * obstacle box.
    */
  bool IsADCOverlappingWithObstacle(const common::PathPoint& adc_path_point,
                                    const common::math::Box2d& obs_box,
                                    const double l_buffer) const;

 private:
  double planning_time_;
  double planning_distance_;
  const PathData& path_data_;
  const common::VehicleParam& vehicle_param_;
  double adc_path_init_s_;

  std::unordered_map<std::string, STBoundary> obs_id_to_st_boundary_;
};

}  // namespace planning
}  // namespace apollo
