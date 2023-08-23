/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#include <gtest/gtest.h>
#include <cuda.h>
#include <cuda_runtime_api.h>

#include <holoscan/operators/segmentation_postprocessor/segmentation_postprocessor.cuh>

// This test data is generated in python as follows:
//
//  import numpy as np
//  def softmax(x):
//      return np.exp(x) / np.sum(np.exp(x), axis=0)
//  h = 1920 // 100
//  w = 1080 // 100
//  c = 5
//  x = 6 * np.random.rand(h, w, c).astype(np.float32)
//  x = softmax(x).round(3)
//  def print_c_array(data):
//      print(np.array2string(data.flatten(), separator=','))
//  # Print out input data
//  print_c_array(x)
//  # Print out expected output data
//  print_c_array(np.argmax(x, axis=-1))
//
static const float kArgmaxInputData[] = {
    0.003, 0.028, 0.007, 0.144, 0.041, 0.002, 0.089, 0.078, 0.045, 0.022, 0.125, 0.096, 0.008,
    0.008, 0.01,  0.001, 0.158, 0.097, 0.004, 0.003, 0.005, 0.117, 0.006, 0.01,  0.08,  0.133,
    0.001, 0.003, 0.018, 0.018, 0.008, 0.088, 0.033, 0.002, 0.007, 0.012, 0.323, 0.014, 0.012,
    0.004, 0.001, 0.026, 0.004, 0.213, 0.22,  0.018, 0.004, 0.018, 0.037, 0.001, 0.006, 0.065,
    0.006, 0.116, 0.14,  0.002, 0.003, 0.092, 0.002, 0.001, 0.009, 0.005, 0.016, 0.023, 0.017,
    0.002, 0.001, 0.028, 0.004, 0.073, 0.002, 0.007, 0.001, 0.002, 0.137, 0.091, 0.015, 0.113,
    0.037, 0.008, 0.011, 0.18,  0.002, 0.006, 0.033, 0.107, 0.006, 0.039, 0.003, 0.031, 0.233,
    0.151, 0.013, 0.056, 0.017, 0.006, 0.082, 0.155, 0.018, 0.047, 0.083, 0.136, 0.087, 0.007,
    0.02,  0.021, 0.003, 0.006, 0.013, 0.016, 0.071, 0.002, 0.156, 0.033, 0.002, 0.168, 0.031,
    0.011, 0.01,  0.006, 0.009, 0.078, 0.019, 0.002, 0.057, 0.087, 0.111, 0.019, 0.,    0.004,
    0.058, 0.109, 0.005, 0.183, 0.431, 0.012, 0.336, 0.004, 0.136, 0.278, 0.012, 0.023, 0.001,
    0.224, 0.003, 0.027, 0.003, 0.003, 0.005, 0.064, 0.004, 0.005, 0.012, 0.371, 0.031, 0.099,
    0.004, 0.149, 0.007, 0.026, 0.008, 0.009, 0.005, 0.122, 0.002, 0.005, 0.009, 0.021, 0.002,
    0.152, 0.063, 0.075, 0.144, 0.002, 0.085, 0.003, 0.002, 0.004, 0.021, 0.405, 0.114, 0.003,
    0.067, 0.012, 0.014, 0.002, 0.001, 0.012, 0.104, 0.026, 0.024, 0.001, 0.074, 0.003, 0.008,
    0.171, 0.043, 0.001, 0.21,  0.087, 0.007, 0.001, 0.001, 0.005, 0.003, 0.113, 0.257, 0.006,
    0.173, 0.056, 0.012, 0.01,  0.018, 0.04,  0.013, 0.071, 0.173, 0.011, 0.002, 0.044, 0.184,
    0.024, 0.085, 0.021, 0.017, 0.004, 0.013, 0.021, 0.055, 0.019, 0.01,  0.023, 0.013, 0.093,
    0.04,  0.033, 0.004, 0.009, 0.22,  0.032, 0.002, 0.092, 0.024, 0.189, 0.015, 0.032, 0.001,
    0.017, 0.003, 0.001, 0.03,  0.019, 0.005, 0.001, 0.142, 0.006, 0.001, 0.215, 0.001, 0.005,
    0.01,  0.062, 0.09,  0.001, 0.009, 0.01,  0.024, 0.315, 0.266, 0.06,  0.077, 0.036, 0.031,
    0.036, 0.003, 0.028, 0.012, 0.013, 0.138, 0.003, 0.103, 0.001, 0.101, 0.013, 0.107, 0.007,
    0.002, 0.026, 0.012, 0.198, 0.004, 0.005, 0.002, 0.021, 0.018, 0.001, 0.068, 0.014, 0.004,
    0.061, 0.003, 0.089, 0.049, 0.175, 0.082, 0.17,  0.042, 0.008, 0.116, 0.012, 0.005, 0.086,
    0.15,  0.004, 0.237, 0.002, 0.102, 0.035, 0.001, 0.001, 0.152, 0.031, 0.003, 0.092, 0.003,
    0.001, 0.005, 0.01,  0.008, 0.053, 0.004, 0.002, 0.035, 0.265, 0.005, 0.04,  0.055, 0.138,
    0.01,  0.012, 0.216, 0.005, 0.334, 0.007, 0.001, 0.011, 0.061, 0.015, 0.085, 0.004, 0.004,
    0.056, 0.014, 0.001, 0.077, 0.025, 0.006, 0.008, 0.004, 0.018, 0.015, 0.024, 0.315, 0.003,
    0.37,  0.174, 0.008, 0.001, 0.065, 0.002, 0.113, 0.244, 0.015, 0.049, 0.313, 0.002, 0.009,
    0.003, 0.133, 0.094, 0.013, 0.001, 0.312, 0.005, 0.01,  0.006, 0.002, 0.006, 0.078, 0.225,
    0.001, 0.155, 0.009, 0.002, 0.046, 0.014, 0.078, 0.155, 0.281, 0.001, 0.002, 0.045, 0.328,
    0.004, 0.105, 0.044, 0.047, 0.039, 0.003, 0.126, 0.015, 0.006, 0.065, 0.011, 0.034, 0.132,
    0.016, 0.001, 0.298, 0.003, 0.069, 0.01,  0.084, 0.076, 0.003, 0.099, 0.003, 0.178, 0.182,
    0.016, 0.003, 0.01,  0.05,  0.012, 0.009, 0.006, 0.058, 0.057, 0.004, 0.006, 0.002, 0.001,
    0.001, 0.015, 0.068, 0.02,  0.002, 0.014, 0.087, 0.145, 0.017, 0.184, 0.006, 0.017, 0.059,
    0.016, 0.014, 0.003, 0.182, 0.001, 0.009, 0.003, 0.054, 0.004, 0.012, 0.001, 0.005, 0.155,
    0.003, 0.051, 0.007, 0.001, 0.136, 0.004, 0.003, 0.04,  0.09,  0.007, 0.119, 0.004, 0.121,
    0.116, 0.035, 0.028, 0.004, 0.213, 0.002, 0.008, 0.08,  0.002, 0.045, 0.044, 0.014, 0.041,
    0.002, 0.098, 0.031, 0.188, 0.005, 0.001, 0.006, 0.001, 0.208, 0.018, 0.017, 0.106, 0.039,
    0.001, 0.006, 0.017, 0.075, 0.249, 0.005, 0.004, 0.053, 0.158, 0.017, 0.045, 0.013, 0.002,
    0.063, 0.123, 0.108, 0.001, 0.001, 0.183, 0.16,  0.077, 0.001, 0.021, 0.018, 0.138, 0.094,
    0.003, 0.011, 0.051, 0.058, 0.053, 0.161, 0.029, 0.001, 0.071, 0.002, 0.015, 0.004, 0.002,
    0.002, 0.007, 0.009, 0.006, 0.279, 0.002, 0.009, 0.014, 0.02,  0.002, 0.052, 0.002, 0.205,
    0.251, 0.01,  0.004, 0.007, 0.005, 0.013, 0.004, 0.037, 0.004, 0.014, 0.007, 0.007, 0.079,
    0.006, 0.024, 0.16,  0.003, 0.161, 0.006, 0.002, 0.005, 0.113, 0.234, 0.011, 0.004, 0.037,
    0.012, 0.017, 0.003, 0.011, 0.003, 0.165, 0.095, 0.001, 0.001, 0.004, 0.174, 0.015, 0.121,
    0.004, 0.126, 0.004, 0.005, 0.008, 0.001, 0.063, 0.001, 0.032, 0.002, 0.006, 0.054, 0.008,
    0.016, 0.039, 0.175, 0.053, 0.001, 0.002, 0.062, 0.082, 0.119, 0.005, 0.036, 0.147, 0.029,
    0.06,  0.104, 0.013, 0.238, 0.005, 0.001, 0.052, 0.005, 0.005, 0.002, 0.014, 0.09,  0.003,
    0.001, 0.005, 0.008, 0.023, 0.003, 0.065, 0.019, 0.101, 0.001, 0.12,  0.006, 0.076, 0.022,
    0.305, 0.108, 0.028, 0.064, 0.014, 0.193, 0.038, 0.273, 0.011, 0.262, 0.01,  0.001, 0.012,
    0.006, 0.012, 0.047, 0.159, 0.002, 0.001, 0.107, 0.156, 0.116, 0.001, 0.06,  0.009, 0.002,
    0.004, 0.001, 0.07,  0.016, 0.001, 0.001, 0.012, 0.023, 0.011, 0.001, 0.021, 0.004, 0.029,
    0.041, 0.002, 0.002, 0.042, 0.003, 0.018, 0.053, 0.039, 0.163, 0.003, 0.125, 0.041, 0.04,
    0.001, 0.002, 0.042, 0.135, 0.009, 0.026, 0.052, 0.046, 0.091, 0.001, 0.024, 0.001, 0.002,
    0.134, 0.055, 0.12,  0.012, 0.053, 0.002, 0.001, 0.128, 0.044, 0.001, 0.046, 0.002, 0.001,
    0.085, 0.211, 0.032, 0.055, 0.061, 0.095, 0.129, 0.004, 0.027, 0.275, 0.056, 0.008, 0.168,
    0.002, 0.002, 0.002, 0.002, 0.151, 0.002, 0.006, 0.003, 0.005, 0.126, 0.143, 0.144, 0.002,
    0.049, 0.001, 0.003, 0.064, 0.027, 0.001, 0.013, 0.039, 0.009, 0.067, 0.063, 0.074, 0.039,
    0.025, 0.006, 0.002, 0.059, 0.002, 0.002, 0.002, 0.002, 0.006, 0.008, 0.044, 0.001, 0.041,
    0.018, 0.015, 0.031, 0.002, 0.013, 0.039, 0.005, 0.1,   0.071, 0.001, 0.064, 0.272, 0.008,
    0.121, 0.011, 0.001, 0.004, 0.032, 0.137, 0.094, 0.005, 0.003, 0.079, 0.011, 0.031, 0.052,
    0.212, 0.004, 0.118, 0.079, 0.391, 0.275, 0.006, 0.309, 0.003, 0.009, 0.015, 0.002, 0.007,
    0.249, 0.005, 0.002, 0.001, 0.434, 0.022, 0.003, 0.123, 0.083, 0.019, 0.006, 0.211, 0.015,
    0.005, 0.008, 0.021, 0.004, 0.046, 0.002, 0.002, 0.091, 0.014, 0.001, 0.283, 0.002, 0.171,
    0.016, 0.183, 0.024, 0.021, 0.19,  0.044, 0.002, 0.004, 0.001, 0.059, 0.01,  0.03,  0.001,
    0.007, 0.004, 0.083, 0.008, 0.006, 0.013, 0.094, 0.003, 0.049, 0.065, 0.208, 0.056, 0.016,
    0.003, 0.058, 0.109, 0.007, 0.111, 0.021, 0.001, 0.107, 0.018, 0.012, 0.003, 0.004, 0.011,
    0.02,  0.214, 0.021, 0.248, 0.002, 0.002, 0.007, 0.05,  0.002, 0.012, 0.279, 0.004, 0.098,
    0.048, 0.004, 0.003, 0.03,  0.068, 0.003, 0.046, 0.005, 0.001, 0.117, 0.025, 0.021, 0.003,
    0.039, 0.106, 0.015, 0.169, 0.001, 0.002, 0.099, 0.001, 0.003, 0.01,  0.005, 0.015, 0.026,
    0.004, 0.036, 0.055, 0.248, 0.177, 0.001, 0.056, 0.098, 0.002, 0.125, 0.235, 0.082, 0.146,
    0.012, 0.002, 0.005, 0.002, 0.015, 0.002, 0.117, 0.055, 0.011, 0.198, 0.163, 0.014, 0.008,
    0.018};

static const uint8_t kArgmaxOutputData[] = {
    3, 1, 0, 1, 1, 0, 1, 1, 4, 3, 4, 2, 3, 4, 4, 2, 1, 0, 0, 2, 1, 0, 2, 0, 1, 1, 4, 1, 3, 4, 3, 2,
    3, 4, 2, 4, 0, 3, 2, 3, 0, 1, 3, 1, 0, 3, 3, 3, 3, 0, 4, 2, 2, 2, 0, 3, 4, 4, 3, 1, 3, 0, 4, 1,
    0, 4, 3, 2, 2, 3, 4, 0, 4, 0, 4, 3, 2, 4, 1, 3, 2, 4, 2, 3, 2, 3, 2, 1, 4, 4, 1, 3, 2, 2, 2, 3,
    0, 0, 0, 2, 2, 0, 1, 0, 1, 0, 1, 3, 1, 3, 0, 4, 4, 1, 4, 1, 1, 1, 0, 0, 4, 4, 3, 4, 2, 2, 0, 0,
    4, 1, 0, 2, 2, 1, 0, 3, 3, 4, 2, 2, 4, 0, 0, 0, 2, 4, 4, 2, 0, 0, 2, 2, 3, 0, 0, 2, 2, 2, 1, 3,
    2, 1, 0, 4, 3, 1, 0, 4, 2, 4, 4, 1, 4, 3, 3, 0, 4, 2, 4, 1, 1, 1, 3, 1, 4, 1, 3, 0, 2, 0};

TEST(SegmentationPostprocessor, Argmax) {
  holoscan::ops::segmentation_postprocessor::Shape shape;
  shape.height = 19;
  shape.width = 10;
  shape.channels = 5;

  const uint32_t input_data_size = sizeof(kArgmaxInputData);
  const uint32_t output_data_size = sizeof(kArgmaxOutputData);

  ASSERT_EQ(input_data_size, shape.height * shape.width * shape.channels * sizeof(float));
  ASSERT_EQ(output_data_size,
            shape.height * shape.width *
                sizeof(holoscan::ops::segmentation_postprocessor::output_type_t));

  holoscan::ops::segmentation_postprocessor::output_type_t host_output_data[output_data_size] = {};

  float* device_input_data = nullptr;
  holoscan::ops::segmentation_postprocessor::output_type_t* device_output_data = nullptr;
  ASSERT_EQ(cudaSuccess, cudaMalloc(reinterpret_cast<void**>(&device_input_data), input_data_size));
  ASSERT_EQ(cudaSuccess,
            cudaMalloc(reinterpret_cast<void**>(&device_output_data), output_data_size));

  ASSERT_EQ(
      cudaSuccess,
      cudaMemcpy(device_input_data, kArgmaxInputData, input_data_size, cudaMemcpyHostToDevice));
  ASSERT_EQ(cudaSuccess, cudaMemset(device_output_data, 0, output_data_size));

  holoscan::ops::segmentation_postprocessor::cuda_postprocess(
      holoscan::ops::segmentation_postprocessor::NetworkOutputType::kSoftmax,
      holoscan::ops::segmentation_postprocessor::DataFormat::kHWC,
      shape,
      device_input_data,
      device_output_data);

  ASSERT_EQ(
      cudaSuccess,
      cudaMemcpy(host_output_data, device_output_data, output_data_size, cudaMemcpyDeviceToHost));

  ASSERT_EQ(cudaSuccess, cudaFree(device_input_data));
  ASSERT_EQ(cudaSuccess, cudaFree(device_output_data));

  for (uint32_t i = 0; i < output_data_size / sizeof(host_output_data[0]); i++) {
    ASSERT_EQ(kArgmaxOutputData[i], host_output_data[i]) << "Failed at index: " << i;
  }
}
