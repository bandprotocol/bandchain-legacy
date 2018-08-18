Band Protocol
=============

What is Band Protocol?
----------------------

Band Protocol is a decentralized, permissionless blockchain protocol to create
tokenized marketplace and token-curated community. For more information,
see https://bandprotocol.com/.

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
Be sure to have them available in your local system (version as listed in 
[CMakeLists.txt](CMakeLists.txt)).

- [Boost](https://www.boost.org/)
- [ProtoBuf](https://developers.google.com/protocol-buffers/)
- [libsodium](https://github.com/jedisct1/libsodium)
- [CxxTest](https://cxxtest.com/)

Then follow the normal `cmake` process as shown above. 

License
-------

Band Protocol is released under the terms of Apache License 2.0. See
[LICENSE](LICENSE) for more information or see
http://www.apache.org/licenses/LICENSE-2.0.
