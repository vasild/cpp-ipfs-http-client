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
#include <sstream>
#include <stdexcept>

#include <ipfs/client.h>
#include <ipfs/test/utils.h>

int main(int, char**) {
  try {
    ipfs::Client client("localhost", 5001);

    /** [ipfs::Client::NamePublish] */
    std::string object_id = "QmRrVRGx5xAXX52BYuScmJk1KWPny86BtexP8YNJ8jz76U";
    std::string name;
    client.NamePublish(
        object_id,
        &name);
    /* An example output: "QmbJ5UzreC86qtHrWC2SwWKLsTiLqTuG4cqHHJVdYPK6s9" */
    std::cout << "Published name: " << name << std::endl;

    /** [ipfs::Client::NameResolve] */
    // std::string name = "QmPXahdEAs7nGhTAPmct458LEWtEsWRGaX1xXBURdJu1ir";
    std::string resolved_object_path;
    client.NameResolve(
        name,
        &resolved_object_path);
    /* An example output:
        "/ipfs/QmRrVRGx5xAXX52BYuScmJk1KWPny86BtexP8YNJ8jz76U"
    */
    std::cout << "Resolved name: " << resolved_object_path << std::endl;

  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
