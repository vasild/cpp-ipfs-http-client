# IPFS C++ HTTP API client library

The main class the library exports is @link ipfs::Client @endlink. The rest is only interesting to developers of this library.

## Build & Install

```sh
# Fetch the source code from GitHub.
git clone git@github.com:vasild/cpp-ipfs-http-client.git
cd cpp-ipfs-http-client

# Build the library inside a seperate build directory.
# Note: This is a shared build, you can Omit BUILD_SHARED_LIBS for static builds.
#
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
  -DBUILD_TESTING:BOOL=OFF \
  ..
make -j 8

# Optionally install it.
make install
```

_Hint:_ Use `cmake -LAH` to list all available options.

_Note:_ Omitting `DBUILD_SHARED_LIBS` above is fine as well, if you want to statically link this library into your project.

## Build via C++ compiler

```sh
g++ -std=c++11 -I/path/to/header -L/path/to/lib -lipfs-http-client myprog.cc -o myprog
```

## Install & Build using existing CMake project

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

## Dependencies

- [C++11 compiler](https://github.com/nlohmann/json#supported-compilers) (nlohmann json project is fetched automatically by CMake)
- [CMake](http://cmake.org), version 3.11.0 or higher
- [libcurl](https://curl.haxx.se/libcurl), version 7.25.0 or higher

Optionally (for generation this documentation you're reading):

- [Doxygen](https://www.doxygen.nl), version 1.9.0 or higher

# License

The code is distributed under the [MIT License](http://opensource.org/licenses/MIT).
