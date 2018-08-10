Band Protocol
=============

What is Band Protocol?
----------------------

Band Protocol is a decentralized, permissionless blockchain protocol to create
tokenized marketplace and token-curated community. For more information,
see https://bandprotocol.com/.

Build
-----

Band Protocol requires the following dependencies. Be sure to have them
available in your local system.
- [Tendermint](https://tendermint.com/)
- [Boost](https://www.boost.org/)
- [ProtoBuf](https://developers.google.com/protocol-buffers/)
- [libsodium](https://github.com/jedisct1/libsodium)
- [spdlog](https://github.com/gabime/spdlog)
- [CxxTest](https://cxxtest.com/)

Band Protocol uses [CMake](https://cmake.org/) to facilitate the build process.

```
mkdir build
cd build
cmake ..
make
```

The binaries will be located in the same `build` directory.

License
-------

Band Protocol is released under the terms of Apache License 2.0. See
[LICENSE](LICENSE) for more information or see
http://www.apache.org/licenses/LICENSE-2.0.
