/* Copyright (c) 2016-2021, The C++ IPFS client library developers

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
#include <ipfs/test/utils.h>

#include <iostream>
#include <sstream>
#include <stdexcept>

using Json = nlohmann::json;

int main(int, char**) {
  try {
    ipfs::Client client("localhost", 5001);

    /** [ipfs::Client::KeyGen] */
    std::string key_id;
    client.KeyGen("foobar-key", "rsa", 2048, &key_id);
    std::cout << "Generated key: " << key_id << std::endl;
    client.KeyGen("foobar-key2", "rsa", 2048, &key_id);
    std::cout << "Generated key2: " << key_id << std::endl;
    /* An example output:
    Generated key: "QmQeVW8BKqpHbUV5GcecC3wDLF3iqV6ZJhtFN8q8mUYFUs"
    */
    /** [ipfs::Client::KeyGen] */

    /** [ipfs::Client::KeyList] */
    Json key_list;
    client.KeyList(&key_list);
    std::cout << "A list of all local keys: " << key_list.dump(2) << std::endl;
    /* An example output:
    A list of all local keys: [
      {
        "Id": "QmQ9KnoHP3iRwXLSrdDb8wdJCbEdjk7hVFYGUGDUsdC41k",
        "Name": "self"
      },
      {
        "Id": "QmNjm428tzU5P2ik1yDJNZP2F5zG2jb8fHXZ5D2mzvJGLg",
        "Name": "foobar-key"
      }
    ]
    */
    /** [ipfs::Client::KeyList] */

    /** [ipfs::Client::KeyRm] */
    /* Successful, if no error is thrown. */
    client.KeyRm("foobar-key");
    /** [ipfs::Client::KeyRm] */

    /** [ipfs::Client::KeyRename] */
    Json key_list_rename;
    /* Renaming foobar-key2 to foobar-new-key2 */
    client.KeyRename("foobar-key2", "foobar-new-key2");
    client.KeyList(&key_list_rename);
    std::cout << "A list of new local keys: " << key_list_rename.dump(2) << std::endl;
    /** [ipfs::Client::KeyRename] */

  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
