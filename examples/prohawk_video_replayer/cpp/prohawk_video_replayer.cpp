/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include <holoscan/holoscan.hpp>
#include <holoscan/operators/video_stream_replayer/video_stream_replayer.hpp>
#include <holoscan/operators/holoviz/holoviz.hpp>
#include <holoscan/operators/prohawkop/prohawkop.hpp>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"


class ProhawkVideoReplayerApp : public holoscan::Application {
 public:
  void compose() override {
    using namespace holoscan;

    // Define the replayer and holoviz operators and configure using yaml configuration
//auto pingRX = make_operator<ops::PingRxOp>("input");
auto prohawkOP = make_operator<ops::ProhawkOp>("input");

    auto replayer = make_operator<ops::VideoStreamReplayerOp>("replayer", from_config("replayer"));
    auto visualizer = make_operator<ops::HolovizOp>("holoviz", from_config("holoviz"));

    // Define the workflow: replayer -> holoviz
//add_flow(replayer,pingRX,{{"","input"}});
add_flow(replayer,prohawkOP,{{"","input"}});
add_flow(prohawkOP, visualizer, {{"output1", "receivers"}});
//add_flow(replayer, visualizer, {{"output", "receivers"}});

  //  add_flow(replayer, visualizer, {{"output", "receivers"}});
  }
};

int main(int argc, char** argv) {
  // Get the yaml configuration file
  auto config_path = std::filesystem::canonical(argv[0]).parent_path();
  config_path += "/prohawk_video_replayer.yaml";
  if ( argc >= 2 ) {
    config_path = argv[1];
  }

  auto app = holoscan::make_application<ProhawkVideoReplayerApp>();
  app->config(config_path);
  app->run();

  return 0;
}
