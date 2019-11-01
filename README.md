# IPFS C++ HTTP API client library

[![Build Status](https://api.travis-ci.org/vasild/cpp-ipfs-http-client.svg?branch=master)](https://travis-ci.org/vasild/cpp-ipfs-http-client)
[![Coverage Status](https://coveralls.io/repos/github/vasild/cpp-ipfs-http-client/badge.svg?branch=master)](https://coveralls.io/github/vasild/cpp-ipfs-http-client?branch=master)
[![Documentation](https://img.shields.io/badge/docs-doxygen-blue.svg)](https://vasild.github.io/cpp-ipfs-http-client)
[![GitHub Issues](https://img.shields.io/github/issues/vasild/cpp-ipfs-http-client.svg)](http://github.com/vasild/cpp-ipfs-http-client/issues)
[![CII Best Practices](https://bestpractices.coreinfrastructure.org/projects/465/badge)](https://bestpractices.coreinfrastructure.org/projects/465)

![](https://ipfs.io/ipfs/QmQJ68PFMDdAsgCZvA1UVzzn18asVcf7HVvCDgpjiSCAse)

Allows C++ applications to communicate with an [IPFS](https://ipfs.io) node.
It implements [IPFS API bindings](https://github.com/ipfs/interface-js-ipfs-core/blob/master/README.md#api) for C++.
See [the documentation](https://vasild.github.io/cpp-ipfs-http-client) or [the source code](https://github.com/vasild/cpp-ipfs-http-client).

See also [IPFS on GitHub](https://github.com/ipfs).

Currently implemented methods:

- [block](https://github.com/ipfs/interface-js-ipfs-core/blob/master/SPEC/BLOCK.md): get(), put(), stat()
- [config](https://github.com/ipfs/interface-js-ipfs-core/blob/master/SPEC/CONFIG.md): get(), set(), replace()
- [dht](https://github.com/ipfs/interface-js-ipfs-core/blob/master/SPEC/DHT.md): findpeer(), findprovs()
- [files](https://github.com/ipfs/interface-js-ipfs-core/blob/master/SPEC/FILES.md): get(), add()
- [miscellaneous](https://github.com/ipfs/interface-js-ipfs-core/blob/master/SPEC/MISCELLANEOUS.md): id(), version()
- [object](https://github.com/ipfs/interface-js-ipfs-core/blob/master/SPEC/OBJECT.md): new(), put(), get(), data(), links(), stat(), patch.addLink(), patch.rmLink(), patch.appendData(), patch.setData()
- [pin](https://github.com/ipfs/interface-js-ipfs-core/blob/master/SPEC/PIN.md): add(), ls(), rm()
- [swarm](https://github.com/ipfs/interface-js-ipfs-core/blob/master/SPEC/SWARM.md): addrs(), connect(), disconnect(), peers()
- [key](https://github.com/ipfs/interface-js-ipfs-core/blob/master/SPEC/KEY.md): gen(), list(), rm()

Not all methods are implemented.

# TODO

- Implement the missing methods
- Contributors are welcome!

# Install

```sh
cmake /path/to/cpp-ipfs-http-client
make
make install
```

See the [documentation for details](https://vasild.github.io/cpp-ipfs-http-client).

## Dependencies

- [C++11 compiler](https://github.com/nlohmann/json#supported-compilers)
- [CMake](http://cmake.org)
- [libcurl](https://curl.haxx.se/libcurl)
- [JSON for Modern C++](https://github.com/nlohmann/json)

# Usage

```cpp
#include <iostream>
#include <sstream>

#include <ipfs/client.h>

int main(int, char**) {
  std::stringstream contents;

  ipfs::Client client("localhost", 5001);

  client.FilesGet("/ipfs/QmYwAPJzv5CZsnA625s3Xf2nemtYgPpHdWEz79ojWnPbdG/readme", &contents);

  std::cout << contents.str() << std::endl;

  return 0;
}
```

```sh
c++ -std=c++11 -I/path/to/header -L/path/to/lib -lipfs-http-client myprog.cc -o myprog
```

# Contribute

Feel free to [open issues](https://github.com/vasild/cpp-ipfs-http-client/issues/new) and [pull requests](https://github.com/vasild/cpp-ipfs-http-client/compare).
Report vulnerabilities publicly, similar to other non-security issues.

The project adheres to the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html). Use [clang-format](http://clang.llvm.org/docs/ClangFormat.html) to properly format the code when you submit patches.

Write tests for new code. Changes should not cause the code coverage to go down.

# License

The code is distributed under the [MIT License](http://opensource.org/licenses/MIT).
