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

#ifndef NVIDIA_GXF_MULTIMEDIA_EXTENSIONS_VIDEOENCODER_OUTPUT_HPP_
#define NVIDIA_GXF_MULTIMEDIA_EXTENSIONS_VIDEOENCODER_OUTPUT_HPP_

#include <cstdio>
#include <fstream>
#include <string>

#include "gxf/multimedia/video.hpp"
#include "gxf/std/codelet.hpp"
#include "gxf/std/receiver.hpp"
#include "gxf/std/tensor.hpp"
#include "gxf/std/transmitter.hpp"

namespace nvidia {
namespace gxf {

// Video Saving Codelet.
class VideoWriteBitstream : public gxf::Codelet {
 public:
  VideoWriteBitstream()  = default;
  ~VideoWriteBitstream() = default;

  gxf_result_t registerInterface(gxf::Registrar* registrar) override;
  gxf_result_t initialize() override {
    return GXF_SUCCESS;
  }
  gxf_result_t deinitialize() override {
    return GXF_SUCCESS;
  }

  gxf_result_t start() override;
  gxf_result_t tick() override;
  gxf_result_t stop() override;

 private:
  // The path to save the video
  gxf::Parameter<std::string> output_video_path_;
  /// The width of the output video
  gxf::Parameter<int> frame_width_;
  // The height of the output video
  gxf::Parameter<int> frame_height_;
  // Input buffer storage type
  gxf::Parameter<int> inbuf_storage_type_;
  // Data receiver to get data
  gxf::Parameter<gxf::Handle<gxf::Receiver>> data_receiver_;
  // File for CRC verification
  gxf::Parameter<std::string> input_crc_file_path_;

  // File stream
  std::FILE* file_;
  uint32_t frame_num_;
  unsigned char* stream_buf_;
  unsigned char* stream_buf_dptr_;

  // CRC related functions
  std::FILE* inputCRCFile;
  /* Creates CRC Table */
  void BuildCRCTable(uint32_t* crcTable);
  /* Calculates the CRC for a block of data */
  uint32_t CalculateBufferCRC(uint32_t count, uint32_t crc, uint8_t* buffer);
};

}  // namespace gxf
}  // namespace nvidia

#endif  // NVIDIA_GXF_MULTIMEDIA_EXTENSIONS_VIDEOENCODER_OUTPUT_HPP_
