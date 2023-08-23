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

#ifndef NVIDIA_GXF_MULTIMEDIA_EXTENSIONS_VIDEOENCODER_HPP_
#define NVIDIA_GXF_MULTIMEDIA_EXTENSIONS_VIDEOENCODER_HPP_

#include <memory>
#include <queue>
#include <string>
#include <utility>
#include <vector>

#include "gxf/core/component.hpp"
#include "gxf/core/gxf.h"
#include "gxf/core/handle.hpp"
#include "gxf/multimedia/video.hpp"
#include "gxf/std/codelet.hpp"
#include "gxf/std/parameter_parser_std.hpp"
#include "gxf/std/receiver.hpp"
#include "gxf/std/tensor.hpp"
#include "gxf/std/transmitter.hpp"

namespace nvidia {
namespace gxf {

// Min/Max resolution supported by the video encoder.
constexpr uint32_t  kVideoEncoderMinHeight = 128;
constexpr uint32_t  kVideoEncoderMinWidth = 128;
constexpr uint32_t  kVideoEncoderMaxHeight = 4096;
constexpr uint32_t  kVideoEncoderMaxWidth = 4096;

class VideoEncoder : public gxf::Codelet {
 public:
  // Explicitly declare constructors and destructors
  // to get around forward declaration of Impl
  VideoEncoder();
  ~VideoEncoder();

  gxf_result_t registerInterface(gxf::Registrar* registrar) override;
  gxf_result_t start() override;
  gxf_result_t tick() override;
  gxf_result_t stop() override;

 private:
  // Hide implementation details
  struct Impl;
  std::unique_ptr<Impl> impl_;

  // Encoder I/O related Parameters
  gxf::Parameter<gxf::Handle<gxf::Receiver>> input_frame_;
  gxf::Parameter<gxf::Handle<gxf::Transmitter>> output_transmitter_;
  gxf::Parameter<gxf::Handle<gxf::Allocator>> pool_;
  gxf::Parameter<uint32_t> outbuf_storage_type_;
  gxf::Parameter<uint32_t> inbuf_storage_type_;

  // Encoder Paramaters
  gxf::Parameter<std::string> device_;
  gxf::Parameter<int32_t> codec_;
  gxf::Parameter<uint32_t> input_height_;
  gxf::Parameter<uint32_t> input_width_;
  gxf::Parameter<std::string> input_format_;
  gxf::Parameter<int32_t> profile_;
  gxf::Parameter<int32_t> bitrate_;
  gxf::Parameter<int32_t> framerate_;

  gxf_result_t queueInputYUVBuf(const gxf::Handle<gxf::VideoBuffer> input_img);
  gxf_result_t checkInputParams();
};

}  // namespace gxf
}  // namespace nvidia

#endif  // NVIDIA_GXF_MULTIMEDIA_EXTENSIONS_VIDEOENCODER_HPP_
