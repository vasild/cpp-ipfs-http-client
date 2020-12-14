# IPFS C++ HTTP API client library

The main class the library exports is @link ipfs::Client @endlink. The rest is only interesting to developers of this library.

# Install

```sh
# Fetch the source code from GitHub.
git clone git@github.com:vasild/cpp-ipfs-http-client.git

# Build out-of-source using cmake in the usual way.
# These extra options may be of interest in case the dependencies have
# been installed in non-standard locations:
# CURL_INCLUDE_DIR
# CURL_LIBRARY
mkdir build
cd build
cmake \
  -DCMAKE_INSTALL_PREFIX:PATH=/home/joe.smith/cpp-ipfs-http-client-install \
  -DCURL_INCLUDE_DIR:PATH=/home/joe.smith/include
  -DCURL_LIBRARY:PATH=/home/joe.smith/lib/libcurl.so
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_SHARED_LIBS:BOOL=ON \
  ../cpp-ipfs-http-client
make -j 8

# Optionally install it.
make install
```

*Note:* Omitting `DBUILD_SHARED_LIBS` above is fine as well, if you want to statically link this library into your project.

## Dependencies

- [C++11 compiler](https://github.com/nlohmann/json#supported-compilers)
- [CMake](http://cmake.org), version 3.11.0 or higher
- [libcurl](https://curl.haxx.se/libcurl), version 7.25.0 or higher

# License

The code is distributed under the [MIT License](http://opensource.org/licenses/MIT).
