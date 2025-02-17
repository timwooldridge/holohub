/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
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
 */

//#ifndef HOLOSCAN_OPERATORS_PING_RX_HPP
#ifndef HOLOSCAN_OPERATORS_PROHAWKOP_HPP
//#define HOLOSCAN_OPERATORS_PING_RX_HPP
#define HOLOSCAN_OPERATORS_PROHAWKOP_HPP

#include <holoscan/holoscan.hpp>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "PTGDE/CPTGDE.h"

namespace holoscan::ops {

class ProhawkOp : public Operator {
 public:
  HOLOSCAN_OPERATOR_FORWARD_ARGS(ProhawkOp)

  ProhawkOp() = default;

CPTGDE de;
CPTGDE_PARAMETER p;
cv::Mat logo_mat=cv::imread("ProhawkLogo.png");

bool reset_set=false;
bool prohawkStartFlag=false;
bool phawklogo=false;

int filter1;
char filter_tmp;
std::string selectedFilter;
bool sbsview=false;
void setup(OperatorSpec& spec) override;

void compute(InputContext& op_input, OutputContext&, ExecutionContext&) override;
};


}  // namespace holoscan::ops

#endif /* HOLOSCAN_OPERATORS_PING_RX_HPP */
