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

#include <iostream>
#include <stdexcept>
#include <string>

#include <ipfs/client.h>

#ifdef NDEBUG
#error This file only makes sense in debug mode, dont try to compile it in non-debug.
#endif /* NDEBUG */

namespace ipfs {
namespace http {
extern std::string replace_body;
}
}

static void must_fail(const std::string& label, std::function<void()> f) {
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

int main(int, char**) {
  try {
    ipfs::Client client("localhost", 5001);

    std::string object_id;
    client.ObjectNew(&object_id);

    must_fail("client.PinAdd()", [&client, &object_id]() {
      ipfs::http::replace_body = R"({"Pins": []})";
      client.PinAdd(object_id);
    });

    must_fail("client.PinAdd()", [&client, &object_id]() {
      ipfs::http::replace_body = R"({"no Pins property here": []})";
      client.PinAdd(object_id);
    });

    must_fail("client.Id()", [&client]() {
      ipfs::http::replace_body = R"(not a JSON ~!*(@&(~{] indeed)";
      ipfs::Json id;
      client.Id(&id);
    });

  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
