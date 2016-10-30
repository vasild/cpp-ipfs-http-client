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

- C++11 compiler
- [CMake](http://cmake.org)
- [libcurl](https://curl.haxx.se/libcurl)
- [JSON for Modern C++](https://github.com/nlohmann/json)

# Usage

The main class the library exports is @link ipfs::Client @endlink. The rest is only interesting to developers of this library.

# Contribute

Feel free to open issues and pull requests.

# License

Copyright &copy; 2016-2016 Vasil Dimov. The code is distributed under the [MIT License](http://opensource.org/licenses/MIT).
