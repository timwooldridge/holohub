/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES.
 * All rights reserved. SPDX-License-Identifier: Apache-2.0
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

#ifndef NVIDIA_GXF_MULTIMEDIA_EXTENSIONS_DECODE_VIDEODECODER_HPP_
#define NVIDIA_GXF_MULTIMEDIA_EXTENSIONS_DECODE_VIDEODECODER_HPP_

#include <cuda_runtime.h>
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

class VideoDecoder : public gxf::Codelet {
 public:
  // Explicitly declare constructors and destructors
  // to get around forward declaration of Impl
  VideoDecoder();
  ~VideoDecoder();

  gxf_result_t registerInterface(gxf::Registrar* registrar) override;
  gxf_result_t start() override;
  gxf_result_t tick() override;
  gxf_result_t stop() override;

 private:
  // Hide implementation details
  struct Impl;
  std::unique_ptr<Impl> impl_;

  gxf::Parameter<gxf::Handle<gxf::Allocator>> pool_;
  gxf::Parameter<gxf::Handle<gxf::Receiver>> image_receiver_;
  gxf::Parameter<gxf::Handle<gxf::Transmitter>> output_transmitter_;
  gxf::Parameter<int32_t> codec_;
  gxf::Parameter<int32_t> inbuf_storage_type_;
  gxf::Parameter<int32_t> outbuf_storage_type_;
  gxf::Parameter<std::string> output_format_;

  gxf::Handle<gxf::VideoBuffer> decoded_frame_;
  nvidia::gxf::Handle<nvidia::gxf::Allocator> memory_pool_;

  gxf::Parameter<std::string> device_;
  // Copy YUV data from V4L2 buffer to gxf videobuffer
  gxf_result_t copyYUVFrame();
};

}  // namespace gxf
}  // namespace nvidia

#endif  // NVIDIA_GXF_MULTIMEDIA_EXTENSIONS_DECODE_VIDEODECODER_HPP_
