/* Copyright (c) 2016-2016, Vasil Dimov

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#ifndef IPFS_TEST_UTILS_H
#define IPFS_TEST_UTILS_H

#include <functional>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <ipfs/client.h>

namespace ipfs {

namespace test {

/** Check if a given set of properties exist in a JSON. */
inline void check_if_properties_exist(
    /** [in] Label to use when throwing an exception if a failure occurs. */
    const std::string& label,
    /** [in] JSON to check for the properties. */
    const ipfs::Json& j,
    /** [in] List of properties. */
    const std::vector<const char*>& properties) {
  for (const char* property : properties) {
    if (j.find(property) == j.end()) {
      throw std::runtime_error(label + ": the property \"" + property +
                               "\" was not found in the response:\n" +
                               j.dump(2));
    }
  }
}

/** Check if a string contains another string and throw an exception if it does
 * not. */
inline void check_if_string_contains(
    /** [in] Label to use when throwing an exception if a failure occurs. */
    const std::string& label,
    /** [in] String to search into. */
    const std::string& big,
    /** [in] String to search for. */
    const std::string& needle) {
  if (big.find(needle) == big.npos) {
    throw std::runtime_error(label + ": \"" + needle +
                             "\" was not found in the response:\n" + big);
  }
}

/** Convert a string to hex. For example: "abcd" -> "61626364". */
inline std::string string_to_hex(
    /** [in] String to convert. */
    const std::string& input) {
  std::stringstream ss;
  ss << std::hex;
  for (size_t i = 0; i < input.length(); ++i) {
    ss << std::setw(2) << std::setfill('0') << static_cast<int>(input[i]);
  }
  return ss.str();
}

/** Execute a function and expect it to throw an exception. */
inline void must_fail(
    /** [in] Label to prefix in diagnostics. */
    const std::string& label,
    /** [in,out] Function to execute, must throw an exception. */
    std::function<void()> f) {
  bool succeeded;

  try {
    f();
    succeeded = true;
  } catch (const std::exception& e) {
    std::cout << label + " failed as expected with error message: " << e.what()
              << std::endl;
    succeeded = false;
  }

  if (succeeded) {
    throw std::runtime_error(label + " succeeded but should have failed.");
  }
}

} /* namespace test */
} /* namespace ipfs */
#endif /* IPFS_TEST_UTILS_H */
