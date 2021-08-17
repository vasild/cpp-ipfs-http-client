/* Copyright (c) 2021, The C++ IPFS client library developers

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
#include <stdexcept>
#include <string>

int main(int, char**) {
  try {
    ipfs::Client client("localhost", 5001);

    /** [ipfs::Client::StatsBw] */
    ipfs::Json bandwidth_info;

    client.StatsBw(&bandwidth_info);

    std::cout << "Bandwidth:" << std::endl << bandwidth_info.dump(2) << std::endl;
    /* An example output:
    Bandwidth:
    {
    "RateIn": 4541.421091935148,
    "RateOut": 677.3253862633403,
    "TotalIn": 15994960,
    "TotalOut": 6696092
    }
    */
    /** [ipfs::Client::StatsBw] */

    /** [ipfs::Client::StatsRepo] */
    ipfs::Json repo_stats;

    client.StatsRepo(&repo_stats);

    std::cout << "Repo:" << std::endl << repo_stats.dump(2) << std::endl;
    /* An example output:
    Repo:
    {
      "RepoSize":256893470,
      "StorageMax":20000000000,
      "NumObjects":24935,
      "RepoPath":"/home/melroy/.ipfs",
      "Version":"fs-repo@11"
    }
    */
    /** [ipfs::Client::StatsRepo] */

  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }



  return 0;
}
