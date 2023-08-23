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

#ifndef HOLOSCAN_CORE_LOGGER_HPP
#define HOLOSCAN_CORE_LOGGER_HPP

#include <fmt/format.h>

#include <chrono>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

#define HOLOSCAN_LOG_LEVEL_TRACE 0
#define HOLOSCAN_LOG_LEVEL_DEBUG 1
#define HOLOSCAN_LOG_LEVEL_INFO 2
#define HOLOSCAN_LOG_LEVEL_WARN 3
#define HOLOSCAN_LOG_LEVEL_ERROR 4
#define HOLOSCAN_LOG_LEVEL_CRITICAL 5
#define HOLOSCAN_LOG_LEVEL_OFF 6

// Please define (or call CMake's `set_compile_definitions` with) HOLOSCAN_LOG_ACTIVE_LEVEL before
// including <holoscan/holoscan.h> to one of the above levels if you want to skip logging at
// a certain level at compile time.
//
// E.g.,
//     #define HOLOSCAN_LOG_ACTIVE_LEVEL 3
//     #include <holoscan/holoscan.h>
//     ...
//
// Then, it will only log at the WARN(3)/ERROR(4)/CRITICAL(5) levels.

// Workaround for zero-arguments
// (https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0306r2.html)
// If __VA_OPT__ is supported (since C++20), we could use it to use compile-time format string check
// : FMT_STRING(format)
// (https://fmt.dev/latest/api.html#compile-time-format-string-checks)
#define HOLOSCAN_LOG_CALL(level, ...) \
  ::holoscan::Logger::log(            \
      __FILE__, __LINE__, static_cast<const char*>(__FUNCTION__), level, __VA_ARGS__)

// clang-format off
#if HOLOSCAN_LOG_ACTIVE_LEVEL <= HOLOSCAN_LOG_LEVEL_TRACE
/**
 * @brief Print a trace message to the log (with file, line, and function name).
 *
 * The format string follows the [fmtlib format string syntax](https://fmt.dev/latest/syntax.html).
 */
#    define HOLOSCAN_LOG_TRACE(...) HOLOSCAN_LOG_CALL(::holoscan::LogLevel::TRACE, __VA_ARGS__)
#else
#    define HOLOSCAN_LOG_TRACE(...) (void)0
#endif

#if HOLOSCAN_LOG_ACTIVE_LEVEL <= HOLOSCAN_LOG_LEVEL_DEBUG
/**
 * @brief Print a debug message to the log (with file, line, and function name).
 *
 * The format string follows the [fmtlib format string syntax](https://fmt.dev/latest/syntax.html).
 */
#    define HOLOSCAN_LOG_DEBUG(...) HOLOSCAN_LOG_CALL(::holoscan::LogLevel::DEBUG, __VA_ARGS__)
#else
#    define HOLOSCAN_LOG_DEBUG(...) (void)0
#endif

#if HOLOSCAN_LOG_ACTIVE_LEVEL <= HOLOSCAN_LOG_LEVEL_INFO
/**
 * @brief Print an info message to the log (with file, line, and function name).
 *
 * The format string follows the [fmtlib format string syntax](https://fmt.dev/latest/syntax.html).
 */
#    define HOLOSCAN_LOG_INFO(...) HOLOSCAN_LOG_CALL(::holoscan::LogLevel::INFO, __VA_ARGS__)
#else
#    define HOLOSCAN_LOG_INFO(...) (void)0
#endif

#if HOLOSCAN_LOG_ACTIVE_LEVEL <= HOLOSCAN_LOG_LEVEL_WARN
/**
 * @brief Print a warning message to the log (with file, line, and function name).
 *
 * The format string follows the [fmtlib format string syntax](https://fmt.dev/latest/syntax.html).
 */
#    define HOLOSCAN_LOG_WARN(...) HOLOSCAN_LOG_CALL(::holoscan::LogLevel::WARN, __VA_ARGS__)
#else
#    define HOLOSCAN_LOG_WARN(...) (void)0
#endif

#if HOLOSCAN_LOG_ACTIVE_LEVEL <= HOLOSCAN_LOG_LEVEL_ERROR
/**
 * @brief Print an error message to the log (with file, line, and function name).
 *
 * The format string follows the [fmtlib format string syntax](https://fmt.dev/latest/syntax.html).
 */
#    define HOLOSCAN_LOG_ERROR(...) HOLOSCAN_LOG_CALL(::holoscan::LogLevel::ERROR, __VA_ARGS__)
#else
#    define HOLOSCAN_LOG_ERROR(...) (void)0
#endif

#if HOLOSCAN_LOG_ACTIVE_LEVEL <= HOLOSCAN_LOG_LEVEL_CRITICAL
/**
 * @brief Print a critical message to the log (with file, line, and function name).
 *
 * The format string follows the [fmtlib format string syntax](https://fmt.dev/latest/syntax.html).
 */
#    define HOLOSCAN_LOG_CRITICAL(...) \
HOLOSCAN_LOG_CALL(::holoscan::LogLevel::CRITICAL, __VA_ARGS__)
#else
#    define HOLOSCAN_LOG_CRITICAL(...) (void)0
#endif
// clang-format on

namespace holoscan {

enum class LogLevel {
  TRACE = 0,     ///< SPDLOG_LEVEL_TRACE
  DEBUG = 1,     ///< SPDLOG_LEVEL_DEBUG
  INFO = 2,      ///< SPDLOG_LEVEL_INFO
  WARN = 3,      ///< SPDLOG_LEVEL_WARN
  ERROR = 4,     ///< SPDLOG_LEVEL_ERROR
  CRITICAL = 5,  ///< SPDLOG_LEVEL_CRITICAL
  OFF = 6,       ///< SPDLOG_LEVEL_OFF
};

/**
 * @brief A logger class that wraps spdlog.
 *
 * Please see the [spdlog documentation](https://spdlog.docsforge.com/v1.x/api/spdlog/) for more
 * details of the API.
 */
class Logger {
 public:
  static void load_env_level();

  static void set_level(LogLevel level);
  static LogLevel level();

  static void set_pattern(std::string pattern);

  static bool should_backtrace();
  static void disable_backtrace();
  static void enable_backtrace(size_t n_messages);
  static void dump_backtrace();

  static void flush();
  static LogLevel flush_level();
  static void flush_on(LogLevel level);

  template <typename FormatT, typename... ArgsT>
  static void log(const char* file, int line, const char* function_name, LogLevel level,
                  const FormatT& format, ArgsT&&... args) {
    log_message(file,
                line,
                function_name,
                level,
                format,
                fmt::make_args_checked<ArgsT...>(format, args...));
  }

  template <typename FormatT, typename... ArgsT>
  static void log(LogLevel level, const FormatT& format, ArgsT&&... args) {
    log_message(level, format, fmt::make_args_checked<ArgsT...>(format, args...));
  }

 private:
  static void log_message(const char* file, int line, const char* function_name, LogLevel level,
                          fmt::string_view format, fmt::format_args args);
  static void log_message(LogLevel level, fmt::string_view format, fmt::format_args args);
};

/**
 * @brief set the log level of the Holoscan Application from the environment variable
 * `HOLOSCAN_LOG_LEVEL`.
 *
 * `HOLOSCAN_LOG_LEVEL` can be set to one of the following values:
 *
 * - TRACE
 * - DEBUG
 * - INFO
 * - WARN
 * - ERROR
 * - CRITICAL
 * - OFF
 *
 * ```bash
 * export HOLOSCAN_LOG_LEVEL=TRACE
 * ```
 */
inline void load_env_log_level() {
  Logger::load_env_level();
}

/**
 * @brief Set global logging level.
 *
 * @param level The new log level.
 */
inline void set_log_level(LogLevel level) {
  Logger::set_level(level);
}

/**
 * @brief Get global logging level.
 *
 * @return The current log level.
 */
inline LogLevel log_level() {
  return Logger::level();
}

/**
 * @brief Set global format string.
 *
 * Please refer to the [spdlog
 * documentation](https://spdlog.docsforge.com/v1.x/3.custom-formatting/#customizing-format-using-set_pattern)
 * for the format string syntax.
 *
 * @param pattern The format string.
 */
inline void set_log_pattern(std::string pattern) {
  // https://spdlog.docsforge.com/v1.x/0.faq/#colors-do-not-appear-when-using-custom-format
  Logger::set_pattern(pattern);
}

/**
 * @brief Print a trace message to the log.
 *
 * The format string follows the [fmtlib format string syntax](https://fmt.dev/latest/syntax.html).
 */
template <typename FormatT, typename... ArgsT>
inline void log_trace(const FormatT& format, ArgsT&&... args) {
  Logger::log(LogLevel::TRACE, format, std::forward<ArgsT>(args)...);
}

/**
 * @brief Print a debug message to the log.
 *
 * The format string follows the [fmtlib format string syntax](https://fmt.dev/latest/syntax.html).
 */
template <typename FormatT, typename... ArgsT>
inline void log_debug(const FormatT& format, ArgsT&&... args) {
  Logger::log(LogLevel::DEBUG, format, std::forward<ArgsT>(args)...);
}

/**
 * @brief Print an info message to the log.
 *
 * The format string follows the [fmtlib format string syntax](https://fmt.dev/latest/syntax.html).
 */
template <typename FormatT, typename... ArgsT>
inline void log_info(const FormatT& format, ArgsT&&... args) {
  Logger::log(LogLevel::INFO, format, std::forward<ArgsT>(args)...);
}

/**
 * @brief Print a warning message to the log.
 *
 * The format string follows the [fmtlib format string syntax](https://fmt.dev/latest/syntax.html).
 */
template <typename FormatT, typename... ArgsT>
inline void log_warn(const FormatT& format, ArgsT&&... args) {
  Logger::log(LogLevel::WARN, format, std::forward<ArgsT>(args)...);
}

/**
 * @brief Print an error message to the log.
 *
 * The format string follows the [fmtlib format string syntax](https://fmt.dev/latest/syntax.html).
 */
template <typename FormatT, typename... ArgsT>
inline void log_error(const FormatT& format, ArgsT&&... args) {
  Logger::log(LogLevel::ERROR, format, std::forward<ArgsT>(args)...);
}

/**
 * @brief Print a critical message to the log.
 *
 * The format string follows the [fmtlib format string syntax](https://fmt.dev/latest/syntax.html).
 */
template <typename FormatT, typename... ArgsT>
inline void log_critical(const FormatT& format, ArgsT&&... args) {
  Logger::log(LogLevel::CRITICAL, format, std::forward<ArgsT>(args)...);
}

}  // namespace holoscan

#endif /* HOLOSCAN_CORE_LOGGER_HPP */
