/* Copyright (c) 2016-2022, The C++ IPFS client library developers

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

#include <ipfs/client.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <thread>

int main(int, char**) {
  try {
    // Create client with high time-out of 2 minutes
    ipfs::Client client("localhost", 5001, "2m");

    std::cout << "INFO: Starting thread..." << std::endl;

    // Only start a single thread
    std::thread thread([&client]() {
      std::stringstream contents;
      try {
        // File should not exists, takes forever (until time-out)
        client.FilesGet("QmZp1rrtGTictR2rpNcx4673R7qU9Jdr9DQ6Z7F6Wgo2bQ",
                        &contents);
      } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
      }
    });

    // Abort request directly / stopping thread
    if (thread.joinable()) {
      std::cout << "INFO: Abort thread directly" << std::endl;
      /** [ipfs::Client::Abort] */
      client.Abort();
      thread.join();
      client.Reset();
      /** [ipfs::Client::Abort] */
    }

    std::cout << "INFO: Done!" << std::endl;
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
