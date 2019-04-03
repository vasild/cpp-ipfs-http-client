# Elastos Hive Native SDK

[![Build Status](https://camo.githubusercontent.com/0950ada27e9e49dd28bd1083f3ce1c4acffa983a/68747470733a2f2f6170692e7472617669732d63692e6f72672f766173696c642f6370702d697066732d6170692e7376673f6272616e63683d6d6173746572)](https://travis-ci.org/elastos/Elastos.NET.Hive.Native.SDK)
[![Coverage Status](https://camo.githubusercontent.com/952ba40079aafb7f388c94988b5d7df29a8a8692/68747470733a2f2f636f766572616c6c732e696f2f7265706f732f6769746875622f766173696c642f6370702d697066732d6170692f62616467652e7376673f6272616e63683d6d6173746572)](https://coveralls.io/github/elastos/Elastos.NET.Hive.Native.SDK?branch=master)

## Introduction

Elastos Hive is a decentralized file storage infrastructure which leveraged IPFS/Cluster. It provides file-grained storage capabilities to Elastos dApps, while Hive Native SDK is a set of C/C++ APIs used by dApps to access or store files to or from Hive storage network without any hive-node built-in within dApps.

## Table of Contents

- [Elastos Hive Native SDK](#elastos-hive-native-sdk)
  - [Introduction](#introduction)
  - [Table of Contents](#table-of-contents)
- [Build on GNU/Linux Host](#build-on-linux-host)
  - [Dependencies](#dependencies)
  - [Install Pre-Requirements](#install-pre-requirement)
  - [Build to run on host](#build-to-run-on-host)
- [Run Tests](#run-tests)
- [Usage](#usage)
- [Contribution](#contribution)
- [Acknowledgments](#acknowledgments)
- [License](#license)

# Build on Linux Host

## Dependencies

- [C++11 compiler](https://github.com/nlohmann/json#supported-compilers)
- [CMake](http://cmake.org)
- [libcurl](https://curl.haxx.se/libcurl)
- [JSON for Modern C++](https://github.com/nlohmann/json)

## Install Pre-Requirements

To generate Makefiles by using cmake and manage dependencies of this repository, certain packages must be installed on the host before compilation.

```shell
$ sudo apt-get update
$ sudo apt-get install -f build-essential libcurl-dev cmake git
```

Download this repository using Git:

```shell
$ git clone https://github.com/elastos/Elastos.NET.Hive.Native.SDK
```

## Build to run on host (Ubuntu / Debian / Linux)

To compile the project from source code for the target to run on Ubuntu / Debian / Linux, carry out the following steps:

Open a new terminal window.

Navigate to the previously downloaded folder that contains the source code of this repository.

```shell
$ cd YOUR-PATH/Elastos.NET.Hive.Native.SDK
```

Create a new folder, then change to directory.

```shell
$ mkdir build
$ cd build
```

Generate the Makefile in the current directory:

```shell
$ cmake ..
$ make
```

To be able to build a distribution and install it to the customized location, run the following commands:

```shell
$ cmake -DCMAKE_INSTALL_PREFIX=YOUR-INSTALL-PATH ..
$ make install
```
# Run Tests

After installation, simply enter commands to run tests:

```shell
$ cd YOUR-INSTALL-PATH/bin
$ make test
```

## Usage

Here is a simple example to use C++ APIs:

```c++
#include <iostream>
#include <string>
#include <ipfs/node.h>

int main(int, char**) {
  ipfs::Json userInfo;
  std::string uid = "uid-132fdc89-4e85-47f5-b942-fd7ba515a6f3";

  ipfs::Node node("localhost", 9095);
  node.UidInfo(uid, &userInfo);

  std::cout << "uid info: " << userInfo["UID"] << " => " << userInfo["PeerID"]
            << std::endl;
}
```

Then run the following command to compile it.

```sh
c++ -std=c++11 -I/path/to/header -L/path/to/lib -lhive-api myprog.cc -o myprog
```

## Contributions

We welcome any contribution to Elastos Hive projects.

## Acknowledgments
A sincere thank you to all teams and projects that we rely on directly or indirectly.

## License
This project is licensed under the terms of the [MIT license](https://github.com/elastos/Elastos.NET.Hive.Native.SDK/blob/dev-master/LICENSE.MIT).
