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

#ifndef HOLOSCAN_CORE_FRAGMENT_HPP
#define HOLOSCAN_CORE_FRAGMENT_HPP

#include <iostream>     // for std::cout
#include <memory>       // for std::shared_ptr
#include <set>          // for std::set
#include <string>       // for std::string
#include <type_traits>  // for std::enable_if_t, std::is_constructible
#include <utility>      // for std::pair

#include "common.hpp"
#include "config.hpp"
#include "executor.hpp"
#include "graph.hpp"

namespace holoscan {

/**
 * @brief The fragment of the application.
 *
 * A fragment is a building block of the Application. It is a Directed Acyclic Graph (DAG) of
 * operators. A fragment can be assigned to a physical node of a Holoscan cluster during execution.
 * The run-time execution manages communication across fragments. In a Fragment, Operators (Graph
 * Nodes) are connected to each other by flows (Graph Edges).
 */
class Fragment {
 public:
  Fragment() = default;
  virtual ~Fragment() = default;

  Fragment(Fragment&&) = default;

  Fragment& operator=(Fragment&&) = default;

  /**
   * @brief Set the name of the operator.
   *
   * @param name The name of the operator.
   * @return The reference to this operator.
   */
  Fragment& name(const std::string& name) &;

  /**
   * @brief Set the name of the operator.
   *
   * @param name The name of the operator.
   * @return The reference to this operator.
   */
  Fragment&& name(const std::string& name) &&;

  /**
   * @brief Get the name of the fragment.
   *
   * @return The name of the fragment.
   */
  const std::string& name() const;

  /**
   * @brief Set the application of the fragment.
   *
   * @param app The pointer to the application of the fragment.
   * @return The reference to this fragment.
   */
  Fragment& application(Application* app);

  /**
   * @brief Set the configuration of the fragment.
   *
   * The configuration file is a YAML file that has the information of GXF extension paths and some
   * parameter values for operators.
   *
   * The `extensions` field in the YAML configuration file is a list of GXF extension paths.
   * The paths can be absolute or relative to the current working directory, considering paths in
   * `LD_LIBRARY_PATH` environment variable.
   *
   * The paths can consist of the following parts:
   *
   * - GXF core extensions
   *   - built-in extensions such as `libgxf_std.so` and `libgxf_cuda.so`.
   *   - `libgxf_std.so`, `libgxf_cuda.so`, `libgxf_multimedia.so`, `libgxf_serialization.so` are
   *     always loaded by default.
   *   - GXF core extensions are copied to the `lib` directory of the build/installation directory.
   * - Other GXF extensions
   *   - GXF extensions that are required for operators that this fragment uses.
   *   - some core GXF extensions such as `libgxf_stream_playback.so` are always loaded by default.
   *   - these paths are usually relative to the build/installation directory.
   *
   * The extension paths are used to load dependent GXF extensions at runtime when
   * `::run()` method is called.
   *
   * For other fields in the YAML file, you can freely define the parameter values for
   * operators/fragments.
   *
   * For example:
   *
   * ```yaml
   * extensions:
   *   - libmy_recorder.so
   *
   * replayer:
   *   directory: "../data/endoscopy/video"
   *   basename: "surgical_video"
   *   frame_rate: 0   # as specified in timestamps
   *   repeat: false   # default: false
   *   realtime: true  # default: true
   *   count: 0        # default: 0 (no frame count restriction)
   *
   * recorder:
   *   out_directory: "/tmp"
   *   basename: "tensor_out"
   * ```
   *
   * You can get the value of this configuration file by calling `::from_config()` method.
   *
   * @param config_file The path to the configuration file.
   * @param prefix The prefix string that is prepended to the key of the configuration. (not
   * implemented yet)
   */
  void config(const std::string& config_file, const std::string& prefix = "");

  /**
   * @brief Get the configuration of the fragment.
   *
   * @return The reference to the configuration of the fragment (`Config` object.)
   */
  Config& config();

  /**
   * @brief Get the graph of the fragment.
   *
   * @return The reference to the graph of the fragment (`Graph` object.)
   */
  Graph& graph();

  /**
   * @brief Get the executor of the fragment.
   *
   * @return The reference to the executor of the fragment (`Executor` object.)
   */
  Executor& executor();

  /**
   * @brief Get the Argument(s) from the configuration file.
   *
   * For the given key, this method returns the value of the configuration file.
   *
   * For example:
   *
   * ```yaml
   * source: "replayer"
   * do_record: false   # or 'true' if you want to record input video stream.
   *
   * aja:
   *   width: 1920
   *   height: 1080
   *   rdma: true
   * ```
   *
   * `from_config("aja")` returns an ArgList (vector-like) object that contains the following
   * items:
   *
   * - `Arg("width") = 1920`
   * - `Arg("height") = 1080`
   * - `Arg("rdma") = true`
   *
   * You can use '.' (dot) to access nested fields.
   *
   * `from_config("aja.rdma")` returns an ArgList object that contains only one item and it can be
   * converted to `bool` through `ArgList::as()` method:
   *
   * ```cpp
   * bool is_rdma = from_config("aja.rdma").as<bool>();
   * ```
   *
   * @param key The key of the configuration.
   * @return The argument list of the configuration for the key.
   */
  ArgList from_config(const std::string& key);

  /**
   * @brief Create a new operator.
   *
   * @tparam OperatorT The type of the operator.
   * @param name The name of the operator.
   * @param args The arguments for the operator.
   * @return The shared pointer to the operator.
   */
  template <typename OperatorT, typename StringT, typename... ArgsT,
            typename = std::enable_if_t<std::is_constructible_v<std::string, StringT>>>
  std::shared_ptr<OperatorT> make_operator(const StringT& name, ArgsT&&... args) {
    HOLOSCAN_LOG_DEBUG("Creating operator '{}'", name);
    auto op = std::make_shared<OperatorT>(std::forward<ArgsT>(args)...);
    op->name(name);
    op->fragment(this);
    auto spec = std::make_shared<OperatorSpec>(this);
    op->setup(*spec.get());
    op->spec(spec);

    op->initialize();

    return op;
  }
  /**
   * @brief Create a new operator.
   *
   * @tparam OperatorT The type of the operator.
   * @param args The arguments for the operator.
   * @return The shared pointer to the operator.
   */
  template <typename OperatorT, typename... ArgsT>
  std::shared_ptr<OperatorT> make_operator(ArgsT&&... args) {
    HOLOSCAN_LOG_DEBUG("Creating operator");
    auto op = make_operator<OperatorT>("", std::forward<ArgsT>(args)...);
    return op;
  }

  /**
   * @brief Create a new (operator) resource.
   *
   * @tparam ResourceT The type of the resource.
   * @param name The name of the resource.
   * @param args The arguments for the resource.
   * @return The shared pointer to the resource.
   */
  template <typename ResourceT, typename StringT, typename... ArgsT,
            typename = std::enable_if_t<std::is_constructible_v<std::string, StringT>>>
  std::shared_ptr<ResourceT> make_resource(const StringT& name, ArgsT&&... args) {
    HOLOSCAN_LOG_DEBUG("Creating resource '{}'", name);
    auto resource = std::make_shared<ResourceT>(std::forward<ArgsT>(args)...);
    resource->name(name);
    resource->fragment(this);
    auto spec = std::make_shared<ComponentSpec>(this);
    resource->setup(*spec.get());
    resource->spec(spec);

    resource->initialize();

    return resource;
  }
  /**
   * @brief Create a new (operator) resource.
   *
   * @tparam ResourceT The type of the resource.
   * @param args The arguments for the resource.
   * @return The shared pointer to the resource.
   */
  template <typename ResourceT, typename... ArgsT>
  std::shared_ptr<ResourceT> make_resource(ArgsT&&... args) {
    HOLOSCAN_LOG_DEBUG("Creating resource");
    auto resource = make_resource<ResourceT>("", std::forward<ArgsT>(args)...);
    return resource;
  }

  /**
   * @brief Create a new condition.
   *
   * @tparam ConditionT The type of the condition.
   * @param name The name of the condition.
   * @param args The arguments for the condition.
   * @return The shared pointer to the condition.
   */
  template <typename ConditionT, typename StringT, typename... ArgsT,
            typename = std::enable_if_t<std::is_constructible_v<std::string, StringT>>>
  std::shared_ptr<ConditionT> make_condition(const StringT& name, ArgsT&&... args) {
    HOLOSCAN_LOG_DEBUG("Creating condition '{}'", name);
    auto condition = std::make_shared<ConditionT>(std::forward<ArgsT>(args)...);
    condition->name(name);
    condition->fragment(this);
    auto spec = std::make_shared<ComponentSpec>(this);
    condition->setup(*spec.get());
    condition->spec(spec);

    // Skip initialization. `condition->initialize()` is done in GXFOperator::initialize()

    return condition;
  }

  /**
   * @brief Create a new condition.
   *
   * @tparam ConditionT The type of the condition.
   * @param args The arguments for the condition.
   * @return The shared pointer to the condition.
   */
  template <typename ConditionT, typename... ArgsT>
  std::shared_ptr<ConditionT> make_condition(ArgsT&&... args) {
    HOLOSCAN_LOG_DEBUG("Creating condition");
    auto condition = make_condition<ConditionT>("", std::forward<ArgsT>(args)...);
    return condition;
  }

  /**
   * @brief Add an operator to the graph.
   *
   * The information of the operator is stored in the Graph object.
   * If the operator is already added, this method does nothing.
   *
   * @param op The operator to be added.
   */
  virtual void add_operator(const std::shared_ptr<Operator>& op);

  /**
   * @brief Add a flow between two operators.
   *
   * An output port of the upstream operator is connected to an input port of the
   * downstream operator.
   * The information about the flow (edge) is stored in the Graph object.
   *
   * If the upstream operator or the downstream operator is not in the graph, it will be added to
   * the graph.
   *
   * If there are multiple output ports in the upstream operator or multiple input ports in the
   * downstream operator, it shows an error message.
   *
   * @param upstream_op The upstream operator.
   * @param downstream_op The downstream operator.
   */
  virtual void add_flow(const std::shared_ptr<Operator>& upstream_op,
                        const std::shared_ptr<Operator>& downstream_op);

  /**
   * @brief Add a flow between two operators.
   *
   * An output port of the upstream operator is connected to an input port of the
   * downstream operator.
   * The information about the flow (edge) is stored in the Graph object.
   *
   * If the upstream operator or the downstream operator is not in the graph, it will be added to
   * the graph.
   *
   * In `port_pairs`, an empty port name ("") can be used for specifying a port name if the operator
   * has only one input/output port.
   *
   * If a non-existent port name is specified in `port_pairs`, it first checks if there is a
   * parameter with the same name but with a type of `std::vector<holoscan::IOSpec*>` in the
   * downstream operator.
   * If there is such a parameter (e.g., `receivers`), it creates a new input port with a specific
   * label (`<parameter name>:<index>`. e.g., `receivers:0`), otherwise it shows an error message.
   *
   * For example, if a parameter `receivers` want to have an arbitrary number of receivers,
   *
   *     class HolovizOp : public holoscan::ops::GXFOperator {
   *         ...
   *         private:
   *           Parameter<std::vector<holoscan::IOSpec*>> receivers_;
   *         ...
   *
   * Instead of creating a fixed number of input ports (e.g., `source_video` and `tensor`) and
   * assigning them to the parameter (`receivers`):
   *
   *     void HolovizOp::setup(OperatorSpec& spec) {
   *       ...
   *
   *       auto& in_source_video = spec.input<holoscan::gxf::Entity>("source_video");
   *       auto& in_tensor = spec.input<holoscan::gxf::Entity>("tensor");
   *
   *       spec.param(receivers_,
   *                  "receivers",
   *                  "Input Receivers",
   *                  "List of input receivers.",
   *                  {&in_source_video, &in_tensor});
   *       ...
   *
   * You can skip the creation of input ports and assign them to the parameter (`receivers`) as
   * follows:
   *
   *     void HolovizOp::setup(OperatorSpec& spec) {
   *       ...
   *       spec.param(receivers_,
   *                  "receivers",
   *                  "Input Receivers",
   *                  "List of input receivers.",
   *                  {&in_source_video, &in_tensor});
   *       ...
   *
   * This makes the following code possible in the Application's `compose()` method:
   *
   *     add_flow(source, visualizer_format_converter);
   *     add_flow(visualizer_format_converter, visualizer, {{"", "receivers"}});
   *
   *     add_flow(source, format_converter);
   *     add_flow(format_converter, multiai_inference);
   *     add_flow(multiai_inference, visualizer, {{"", "receivers"}});
   *
   * Instead of:
   *
   *     add_flow(source, visualizer_format_converter);
   *     add_flow(visualizer_format_converter, visualizer, {{"", "source_video"}});
   *
   *     add_flow(source, format_converter);
   *     add_flow(format_converter, multiai_inference);
   *     add_flow(multiai_inference, visualizer, {{"", "tensor"}});
   *
   * By using the parameter (`receivers`) with `std::vector<holoscan::IOSpec*>` type, the framework
   * creates input ports (`receivers:0` and `receivers:1`) implicitly and connects them (and adds
   * the references of the input ports to the `receivers` vector).
   *
   * @param upstream_op The upstream operator.
   * @param downstream_op The downstream operator.
   * @param port_pairs The port pairs. The first element of the pair is the port of the upstream
   * operator and the second element is the port of the downstream operator.
   */
  virtual void add_flow(const std::shared_ptr<Operator>& upstream_op,
                        const std::shared_ptr<Operator>& downstream_op,
                        std::set<std::pair<std::string, std::string>> port_pairs);

  /**
   * @brief Compose a graph.
   *
   * The graph is composed by adding operators and flows in this method.
   */
  virtual void compose();

  /**
   * @brief Initialize the graph and run the graph.
   *
   * This method calls `compose()` to compose the graph, and runs the graph.
   */
  virtual void run();

 protected:
  template <typename ConfigT, typename... ArgsT>
  std::unique_ptr<Config> make_config(ArgsT&&... args) {
    return std::make_unique<ConfigT>(std::forward<ArgsT>(args)...);
  }

  template <typename GraphT>
  std::unique_ptr<Graph> make_graph() {
    return std::make_unique<GraphT>();
  }

  template <typename ExecutorT>
  std::unique_ptr<Executor> make_executor() {
    return std::make_unique<ExecutorT>(this);
  }

  template <typename ExecutorT, typename... ArgsT>
  std::unique_ptr<Executor> make_executor(ArgsT&&... args) {
    return std::make_unique<ExecutorT>(std::forward<ArgsT>(args)...);
  }

  std::string name_;                    ///< The name of the fragment.
  Application* app_ = nullptr;          ///< The application that this fragment belongs to.
  std::unique_ptr<Config> config_;      ///< The configuration of the fragment.
  std::unique_ptr<Graph> graph_;        ///< The graph of the fragment.
  std::unique_ptr<Executor> executor_;  ///< The executor for the fragment.
};

}  // namespace holoscan

#endif /* HOLOSCAN_CORE_FRAGMENT_HPP */
