# Elastos Hive Native SDK

[![](https://img.shields.io/badge/made%20by-Elastos%20org-blue.svg?style=flat-square)](http://elastos.org)
[![](https://img.shields.io/badge/project-Hive-blue.svg?style=flat-square)](http://elastos.org/)
[![standard-readme compliant](https://img.shields.io/badge/standard--readme-OK-green.svg?style=flat-square)](https://github.com/RichardLitt/standard-readme)
[![Build Status](https://travis-ci.org/elastos/Elastos.NET.Hive.Native.SDK.svg?branch=master)](https://travis-ci.org/elastos/Elastos.NET.Hive.Native.SDK)
[![GitHub Issues](https://img.shields.io/github/issues/elastos/Elastos.NET.Hive.Native.SDK.svg)](http://github.com/elastos/Elastos.NET.Hive.Native.SDK/issues)

Allows C++ applications to communicate with [Hive.Cluster](https://github.com/elastos/Elastos.NET.Hive.Cluster).
It implements Hive.Cluster API bindings for C++.
See [the source code](http://github.com/elastos/Elastos.NET.Hive.Native.SDK).

See also [Hive.Cluster on GitHub](https://github.com/elastos/Elastos.NET.Hive.Cluster).

## Table of Contents

- [Install](#install)
- [Dependencies](#dependencies)
- [Usage](#usage)
- [Contributions](#contributions)
- [Acknowledgments](#Acknowledgments)
- [License](#license)

## Install

```sh
cmake /path/to/Elastos.Net.Hive.Native.SDK
make
make install
```

## Dependencies

- [C++11 compiler](https://github.com/nlohmann/json#supported-compilers)
- [CMake](http://cmake.org)
- [libcurl](https://curl.haxx.se/libcurl)
- [JSON for Modern C++](https://github.com/nlohmann/json)

## Usage

```cpp
#include <iostream>
#include <string>

#include <ipfs/node.h>

int main(int, char**) {
  ipfs::Json userInfo;
  std::string uid = "uid-132fdc89-4e85-47f5-b942-fd7ba515a6f3";

  ipfs::Node node("localhost", 9095);
  node.UidInfo(uid, &userInfo);

  std::cout << "uid info: " << userInfo["Name"] << " => " << userInfo["Id"]
            << std::endl;

  return 0;
}
```

```sh
c++ -std=c++11 -I/path/to/header -L/path/to/lib -lhive-api myprog.cc -o myprog
```

## Contributions

We welcome any contribution to Elastos Hive projects.

## Acknowledgments

A sincere thank you to all teams and projects that we rely on directly or indirectly.

## License

This project is licensed under the terms of the [MIT license](https://github.com/elastos/Elastos.NET.Native.SDK/blob/master/LICENSE).