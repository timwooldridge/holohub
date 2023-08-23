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

#ifndef NVIDIA_GXF_MULTIMEDIA_EXTENSIONS_VIDEOENCODER_INPUT_HPP_
#define NVIDIA_GXF_MULTIMEDIA_EXTENSIONS_VIDEOENCODER_INPUT_HPP_

#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

#include "gxf/multimedia/video.hpp"
#include "gxf/std/codelet.hpp"
#include "gxf/std/receiver.hpp"
#include "gxf/std/tensor.hpp"
#include "gxf/std/transmitter.hpp"

namespace nvidia {
namespace gxf {

template <gxf::VideoFormat T> struct DefaultNoPaddingColorPlanes {};

template <>
struct DefaultNoPaddingColorPlanes<gxf::VideoFormat::GXF_VIDEO_FORMAT_BGR> {
  DefaultNoPaddingColorPlanes(size_t width)
      : planes({gxf::ColorPlane("RGB", 3, width * 3)}) {}
  std::array<gxf::ColorPlane, 1> planes;
};

template <>
struct DefaultNoPaddingColorPlanes<gxf::VideoFormat::GXF_VIDEO_FORMAT_GRAY> {
  DefaultNoPaddingColorPlanes(size_t width)
      : planes({gxf::ColorPlane("gray", 1, width)}) {}
  std::array<gxf::ColorPlane, 1> planes;
};

template <>
struct DefaultNoPaddingColorPlanes<gxf::VideoFormat::GXF_VIDEO_FORMAT_NV24> {
  DefaultNoPaddingColorPlanes(uint32_t width)
      : planes({nvidia::gxf::ColorPlane("Y", 1, width),
                nvidia::gxf::ColorPlane("UV", 2, width * 2)}) {}
  std::array<nvidia::gxf::ColorPlane, 2> planes;
};

template <>
struct DefaultNoPaddingColorPlanes<gxf::VideoFormat::GXF_VIDEO_FORMAT_NV12> {
  DefaultNoPaddingColorPlanes(size_t width)
      : planes({gxf::ColorPlane("Y", 1, width),
                gxf::ColorPlane("UV", 2, width)}) {}
  std::array<gxf::ColorPlane, 2> planes;
};

template <>
struct DefaultNoPaddingColorPlanes<gxf::VideoFormat::GXF_VIDEO_FORMAT_YUV420> {
  DefaultNoPaddingColorPlanes(size_t width)
      : planes({gxf::ColorPlane("Y", 1, width),
                gxf::ColorPlane("U", 1, width/2),
                gxf::ColorPlane("V", 1, width/2)}) {}
  std::array<gxf::ColorPlane, 3> planes;
};

template <gxf::VideoFormat T>
gxf_result_t AllocateVideoBuffer(gxf::Handle<gxf::VideoBuffer> video_buffer,
                                 size_t width, size_t height,
                                 gxf::Handle<gxf::Allocator> allocator,
                                 bool is_cpu = true) {
  if (width % 2 != 0 || height % 2 != 0) {
    GXF_LOG_ERROR(
        "image width/height must be even for creation of gxf::VideoBuffer");
    return GXF_FAILURE;
  }
  DefaultNoPaddingColorPlanes<T> planes_trait(width);
  gxf::VideoFormatSize<T> buffer_type_trait;
  uint64_t size = buffer_type_trait.size(width, height, planes_trait.planes);
  std::vector<gxf::ColorPlane> planes_filled{planes_trait.planes.begin(),
                                             planes_trait.planes.end()};
  gxf::VideoBufferInfo buffer_info{
      static_cast<uint32_t>(width), static_cast<uint32_t>(height), T,
      planes_filled, gxf::SurfaceLayout::GXF_SURFACE_LAYOUT_PITCH_LINEAR};
  auto result = video_buffer->resizeCustom(
      buffer_info, size,
      is_cpu ? gxf::MemoryStorageType::kHost : gxf::MemoryStorageType::kDevice,
      allocator);

  if (!result) {
    GXF_LOG_ERROR("reshape tensor failed.");
    return GXF_FAILURE;
  }
  return GXF_SUCCESS;
}

// Video Reading Codelet.
class VideoReadYUV : public gxf::Codelet {
 public:
  VideoReadYUV() = default;
  ~VideoReadYUV() = default;

  gxf_result_t registerInterface(gxf::Registrar* registrar) override;
  gxf_result_t initialize() override { return GXF_SUCCESS; }
  gxf_result_t deinitialize() override { return GXF_SUCCESS; }

  gxf_result_t start() override;
  gxf_result_t tick() override;
  gxf_result_t stop() override;

 private:
  // The path to read the video from
  gxf::Parameter<std::string> input_video_path_;
  // Data allocator to create a tensor
  gxf::Parameter<gxf::Handle<gxf::Allocator>> pool_;
  // Data transmitter to send the data
  gxf::Parameter<gxf::Handle<gxf::Transmitter>> data_transmitter_;
  // storage type for output buffer
  gxf::Parameter<uint32_t> outbuf_storage_type_;
  // File stream
  std::FILE* file_;
  /// The width of the input video
  gxf::Parameter<int> frame_width_;
  // The height of the input video
  gxf::Parameter<int> frame_height_;
  // Input video format
  gxf::Parameter<std::string> input_format_;
  // host meory pointers for file read
  unsigned char* yuv_frame_;
  unsigned char* yuv_dptr_;
};

}  // namespace gxf
}  // namespace nvidia

#endif  // NVIDIA_GXF_MULTIMEDIA_EXTENSIONS_VIDEOENCODER_INPUT_HPP_
