/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <string>

namespace srslog {

namespace detail {

/// This is a lightweight error class that encapsulates a string for error
/// reporting.
class error_string
{
  std::string error;

public:
  error_string() = default;

  /*implicit*/ error_string(std::string error) : error(std::move(error)) {}
  /*implicit*/ error_string(const char* error) : error(error) {}

  /// Returns the error string.
  const std::string& get_error() const { return error; }

  explicit operator bool() const { return !error.empty(); }
};

} // namespace detail

} // namespace srslog
