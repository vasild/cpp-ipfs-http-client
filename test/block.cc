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
#include <sstream>
#include <stdexcept>

#include <ipfs/client.h>
#include <ipfs/test/utils.h>

int main(int, char**) {
  try {
    ipfs::Client client("localhost", 5001);

    /** [ipfs::Client::BlockPut] */
    ipfs::Json block;
    client.BlockPut(
        {"" /* no file name */, ipfs::http::FileUpload::Type::kFileContents,
         "Block put test."},
        &block);
    std::cout << "Stored block key: " << block["Key"] << std::endl;
    /* An example output:
    Stored block key: "QmQpWo5TL9nivqvL18Bq8bS34eewAA6jcgdVsUu4tGeVHo"
    */
    /** [ipfs::Client::BlockPut] */
    ipfs::test::check_if_properties_exist("client.BlockPut()", block,
                                          {"Key", "Size"});

    /** [ipfs::Client::BlockGet] */
    std::stringstream block_contents;
    /* E.g. block["Key"] is "QmQpWo5TL9nivqvL18Bq8bS34eewAA6jcgdVsUu4tGeVHo". */
    client.BlockGet(block["Key"], &block_contents);
    std::cout << "Block (hex): "
              << ipfs::test::string_to_hex(block_contents.str()) << std::endl;
    /* An example output:
    Block (hex): 426c6f636b2070757420746573742e
    */
    /** [ipfs::Client::BlockGet] */

    /** [ipfs::Client::BlockStat] */
    ipfs::Json stat_result;
    client.BlockStat(block["Key"], &stat_result);
    std::cout << "Stat: " << stat_result << std::endl;
    /* An example output:
    Stat: {"Key":"QmQpWo5TL9nivqvL18Bq8bS34eewAA6jcgdVsUu4tGeVHo","Size":15}
    */
    /** [ipfs::Client::BlockStat] */
    ipfs::test::check_if_properties_exist("client.BlockStat()", stat_result,
                                          {"Key", "Size"});
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
