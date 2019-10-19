/* Copyright (c) 2016-2016, Vasil Dimov
   Copyright (c) 2019 Linas Vepštas

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
#include <sstream>
#include <stdexcept>

#include <ipfs/client.h>
#include <ipfs/test/utils.h>

int main(int, char**) {
  try {
    ipfs::Client client("localhost", 5001);

    /** [ipfs::Client::KeyNew] */
    // We need a key here, so as not to clobber the "self" key.
    std::string key_id;
    client.KeyNew("foobar-key", &key_id);
    std::cout << "Generated key: " << key_id << std::endl;

    /** [ipfs::Client::NamePublish] */
    // The object here is well-known.
    std::string object_id = "QmYwAPJzv5CZsnA625s3Xf2nemtYgPpHdWEz79ojWnPbdG";
    std::string name;
    client.NamePublish(
        object_id,
        "foobar-key",
        &name,
        "4h",     // lifetime
        "30s");   // ttl
    /* An example output: "QmbJ5UzreC86qtHrWC2SwWKLsTiLqTuG4cqHHJVdYPK6s9" */
    std::cout << "Published name: " << name << std::endl;

    /** [ipfs::Client::KeyRm] */
    /* Remove the key so that it doesn't pollute the key-space. */
    client.KeyRm("foobar-key");

    /** [ipfs::Client::NameResolve] */
    std::string resolved_object_path;
    client.NameResolve(
        name,
        &resolved_object_path);
    /* The expected output:
        "/ipfs/QmYwAPJzv5CZsnA625s3Xf2nemtYgPpHdWEz79ojWnPbdG";
    */
    std::cout << "Resolved name: " << resolved_object_path << std::endl;

    std::string expected = "/ipfs/" + object_id;
    ipfs::test::check_if_string_contains("client.NameResolve()",
        resolved_object_path, expected);

  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
