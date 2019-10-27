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
#include <ipfs/test/utils.h>

int main(int, char**) {
  try {
    ipfs::Client client("localhost", 5001);

    /** [ipfs::Client::ObjectNew] */
    std::string object_id;
    client.ObjectNew(&object_id);
    std::cout << "New object id: " << object_id << std::endl;
    /* An example output:
    New object id: QmdfTbBqBPQ7VNxZEYEj14VmRuZBkqFbiwReogJgS1zR1n
    */
    /** [ipfs::Client::ObjectNew] */

    /** [ipfs::Client::ObjectPut] */
    ipfs::Json object_to_store = R"(
      {
          "Data": "another",
          "Links": [ {
              "Name": "some link",
              "Hash": "QmXg9Pp2ytZ14xgmQjYEiHjVjMFXzCVVEcRTWJBmLgR39V",
              "Size": 8
          } ]
      }
    )"_json;
    ipfs::Json object_stored;
    client.ObjectPut(object_to_store, &object_stored);
    std::cout << "Object to store:" << std::endl
              << object_to_store.dump(2) << std::endl;
    std::cout << "Stored object:" << std::endl
              << object_stored.dump(2) << std::endl;
    /* An example output:
    Object to store:
    {
      "Data": "another",
      "Links": [
        {
          "Hash": "QmXg9Pp2ytZ14xgmQjYEiHjVjMFXzCVVEcRTWJBmLgR39V",
          "Name": "some link",
          "Size": 8
        }
      ]
    }
    Stored object:
    {
      "Hash": "QmZZmY4KCu9r3e7M2Pcn46Fc5qbn6NpzaAGaYb22kbfTqm",
    }
    */
    /** [ipfs::Client::ObjectPut] */
    ipfs::test::check_if_properties_exist("client.ObjectPut()", object_stored,
                                          {"Hash"});

    /** [ipfs::Client::ObjectGet] */
    ipfs::Json object;
    client.ObjectGet("QmdfTbBqBPQ7VNxZEYEj14VmRuZBkqFbiwReogJgS1zR1n", &object);
    std::cout << "Object: " << std::endl << object.dump(2) << std::endl;
    /* An example output:
    Object:
    {
      "Data": "another",
      "Links": [ {
        "Name": "some link",
        "Hash": "QmXg9Pp2ytZ14xgmQjYEiHjVjMFXzCVVEcRTWJBmLgR39V",
        "Size": 8
      } ]
    }
    */
    /** [ipfs::Client::ObjectGet] */

    /** [ipfs::Client::ObjectData] */
    std::string data;
    client.ObjectData("QmZZmY4KCu9r3e7M2Pcn46Fc5qbn6NpzaAGaYb22kbfTqm", &data);
    std::cout << "Object data: " << data << std::endl;
    /* An example output:
    Object data: another
    */
    /** [ipfs::Client::ObjectData] */

    /** [ipfs::Client::ObjectLinks] */
    ipfs::Json links;
    client.ObjectLinks("QmYwAPJzv5CZsnA625s3Xf2nemtYgPpHdWEz79ojWnPbdG",
                       &links);
    std::cout << "Object links:" << std::endl << links.dump(2) << std::endl;
    /* An example output:
    [
      {
        "Hash": "QmZTR5bcpQD7cFgTorqxZDYaew1Wqgfbd2ud9QqGPAkK2V",
        "Name": "about",
        "Size": 1688
      },
      {
        "Hash": "QmYCvbfNbCwFR45HiNP45rwJgvatpiW38D961L5qAhUM5Y",
        "Name": "contact",
        "Size": 200
      },
      {
        "Hash": "QmY5heUM5qgRubMDD1og9fhCPA6QdkMp3QCwd4s7gJsyE7",
        "Name": "help",
        "Size": 322
      },
      {
        "Hash": "QmdncfsVm2h5Kqq9hPmU7oAVX2zTSVP3L869tgTbPYnsha",
        "Name": "quick-start",
        "Size": 1728
      },
      {
        "Hash": "QmPZ9gcCEpqKTo6aq61g2nXGUhM4iCL3ewB6LDXZCtioEB",
        "Name": "readme",
        "Size": 1102
      },
      {
        "Hash": "QmTumTjvcYCAvRRwQ8sDRxh8ezmrcr88YFU7iYNroGGTBZ",
        "Name": "security-notes",
        "Size": 1027
      }
    ]
    */
    /** [ipfs::Client::ObjectLinks] */

    /** [ipfs::Client::ObjectStat] */
    ipfs::Json stat;
    client.ObjectStat("QmYwAPJzv5CZsnA625s3Xf2nemtYgPpHdWEz79ojWnPbdG", &stat);
    std::cout << "Object data size: " << stat["DataSize"] << std::endl;
    /* An example output:
    Object data size: 2
    */
    /** [ipfs::Client::ObjectStat] */
    ipfs::test::check_if_properties_exist(
        "client.ObjectStat()", stat, {"BlockSize", "CumulativeSize", "DataSize",
                                      "Hash", "LinksSize", "NumLinks"});

    {
      /** [ipfs::Client::ObjectPatchAddLink] */
      /* Create a new node, upload two files and link them to the node. */

      std::string orig_id;
      client.ObjectNew(&orig_id);

      ipfs::Json added_files;
      client.FilesAdd(
          {{"file1.txt", ipfs::http::FileUpload::Type::kFileContents, "f1f1"},
           {"file2.txt", ipfs::http::FileUpload::Type::kFileContents, "f2f2"}},
          &added_files);

      std::string new_id;
      client.ObjectPatchAddLink(orig_id, "link to file1.txt",
                                added_files[0]["hash"], &new_id);

      ipfs::Json new_object;
      client.ObjectGet(new_id, &new_object);
      std::cout << "Added a link to " << orig_id << "." << std::endl
                << "New object " << new_id << ":" << std::endl
                << new_object.dump(2) << std::endl;
      /* An example output:
      Added a link to QmdfTbBqBPQ7VNxZEYEj14VmRuZBkqFbiwReogJgS1zR1n.
      New object QmRgn5ZeLiPRwQHqSbMLUZ2gkyLbJiaxVEx3LrDCqqaCAb:
      {
        "Data": "",
        "Links": [
          {
            "Hash": "QmNYaS23te5Rja36U94JoSTuMxJZmBEnHN8KEcjR6rGRGn",
            "Name": "link to file1.txt",
            "Size": 12
          }
        ]
      }
      */

      orig_id = new_id;
      client.ObjectPatchAddLink(orig_id, "link to file2.txt",
                                added_files[1]["hash"], &new_id);

      client.ObjectGet(new_id, &new_object);
      std::cout << "Added another link to " << orig_id << "." << std::endl
                << "New object " << new_id << ":" << std::endl
                << new_object.dump(2) << std::endl;
      /* An example output:
      Added another link to QmRgn5ZeLiPRwQHqSbMLUZ2gkyLbJiaxVEx3LrDCqqaCAb.
      New object QmTxf3cBwrzyRvCZgDQni5wkRkcpM81wiWFTK17okLtT21:
      {
        "Data": "",
        "Links": [
          {
            "Hash": "QmNYaS23te5Rja36U94JoSTuMxJZmBEnHN8KEcjR6rGRGn",
            "Name": "link to file1.txt",
            "Size": 12
          },
          {
            "Hash": "QmYuNVU4vwpXqX9RLv47HbmiveWwZvLBsXyYbUtEQMJYGQ",
            "Name": "link to file2.txt",
            "Size": 12
          }
        ]
      }
      */
      /** [ipfs::Client::ObjectPatchAddLink] */

      std::string target_id = new_id;
      /** [ipfs::Client::ObjectPatchRmLink] */
      /* std::string target_id = "QmTxf3cBwrzyRvCZgDQni...LtT21" for example. */
      std::string without_link_id;

      client.ObjectPatchRmLink(target_id, "link to file1.txt",
                               &without_link_id);

      ipfs::Json without_link;
      client.ObjectGet(without_link_id, &without_link);
      std::cout << "Removed a link from " << target_id << "." << std::endl
                << "New object " << without_link_id << ":" << std::endl
                << without_link.dump(2) << std::endl;
      /* An example output:
      Removed a link from QmTxf3cBwrzyRvCZgDQni5wkRkcpM81wiWFTK17okLtT21.
      New object QmVUXX6chMMT4cPosYsLq64FZ21ie94KPknSnWfG8YDJBB:
      {
        "Data": "",
        "Links": [
          {
            "Hash": "QmYuNVU4vwpXqX9RLv47HbmiveWwZvLBsXyYbUtEQMJYGQ",
            "Name": "link to file2.txt",
            "Size": 12
          }
        ]
      }
      */

      bool removal_ok;
      try {
        client.ObjectPatchRmLink(target_id, "nonexistent", &without_link_id);
        removal_ok = true;
      } catch (const std::exception&) {
        removal_ok = false;
      }

      if (removal_ok) {
        throw std::runtime_error(
            "Removal of nonexistent link succeeded but should have failed.");
      }
      /** [ipfs::Client::ObjectPatchRmLink] */

      std::string append_here_id = target_id;
      /** [ipfs::Client::ObjectPatchAppendData] */
      /* std::string target_id = "QmTxf3cBwrzyRvCZgDQni...LtT21" for example. */
      std::string with_appended_data_id;

      client.ObjectPatchAppendData(
          target_id,
          {"", ipfs::http::FileUpload::Type::kFileContents, "appended data"},
          &with_appended_data_id);

      ipfs::Json with_appended_data;
      client.ObjectGet(with_appended_data_id, &with_appended_data);
      std::cout << "Appended data to " << target_id << "." << std::endl
                << "New object " << with_appended_data_id << ":" << std::endl
                << with_appended_data.dump(2) << std::endl;
      /* An example output:
      Appended data to QmTxf3cBwrzyRvCZgDQni5wkRkcpM81wiWFTK17okLtT21.
      New object QmbDtmUVyiN8vFZr8cDTpuVtZwnVd3k6wqpEq3NvE67bTj:
      {
        "Data": "appended data",
        "Links": [
          {
            "Hash": "QmNYaS23te5Rja36U94JoSTuMxJZmBEnHN8KEcjR6rGRGn",
            "Name": "link to file1.txt",
            "Size": 12
          },
          {
            "Hash": "QmYuNVU4vwpXqX9RLv47HbmiveWwZvLBsXyYbUtEQMJYGQ",
            "Name": "link to file2.txt",
            "Size": 12
          }
        ]
      }
      */
      /** [ipfs::Client::ObjectPatchAppendData] */

      target_id = with_appended_data_id;
      /** [ipfs::Client::ObjectPatchSetData] */
      /* std::string target_id = "QmbDtmUVyiN8vFZr8cDTp...67bTj" for example. */
      std::string with_new_data_id;

      client.ObjectPatchSetData(
          target_id,
          {"", ipfs::http::FileUpload::Type::kFileContents, "new data"},
          &with_new_data_id);

      ipfs::Json with_new_data;
      client.ObjectGet(with_new_data_id, &with_new_data);
      std::cout << "Set the data of " << target_id << "." << std::endl
                << "New object " << with_new_data_id << ":" << std::endl
                << with_new_data.dump(2) << std::endl;
      /* An example output:
      Set the data of QmbDtmUVyiN8vFZr8cDTpuVtZwnVd3k6wqpEq3NvE67bTj.
      New object QmZ19WxdWzPbLTyTkb1MYDvpdLi1pu1fRtECAiHzhLJAxz:
      {
        "Data": "new data",
        "Links": [
          {
            "Hash": "QmNYaS23te5Rja36U94JoSTuMxJZmBEnHN8KEcjR6rGRGn",
            "Name": "link to file1.txt",
            "Size": 12
          },
          {
            "Hash": "QmYuNVU4vwpXqX9RLv47HbmiveWwZvLBsXyYbUtEQMJYGQ",
            "Name": "link to file2.txt",
            "Size": 12
          }
        ]
      }
      */
      /** [ipfs::Client::ObjectPatchSetData] */
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
