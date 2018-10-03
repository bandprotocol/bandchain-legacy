<div align="center">
  <h1>
    Band Chain
  </h1>

  <p>
    <strong>Blockchain Platform for Decentralize Data Curation</strong>

![C++17](https://img.shields.io/badge/language-C%2B%2B17-orange.svg?longCache=true&style=popout-square)
![Tendermint](https://img.shields.io/badge/consensus-Tendermint-blue.svg?longCache=true&style=popout-square)
![Apache-2.0](https://img.shields.io/badge/license-Apache--2.0-green.svg?longCache=true&style=popout-square)
  </p>
</div>

Band Chain is a decentralized, permissionless blockchain implementation based
on Band Protocol for decentralised data curation. We incentivise trusted, high-quality data sharing in online community via token staking and bonding curve mechanism. See https://bandprotocol.com for more info.

Build
-----

If you use [Docker](https://www.docker.com/), you can create a docker image using
the following command.

```
docker build -t band/buildenv .
```

Particularly if you are residing in non-us region, we highly encourage you to provide 
your country code so docker uses the closest mirror to install dependencies.

```
docker build --build-arg country_code=uk -t band/buildenv .
```

Inside the container, Band Protocol uses [CMake](https://cmake.org/) to facilitate 
the build process. The binaries will be located under the same `build` directory.

```
docker run --rm -v `pwd`:/bandprotocol -it band/buildenv bash
$ cd bandprotocol
$ protoc --cpp_out=. abci/abci.proto
$ mkdir build
$ cd build
$ cmake ..
$ make
```

Alternatively, to build locally, Band Protocol requires the following dependencies.
Be sure to have them available in your local system (versions as listed in 
[CMakeLists.txt](CMakeLists.txt)).

- [Boost](https://www.boost.org/)
- [ProtoBuf](https://developers.google.com/protocol-buffers/)
- [libsodium](https://github.com/jedisct1/libsodium)
- [CxxTest](https://cxxtest.com/)

#### Install rocksdb.

* **OS X**:
    * Install via [homebrew](http://brew.sh/).
    * run `brew install rocksdb`

* **Linux - Ubuntu**
    * Install Dependencies
        ```bash
        apt-get update
        apt-get -y install build-essential libgflags-dev libsnappy-dev zlib1g-dev libbz2-dev liblz4-dev libzstd-dev
        ```
    * Install rocksdb by source code:
        ```bash
        git clone https://github.com/facebook/rocksdb.git
        cd rocksdb & make shared_lib && make install-shared
        ```
* **Linux - Centos**
    * Install Dependencies
        ```bash
        yum -y install epel-release && yum -y update
        yum -y install gflags-devel snappy-devel zlib-devel bzip2-devel gcc-c++  libstdc++-devel
        ```
    * Install rocksdb by source code:
        ```bash
        git clone https://github.com/facebook/rocksdb.git
        cd rocksdb & make shared_lib && make install-shared
        ```

Then follow the normal `cmake` process as shown above. 

License
-------

Band Protocol is released under the terms of Apache License 2.0. See
[LICENSE](LICENSE) for more information or see
http://www.apache.org/licenses/LICENSE-2.0.
