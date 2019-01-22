
#include <iostream>
#include <stdexcept>
#include <string>

#include <hive/node.h>
#include <hive/cluster.h>
#include <hive/test/utils.h>

int main(int, char**) {
  try {
    ipfs::Node client("localhost", 9095);
    ipfs::Json respone;

    /** [ipfs::Node::FilesAdd] */
    ipfs::Json add_result;
    client.FileAdd(
        {{"foo.txt", ipfs::http::FileUpload::Type::kFileContents, "abcd"},
         {"bar.txt", ipfs::http::FileUpload::Type::kFileName, "./file_add.cc"}},
        &add_result);
    std::cout << "FilesAdd() result:" << std::endl
              << add_result.dump(2) << std::endl;

    /* An example output:
    [
      {
        "hash": "QmZJwDYpFZrdLHVy6Zhzd1bxq87zBcW8beR2eidihXPQ4j",
        "path": "bar.txt",
        "size": 1028
      },
      {
        "hash": "QmWPyMW2u7J2Zyzut7TcBMT8pG6F2cB4hmZk1vBJFBt1nP",
        "path": "foo.txt",
        "size": 4
      }
    ]
    */
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
