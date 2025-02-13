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

#include "modules/control/common/mrac_controller.h"

#include <iostream>
#include <string>

#include "cyber/common/file.h"
#include "gtest/gtest.h"
#include "modules/control/proto/control_conf.pb.h"
#include "modules/control/proto/mrac_conf.pb.h"

namespace apollo {
namespace control {

class MracControllerTest : public ::testing::Test {
 public:
  virtual void SetUp() {
    std::string control_conf_file =
        "/apollo/modules/control/testdata/conf/control_conf.pb.txt";
    CHECK(cyber::common::GetProtoFromFile(control_conf_file, &control_conf_));
    lat_controller_conf_ = control_conf_.lat_controller_conf();
  }

 protected:
  ControlConf control_conf_;
  LatControllerConf lat_controller_conf_;
};

TEST_F(MracControllerTest, MracControl) {
  double dt = 0.01;
  MracConf mrac_conf = lat_controller_conf_.actuation_mrac_conf();
  MracController mrac_controller;
  mrac_controller.Init(mrac_conf, dt);
  mrac_controller.Reset();
  EXPECT_NEAR(mrac_controller.Control(10.0, 5.0, dt), 0.0, 1e-6);
  EXPECT_NEAR(mrac_controller.CurrentReferenceState(), 5.0, 1e-6);
  mrac_controller.Reset();
  EXPECT_NEAR(mrac_controller.Control(10.0, 10.0, dt), -5.0, 1e-6);
  mrac_controller.Reset();
  EXPECT_NEAR(mrac_controller.Control(10.0, 15.0, dt), -16.25, 1e-6);
  mrac_controller.Reset();
  double control_value = mrac_controller.Control(-20.0, -25.0, dt);
  EXPECT_NEAR(control_value, 76.875, 1e-6);
  dt = 0.0;
  EXPECT_EQ(mrac_controller.Control(-25.0, -30.0, dt), control_value);
}

}  // namespace control
}  // namespace apollo
