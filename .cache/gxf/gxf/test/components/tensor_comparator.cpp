/*
 * SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gxf/test/components/tensor_comparator.hpp"

#include <cuda_runtime.h>
#include <cstring>
#include <vector>

#include "gxf/std/tensor.hpp"
#include "gxf/std/timestamp.hpp"

namespace nvidia {
namespace gxf {
namespace test {

gxf_result_t TensorComparator::registerInterface(Registrar* registrar) {
  Expected<void> result;
  result &= registrar->parameter(expected_, "expected");
  result &= registrar->parameter(actual_, "actual");
  result &= registrar->parameter(compare_timestamp_, "compare_timestamp",
    "Timestamps components comparison: PubtimeOnly, AcqtimeOnly, PubtimeAndAcqtime, None",
    "Timestamps components comparison: PubtimeOnly, AcqtimeOnly, PubtimeAndAcqtime, None",
    CompareTimestamp::kPubtimeAndAcqtime);
  return ToResultCode(result);
}

gxf_result_t TensorComparator::tick() {
  auto expected = expected_->receive();
  if (!expected) {
    return ToResultCode(expected);
  }
  auto expected_timestamps = expected->findAll<Timestamp>();
  GXF_ASSERT_TRUE(expected_timestamps.has_value());
  auto expected_tensors = expected->findAll<Tensor>();
  GXF_ASSERT_TRUE(expected_tensors.has_value());

  auto actual = actual_->receive();
  if (!actual) {
    return ToResultCode(actual);
  }
  auto actual_timestamps = actual->findAll<Timestamp>();
  GXF_ASSERT_TRUE(actual_timestamps.has_value());
  auto actual_tensors = actual->findAll<Tensor>();
  GXF_ASSERT_TRUE(actual_tensors.has_value());

  if (compare_timestamp_.get() != CompareTimestamp::kNone) {
    GXF_ASSERT_EQ(actual_timestamps->size(), expected_timestamps->size());
    for (size_t i = 0; i < expected_timestamps->size(); i++) {
      GXF_ASSERT_EQ(std::strcmp(actual_timestamps->at(i)->name(),
                                expected_timestamps->at(i)->name()), 0);

      if (compare_timestamp_.get() == CompareTimestamp::kPubtimeAndAcqtime ||
          compare_timestamp_.get() == CompareTimestamp::kPubtimeOnly) {
        GXF_ASSERT_EQ(actual_timestamps->at(i).value()->pubtime,
                      expected_timestamps->at(i).value()->pubtime);
      }
      if (compare_timestamp_.get() == CompareTimestamp::kPubtimeAndAcqtime ||
          compare_timestamp_.get() == CompareTimestamp::kAcqtimeOnly) {
        GXF_ASSERT_EQ(actual_timestamps->at(i).value()->acqtime,
                      expected_timestamps->at(i).value()->acqtime);
      }
    }
  }

  std::vector<byte> expected_buffer;
  std::vector<byte> actual_buffer;

  GXF_ASSERT_EQ(actual_tensors->size(), expected_tensors->size());
  for (size_t i = 0; i < expected_tensors->size(); i++) {
    const auto& expected_tensor = expected_tensors->at(i).value();
    const auto& actual_tensor = actual_tensors->at(i).value();
    GXF_ASSERT_EQ(std::strcmp(actual_tensor.name(), expected_tensor.name()), 0);
    GXF_ASSERT_TRUE(actual_tensor->storage_type() == expected_tensor->storage_type());
    GXF_ASSERT_TRUE(actual_tensor->element_type() == expected_tensor->element_type());
    GXF_ASSERT_EQ(actual_tensor->bytes_per_element(), expected_tensor->bytes_per_element());
    GXF_ASSERT_TRUE(actual_tensor->shape() == expected_tensor->shape());
    switch (expected_tensor->storage_type()) {
      case MemoryStorageType::kHost:
      case MemoryStorageType::kSystem:
        GXF_ASSERT_EQ(std::memcmp(actual_tensor->pointer(), expected_tensor->pointer(),
            expected_tensor->size()), 0);
        break;
      case MemoryStorageType::kDevice:
        expected_buffer.reserve(expected_tensor->size());
        GXF_ASSERT_EQ(cudaMemcpy(expected_buffer.data(), expected_tensor->pointer(),
            expected_tensor->size(), cudaMemcpyDeviceToHost), cudaSuccess);
        actual_buffer.reserve(actual_tensor->size());
        GXF_ASSERT_EQ(cudaMemcpy(actual_buffer.data(), actual_tensor->pointer(),
            actual_tensor->size(), cudaMemcpyDeviceToHost), cudaSuccess);
        GXF_ASSERT_EQ(std::memcmp(actual_buffer.data(), expected_buffer.data(),
            expected_tensor->size()), 0);
        break;
      default:
        return GXF_FAILURE;
    }
  }

  return GXF_SUCCESS;
}

}  // namespace test
}  // namespace gxf
}  // namespace nvidia
