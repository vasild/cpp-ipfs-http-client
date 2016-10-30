# IPFS C++ API client library

The main class the library exports is @link ipfs::Client @endlink. The rest is only interesting to developers of this library.

# Install

```sh
cmake \
  -DCMAKE_INSTALL_PREFIX:PATH=/home/joe.smith/cpp-ipfs-api-install \
  -DJSON_FOR_MODERN_CXX_INCLUDE_DIR:PATH=/usr/local/include \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_SHARED_LIBS:BOOL=ON \
  /home/joe.smith/cpp-ipfs-api
make -j742 install
```

## Dependencies

- [C++11 compiler](https://github.com/nlohmann/json#supported-compilers)
- [CMake](http://cmake.org), version 3.1.0 or higher
- [libcurl](https://curl.haxx.se/libcurl), version 7.25.0 or higher
- [JSON for Modern C++](https://github.com/nlohmann/json), version 2.x.x

# License

Copyright &copy; 2016-2016 Vasil Dimov. The code is distributed under the [MIT License](http://opensource.org/licenses/MIT).
