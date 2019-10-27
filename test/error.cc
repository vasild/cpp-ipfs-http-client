/* Copyright (c) 2016-2019, The C++ IPFS client library developers

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
#include <ipfs/http/transport-curl.h>
#include <ipfs/test/utils.h>

#ifdef NDEBUG
#error This file only makes sense in debug mode, dont try to compile it in non-debug.
#endif /* NDEBUG */

namespace ipfs {
namespace http {
extern std::string replace_body;
}
}

int main(int, char**) {
  try {
    ipfs::Client client_cant_connect("localhost", 57);

    ipfs::test::must_fail("client.Version()", [&client_cant_connect]() {
      ipfs::Json version;
      client_cant_connect.Version(&version);
    });

    ipfs::Client client("localhost", 5001);

    std::string object_id;
    client.ObjectNew(&object_id);

    ipfs::test::must_fail("client.PinAdd()", [&client, &object_id]() {
      ipfs::http::replace_body = R"({"Pins": []})";
      client.PinAdd(object_id);
    });

    ipfs::test::must_fail("client.PinAdd()", [&client, &object_id]() {
      ipfs::http::replace_body = R"({"no Pins property here": []})";
      client.PinAdd(object_id);
    });

    ipfs::test::must_fail("client.Id()", [&client]() {
      ipfs::http::replace_body = R"(not a JSON ~!*(@&(~{] indeed)";
      ipfs::Json id;
      client.Id(&id);
    });

    ipfs::http::TransportCurl transport_curl;
    transport_curl.Test();

  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
