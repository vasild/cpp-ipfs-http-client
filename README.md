# IPFS C++ HTTP API client library

[![Build CI Status](https://github.com/vasild/cpp-ipfs-http-client/actions/workflows/build_and_test.yml/badge.svg)](https://github.com/vasild/cpp-ipfs-http-client/actions/workflows/build_and_test.yml)
[![CodeQL](https://github.com/vasild/cpp-ipfs-http-client/actions/workflows/codeql-analysis.yml/badge.svg)](https://github.com/vasild/cpp-ipfs-http-client/actions/workflows/codeql-analysis.yml)
[![Documentation](https://img.shields.io/badge/docs-doxygen-blue)](https://vasild.github.io/cpp-ipfs-http-client)
[![codecov](https://codecov.io/gh/vasild/cpp-ipfs-http-client/branch/master/graph/badge.svg?token=4k5pulEnHE)](https://codecov.io/gh/vasild/cpp-ipfs-http-client)
[![GitHub Issues](https://img.shields.io/github/issues/vasild/cpp-ipfs-http-client)](http://github.com/vasild/cpp-ipfs-http-client/issues)
[![GitHub pull requests](https://img.shields.io/github/issues-pr/vasild/cpp-ipfs-http-client)](https://github.com/vasild/cpp-ipfs-http-client/pulls)

![IPFS C++ HTTP Client Library](https://ipfs.io/ipfs/QmQJ68PFMDdAsgCZvA1UVzzn18asVcf7HVvCDgpjiSCAse)

Allows C++ applications to communicate with an [IPFS](https://ipfs.io) node.
It implements [IPFS API bindings](https://github.com/ipfs/interface-js-ipfs-core/blob/master/README.md#api) for C++.
See [the documentation](https://vasild.github.io/cpp-ipfs-http-client) and in partially the [Client Class](https://vasild.github.io/cpp-ipfs-http-client/classipfs_1_1Client.html).

See also [IPFS on GitHub](https://github.com/ipfs).

The C++ API is broken up into the following sections (Note: links below go to the `js-ipfs` project). The following calls are implemented in `cpp-ipfs-http-client`:

- [Bitswap](https://github.com/ipfs/js-ipfs/blob/master/docs/core-api/BITSWAP.md): *all methods are still to-do*
- [Block](https://github.com/ipfs/js-ipfs/blob/master/docs/core-api/BLOCK.md): get(), put(), stat()
- [Bootstrap](https://github.com/ipfs/js-ipfs/blob/master/docs/core-api/BOOTSTRAP.md): *all methods are still to-do*
- [Config](https://github.com/ipfs/js-ipfs/blob/master/docs/core-api/CONFIG.md): get(), set(), replace()
- [DAG](https://github.com/ipfs/js-ipfs/blob/master/docs/core-api/DAG.md): *all methods are still to-do*
- [DHT](https://github.com/ipfs/js-ipfs/blob/master/docs/core-api/DHT.md): findpeer(), findprovs()
- [Files](https://github.com/ipfs/js-ipfs/blob/master/docs/core-api/FILES.md): cat(), add(), ls()
- [Key](https://github.com/ipfs/js-ipfs/blob/master/docs/core-api/KEY.md): gen(), list(), rm(), rename()
- [Miscellaneous](https://github.com/ipfs/js-ipfs/blob/master/docs/core-api/MISCELLANEOUS.md): id(), version()
- [Name](https://github.com/ipfs/js-ipfs/blob/master/docs/core-api/NAME.md): *all methods are still to-do*
- [Object](https://github.com/ipfs/js-ipfs/blob/master/docs/core-api/OBJECT.md): new(), put(), get(), data(), links(), stat(), patch.addLink(), patch.rmLink(), patch.appendData(), patch.setData()
- [Pin](https://github.com/ipfs/js-ipfs/blob/master/docs/core-api/PIN.md): add(), ls(), rm()
- [PubSub](https://github.com/ipfs/js-ipfs/blob/master/docs/core-api/PUBSUB.md): *all methods are still to-do*
- [Refs](https://github.com/ipfs/js-ipfs/blob/master/docs/core-api/REFS.md): *all methods are still to-do*
- [Repo](https://github.com/ipfs/js-ipfs/blob/master/docs/core-api/REPO.md): stat()
- [Stats](https://github.com/ipfs/js-ipfs/blob/master/docs/core-api/STATS.md): bw(), repo() see Repo above
- [Swarm](https://github.com/ipfs/js-ipfs/blob/master/docs/core-api/SWARM.md): addrs(), connect(), disconnect(), peers()

As you can see, not all methods are yet implemented.

## TODO

- Implement the missing methods
- Contributors are welcome!

## Dependencies

- [C++11 compiler](https://github.com/nlohmann/json#supported-compilers) (nlohmann json project is fetched automatically by CMake)
- [CMake](http://cmake.org)
- [libcurl](https://curl.haxx.se/libcurl)

When building documention, you also need: 

- [Doxygen](https://www.doxygen.nl/download.html) (>= v1.9.0)

## Build & Install

```sh
git clone https://github.com/vasild/cpp-ipfs-http-client.git
cd cpp-ipfs-http-client
# Configure build
cmake -B build
# Build
cmake --build build -j 8
# Install
sudo cmake --install build
```

*Hint #1:* You can also build using **Ninja** (iso Make), use the following as configure: `cmake -GNinja -B build`, then use: `cmake --build build` which will use Ninja, _no need for `-j` anymore_.  
*Hint #2:* Build a specific target (eg. ipfs-http-client), use: `cmake --build build --target ipfs-http-client -j 8`  
*Hint #3:* You could also build the library without tests, use the option: `cmake -DBUILD_TESTING=OFF -B build`

See the [documentation for details](https://vasild.github.io/cpp-ipfs-http-client).

## Run Test cases

Only build & run the test cases, *without* code coverage:

```sh
# Prepare
cmake -B build
# Build & run our tests
cmake --build build --target our_tests -j 8
```

## Build & run Test cases + Code Coverage

Test cases are build by default, but if you want to build with coverage:

```sh
cmake -DCMAKE_BUILD_TYPE=Debug -DCOVERAGE:BOOL=ON -DBUILD_SHARED_LIBS:BOOL=ON -B build
# Build & run tests + HTML report
cmake --build build --target ctest_coverage_html -j 8

# Or build & run tests + Cobertura XML file
cmake --build build --target ctest_coverage_xml -j 8
```

## Build Doxygen

Build Doxygen files locally. From the root directory of this project:

```sh
cmake -DDOC=ON  -B build
cmake --build build --target doc
```

## Usage

```cpp
#include <ipfs/client.h>

#include <iostream>
#include <sstream>

int main(int, char**) {
  std::stringstream contents;

  ipfs::Client client("localhost", 5001);

  client.FilesGet("/ipfs/QmYwAPJzv5CZsnA625s3Xf2nemtYgPpHdWEz79ojWnPbdG/readme",
                  &contents);

  std::cout << contents.str() << std::endl;

  return 0;
}
```

More info see: [Doxygen Docs - Client Class](https://vasild.github.io/cpp-ipfs-http-client/classipfs_1_1Client.html).

### Multi-threading example

The client constructor and destructor are not thread safe. However, all the API IPFS calls are **thread safe**!

*Note:* A runtime error will be thrown on the request call (in this example the `FilesGet()`) when you call the `Abort()` method, allowing you to stop your code execution inside the thread.

An example of using a thread together with the IPFS Client:

```cpp
#include <ipfs/client.h>

#include <sstream>
#include <thread>

int main(int, char**) {
  ipfs::Client client("localhost", 5001, "2m");

  // Only start one thread
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

  if (thread.joinable()) {
    client.Abort();  // Abort request
    thread.join();   // Should not be blocking now
    client.Reset();  // Reset internal state
  }
  return 0;
}
```

See the full multi-threading example on: [Doxygen - Examples](https://vasild.github.io/cpp-ipfs-http-client/examples.html).

## Build via C++ compiler

```sh
g++ -std=c++11 -I/path/to/header -L/path/to/lib -lipfs-http-client myprog.cc -o myprog
```

## Build via CMake

Use the C++ IPFS Client inside an existing CMake project. We add the IPFS client inside the `lib` folder.
For example via `git submodule` (but `git clone` should also work):

```sh
cd your-cmake-project
git submodule add https://github.com/vasild/cpp-ipfs-http-client.git lib/ipfs-http-client
```

Edit your `CMakeLists.txt` file to include the C++ IPFS HTTP Client in your build:

```cmake
add_subdirectory (lib/ipfs-http-client)
```

Finally, add the C++ IPFS HTTP static library to your target using `target_link_libraries()`  
(in this example `${PROJECT_TARGET}` variable is used as target name):

```cmake
set(PROJECT_TARGET my-app)

target_link_libraries(${PROJECT_TARGET} PRIVATE ipfs-http-client)
```

## Contribute

Feel free to [open issues](https://github.com/vasild/cpp-ipfs-http-client/issues/new) and [pull requests](https://github.com/vasild/cpp-ipfs-http-client/compare).
Report vulnerabilities publicly, similar to other non-security issues.

The project adheres to the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html). Use [clang-format](http://clang.llvm.org/docs/ClangFormat.html) to properly format the code when you submit patches.

Write tests for new code. Changes should not cause the code coverage to go down (ideally up).

## License

The code is distributed under the [MIT License](http://opensource.org/licenses/MIT).
