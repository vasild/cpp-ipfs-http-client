# IPFS C++ API client library

This library allows C++ applications to communicate with [IPFS](https://ipfs.io).
It implements [IPFS API bindings](https://github.com/ipfs/interface-ipfs-core/tree/master/API) for C++.
See [the documentation](https://vasild.github.io/cpp-ipfs-api) or [the source code](https://github.com/vasild/cpp-ipfs-api).

See also [IPFS on GitHub](https://github.com/ipfs).

# Install

```sh
cmake /path/to/cpp-ipfs-api
make
make install
```

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

Feel free to open issues and pull requests.

# License

Copyright &copy; 2016-2016 Vasil Dimov. The code is distributed under the [MIT License](http://opensource.org/licenses/MIT).
