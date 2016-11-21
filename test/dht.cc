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

#include <ipfs/client.h>

int main(int, char**) {
  try {
    ipfs::Client client("localhost", 5001);

    ipfs::Json add_result;
    client.FilesAdd(
        {{"foo.txt", ipfs::http::FileUpload::Type::kFileContents, "abcd"}},
        &add_result);
    const std::string& hash = add_result[0]["hash"];

    /** [ipfs::Client::DhtFindProvs] */
    /* std::string hash = "QmWPyMW2u7J2Zyzut7TcBMT8pG6F...Bt1nP" for example. */
    ipfs::Json providers;

    client.DhtFindProvs(hash, &providers);

    std::cout << "Providers for " << hash << ":" << std::endl
              << providers.dump(2).substr(0, 512) << "..." << std::endl;
    /* An example output:
    [
      {
        "Extra": "",
        "ID": "",
        "Responses": [
          {
            "Addrs": null,
            "ID": "QmZSb7SYajaEEbJU2FB4XJWWfxX9AjwmdreK5MDu9zuQvn"
          }
        ],
        "Type": 4
      },
      {
        "Extra": "",
        "ID": "QmYoQjsZeSyzeeX9yCu5jfETgKinR2Udeej385FzPprsGg",
        "Responses": null,
        "Type": 6
      },
      {
        "Extra": "",
        "ID": "QmdsoQ8xhfkaEpAYCLJ75LEiup9E4ybUdbqDkZZKggfWjS",
        "Responses": null,
        "Type": 6
      },...
    */
    /** [ipfs::Client::DhtFindProvs] */
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
