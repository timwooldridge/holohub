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

#ifndef HOLOSCAN_CORE_EXECUTORS_GXF_GXF_EXECUTOR_HPP
#define HOLOSCAN_CORE_EXECUTORS_GXF_GXF_EXECUTOR_HPP

#include <gxf/core/gxf.h>

#include <functional>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "../../executor.hpp"
#include "../../gxf/gxf_extension_manager.hpp"

namespace holoscan::gxf {

/**
 * @brief Executor for GXF.
 */
class GXFExecutor : public holoscan::Executor {
 public:
  GXFExecutor() = delete;
  explicit GXFExecutor(holoscan::Fragment* app, bool create_gxf_context = true);

  ~GXFExecutor() override;

  /**
   * @brief Initialize the graph and run the graph.
   *
   * This method calls `compose()` to compose the graph, and runs the graph.
   */
  void run(Graph& graph) override;

  /**
   * @brief Set the context.
   *
   * For GXF, GXFExtensionManager(gxf_extension_manager_) is initialized with the context.
   *
   * @param context The context.
   */
  void context(void* context) override;

  // Inherit Executor::context().
  using Executor::context;

  /**
   * @brief Get GXF extension manager.
   *
   * @return The GXF extension manager.
   * @see GXFExtensionManager
   */
  std::shared_ptr<ExtensionManager> extension_manager() override;

  /**
   * @brief Create and setup GXF components for input port.
   *
   * For a given input port specification, create a GXF Receiver component for the port and
   * create a GXF SchedulingTerm component that is corresponding to the Condition of the port.
   *
   * If there is no condition specified for the port, a default condition
   * (MessageAvailableCondition) is created.
   * It currently supports ConditionType::kMessageAvailable and ConditionType::kNone condition
   * types.
   *
   * This function is a static function so that it can be called from other classes without
   * dependency on this class.
   *
   * @param fragment The fragment that this operator belongs to.
   * @param gxf_context The GXF context.
   * @param eid The GXF entity ID.
   * @param io_spec The input port specification.
   * @param bind_port If true, bind the port to the existing GXF Receiver component. Otherwise,
   * create a new GXF Receiver component.
   */
  static void create_input_port(Fragment* fragment, gxf_context_t gxf_context, gxf_uid_t eid,
                                IOSpec* io_spec, bool bind_port = false);

  /**
   * @brief Create and setup GXF components for output port.
   *
   * For a given output port specification, create a GXF Receiver component for the port and
   * create a GXF SchedulingTerm component that is corresponding to the Condition of the port.
   *
   * If there is no condition specified for the port, a default condition
   * (DownstreamMessageAffordableCondition) is created.
   * It currently supports ConditionType::kDownstreamMessageAffordable and ConditionType::kNone
   * condition types.
   *
   * This function is a static function so that it can be called from other classes without
   * dependency on on this class.
   *
   * @param fragment The fragment that this operator belongs to.
   * @param gxf_context The GXF context.
   * @param eid The GXF entity ID.
   * @param io_spec The output port specification.
   * @param bind_port If true, bind the port to the existing GXF Transmitter component. Otherwise,
   * create a new GXF Transmitter component.
   */
  static void create_output_port(Fragment* fragment, gxf_context_t gxf_context, gxf_uid_t eid,
                                 IOSpec* io_spec, bool bind_port = false);

  /**
   * @brief Set the GXF entity ID of the operator initialized by this executor.
   *
   * If this is 0, a new entity is created for the operator.
   * Otherwise, the operator as a codelet will be added to the existing entity specified by this ID.
   * This is useful when initializing operators inside the existing entity.
   * (e.g., when initializing an operator from `holoscan::gxf::OperatorWrapper` class)
   *
   * @param eid The GXF entity ID.
   */
  void op_eid(gxf_uid_t eid) { op_eid_ = eid; }

  /**
   * @brief Set the GXF component ID of the operator initialized by this executor.
   *
   * If this is 0, a new component is created for the operator.
   * This is useful when initializing operators using the existing component inside the existing
   * entity.
   * (e.g., when initializing an operator from `holoscan::gxf::OperatorWrapper` class)
   *
   * @param cid The GXF component ID.
   */
  void op_cid(gxf_uid_t cid) { op_cid_ = cid; }

 protected:
  bool initialize_operator(Operator* op) override;
  bool add_receivers(const std::shared_ptr<Operator>& op, const std::string& receivers_name,
                     std::set<std::string, std::less<>>& input_labels,
                     std::vector<holoscan::IOSpec*>& iospec_vector) override;

 private:
  void register_extensions();
  bool own_gxf_context_ = false;  ///< Whether this executor owns the GXF context.
  gxf_uid_t op_eid_ = 0;          ///< The GXF entity ID of the operator. Create new entity for
                                  ///< initializing a new operator if this is 0.
  gxf_uid_t op_cid_ = 0;  ///< The GXF component ID of the operator. Create new component for
                          ///< initializing a new operator if this is 0.
  std::shared_ptr<GXFExtensionManager> gxf_extension_manager_;  ///< The GXF extension manager.
};

}  // namespace holoscan::gxf

#endif /* HOLOSCAN_CORE_EXECUTORS_GXF_GXF_EXECUTOR_HPP */
