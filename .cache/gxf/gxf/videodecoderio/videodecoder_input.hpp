/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES.
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

#ifndef NVIDIA_GXF_MULTIMEDIA_EXTENSIONS_DECODE_VIDEODECODER_INPUT_HPP_
#define NVIDIA_GXF_MULTIMEDIA_EXTENSIONS_DECODE_VIDEODECODER_INPUT_HPP_

#include <cuda_runtime.h>
#include <string>
#include <vector>

#include "gxf/std/allocator.hpp"
#include "gxf/std/codelet.hpp"
#include "gxf/std/receiver.hpp"
#include "gxf/std/tensor.hpp"
#include "gxf/std/transmitter.hpp"

namespace nvidia {
namespace gxf {

class VideoReadBitStream : public gxf::Codelet {
 public:
  gxf_result_t registerInterface(gxf::Registrar* registrar) override;
  gxf_result_t start() override;
  gxf_result_t tick() override;
  gxf_result_t stop() override;

 private:
  gxf::Parameter<gxf::Handle<gxf::Transmitter>> output_transmitter_;
  gxf::Parameter<std::string> input_file_path_;
  gxf::Parameter<gxf::Handle<gxf::Allocator>> pool_;
  gxf::Parameter<int32_t> outbuf_storage_type_;
  gxf::Parameter<int32_t> aud_nal_present_;

  // File stream
  std::FILE* file_;
  uint32_t TotalCount;
  uint8_t* pBitStreamBuffer;
  uint32_t eos_count;
  uint8_t* stream_buf_;
  uint8_t* stream_buf_dptr_;
};

}  // namespace gxf
}  // namespace nvidia

#endif  // NVIDIA_GXF_MULTIMEDIA_EXTENSIONS_DECODE_VIDEODECODER_INPUT_HPP_
