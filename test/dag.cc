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
#include <ipfs/test/utils.h>

int main(int, char**) {
  try {
    ipfs::Client client("localhost", 5001);

    /** [ipfs::Client::DagPut] */
    ipfs::Json dag_to_store = R"(
      {
          "thing": "stuff",
          "MoreData": "important",
          "Stinks": [ {
              "Name": "old sock",
              "Splash": "QmXg9Pp2ytZ14xgmQjYEiHjVjMFXzCVVEcRTWJBmLgR39V",
              "Spize": 42
          } ]
      }
    )"_json;
    ipfs::Json dag_stored;
    client.DagPut(dag_to_store, &dag_stored);
    std::cout << "Dag to store:" << std::endl
              << dag_to_store.dump(2) << std::endl;
    std::cout << "Stored dag:" << std::endl
              << dag_stored.dump(2) << std::endl;
    /* An example output:
    Dag to store:
    {
      "thing": "stuff",
      "MoreData": "important",
      "Stinks": [
        {
          "Hash": "QmXg9Pp2ytZ14xgmQjYEiHjVjMFXzCVVEcRTWJBmLgR39V",
          "Name": "old sock",
          "Size": 42
        }
      ]
    }
    Stored dag:
    {
      "Cid": {
        "/": "bafyreie3ybajzu6qlfjxpq4mwe62klirtc7yxmpuzclc22e6ifremzlrmq"
      }
    }
    */
    /** [ipfs::Client::DagPut] */
    ipfs::test::check_if_properties_exist("client.DagPut()", dag_stored,
                                          {"Cid"});


    /** [ipfs::Client::DagGet] */
    ipfs::Json dag;
    client.DagGet("bafyreie3ybajzu6qlfjxpq4mwe62klirtc7yxmpuzclc22e6ifremzlrmq", &dag);
    std::cout << "Dag: " << std::endl << dag.dump(2) << std::endl;
    /* An example output:
    Dag:
    {
      "MoreData": "important",
      "Stinks": [
        {
          "Hash": "QmXg9Pp2ytZ14xgmQjYEiHjVjMFXzCVVEcRTWJBmLgR39V",
          "Name": "old sock",
          "Size": 42
        }
      ],
      "thing": "stuff"
    }
    */
    /** [ipfs::Client::DagGet] */

  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
