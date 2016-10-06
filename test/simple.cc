/*
Copyright (c) 2016-2016, Vasil Dimov

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
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <ipfs/api.h>

static inline void check_if_properties_exist(
    const std::string& label, const ipfs::Json& j,
    const std::vector<const char*>& properties) {
  for (const char* property : properties) {
    if (j.find(property) == j.end()) {
      throw std::runtime_error(label + ": the property \"" + property +
                               "\" was not found in the response:\n" +
                               j.dump(2));
    }
  }
}

static inline void check_if_string_contains(const std::string& label,
                                            const std::string& big,
                                            const std::string& needle) {
  if (big.find(needle) == big.npos) {
    throw std::runtime_error(label + ": \"" + needle +
                             "\" was not found in the response:\n" + big);
  }
}

int main(int, char**) {
  try {
    ipfs::Ipfs ipfs = ipfs::Ipfs("localhost", 5001, ipfs::Protocol::kHttp);

    ipfs::Json response_json;

    ipfs.Id(&response_json);
    check_if_properties_exist("ipfs.Id()", response_json,
                              {"Addresses", "ID", "PublicKey"});

    ipfs.Version(&response_json);
    check_if_properties_exist("ipfs.Version()", response_json,
                              {"Repo", "System", "Version"});

    std::stringstream response_stream;
    ipfs.Get("/ipfs/QmYwAPJzv5CZsnA625s3Xf2nemtYgPpHdWEz79ojWnPbdG/readme",
             &response_stream);
    check_if_string_contains("ipfs.Get()", response_stream.str(),
                             "Hello and Welcome to IPFS!");

  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
