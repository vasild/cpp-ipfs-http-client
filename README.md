# IPFS C++ API client library

[![Build Status](https://api.travis-ci.org/vasild/cpp-ipfs-api.svg?branch=master)](https://travis-ci.org/vasild/cpp-ipfs-api)
[![Coverage Status](https://coveralls.io/repos/github/vasild/cpp-ipfs-api/badge.svg?branch=master)](https://coveralls.io/github/vasild/cpp-ipfs-api?branch=master)
[![Documentation](https://img.shields.io/badge/docs-doxygen-blue.svg)](https://vasild.github.io/cpp-ipfs-api)
[![GitHub Issues](https://img.shields.io/github/issues/vasild/cpp-ipfs-api.svg)](http://github.com/vasild/cpp-ipfs-api/issues)
[![CII Best Practices](https://bestpractices.coreinfrastructure.org/projects/465/badge)](https://bestpractices.coreinfrastructure.org/projects/465)

![](https://ipfs.io/ipfs/QmQJ68PFMDdAsgCZvA1UVzzn18asVcf7HVvCDgpjiSCAse)

Allows C++ applications to communicate with [IPFS](https://ipfs.io).
It implements [IPFS API bindings](https://github.com/ipfs/interface-ipfs-core/tree/master/API) for C++.
See [the documentation](https://vasild.github.io/cpp-ipfs-api) or [the source code](https://github.com/vasild/cpp-ipfs-api).

See also [IPFS on GitHub](https://github.com/ipfs).

Currently implemented methods:

- [block](https://github.com/ipfs/interface-ipfs-core/tree/master/API/block): get(), put(), stat()
- [config](https://github.com/ipfs/interface-ipfs-core/tree/master/API/config): get(), set(), replace()
- [dht](https://github.com/ipfs/interface-ipfs-core/tree/master/API/dht): findpeer(), findprovs()
- [files](https://github.com/ipfs/interface-ipfs-core/tree/master/API/files): get(), add()
- [generic](https://github.com/ipfs/interface-ipfs-core/tree/master/API/generic): id(), version()
- [object](https://github.com/ipfs/interface-ipfs-core/tree/master/API/object): new(), put(), get(), data(), links(), stat(), patch.addLink(), patch.rmLink(), patch.appendData(), patch.setData()
- [pin](https://github.com/ipfs/interface-ipfs-core/tree/master/API/pin): add(), ls(), rm()
- [swarm](https://github.com/ipfs/interface-ipfs-core/tree/master/API/swarm): addrs(), connect(), disconnect(), peers()

Not implemented yet:

- [dag](https://github.com/ipfs/interface-ipfs-core/tree/master/API/dag): get(), put()
- [dht](https://github.com/ipfs/interface-ipfs-core/tree/master/API/dht): get(), put(), query()

# TODO

- Implement the above methods
- Compare with the latest [IPFS API bindings](https://github.com/ipfs/interface-ipfs-core/tree/master/API) and implement any new methods that have been defined
- Contributors are welcome!

# Install

```sh
cmake /path/to/cpp-ipfs-api
make
make install
```

See the [documentation for details](https://vasild.github.io/cpp-ipfs-api).

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
c++ -std=c++11 -I/path/to/header -L/path/to/lib -lipfs-api myprog.cc -o myprog
```

# Contribute

Feel free to [open issues](https://github.com/vasild/cpp-ipfs-api/issues/new) and [pull requests](https://github.com/vasild/cpp-ipfs-api/compare).
Report vulnerabilities publicly, similar to other non-security issues.

The project adheres to the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html). Use [clang-format](http://clang.llvm.org/docs/ClangFormat.html) to properly format the code when you submit patches.

Write tests for new code. Changes should not cause the code coverage to go down.

# License

Copyright &copy; 2016-2016 Vasil Dimov. The code is distributed under the [MIT License](http://opensource.org/licenses/MIT).
