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

# Running with Docker

### Getting the containers up and running

The most convenient way to run Band Chain is to use Docker. Under `docker` directory, you'll find the `docker-compose.yaml` file which defines the two containers needed to run the blockchain. Make sure you have [Docker](https://www.docker.com/) installed, then run:

```sh
$> cd docker
$> docker-compose up -d
```

The `-d` tag signals `docker-compose` to run the containers in _detached mode_. This allows us to observe the logs from both **`bandchain`** and **`tendermint`** container individually.

> Note that you can change ubuntu's apt-get mirror by
> changing the country_code in the docker-compose.yaml file

### Attaching to the containers

At this point the containers should be up and running. To observe each container, simply attach to it by using the command:

```sh
$> docker attach bandchain
   # OR
$> docker attach tendermint
```

To detach yourself back, use `Ctrl+P Ctrl+Q` key combination.

### Stopping the containers

Under `docker` directory, run:

```sh
$> docker-compose down
```

# Running on Local Machine

### Installing Dependencies

To build locally, Band Chain requires the following dependencies.
Be sure to have them available in your local system (versions as listed in
[CMakeLists.txt](CMakeLists.txt)).

- [Boost](https://www.boost.org/)
- [ProtoBuf](https://developers.google.com/protocol-buffers/)
- [libsodium](https://github.com/jedisct1/libsodium)
- [CxxTest](https://cxxtest.com/)
- [RocksDB](https://rocksdb.org/)

#### Installing RocksDB

- **OS X**:

  - Install via [homebrew](http://brew.sh/).
  - run `brew install rocksdb`

- **Linux - Ubuntu**
  - Install Dependencies
    ```bash
    apt-get update
    apt-get -y install build-essential libgflags-dev libsnappy-dev zlib1g-dev libbz2-dev liblz4-dev libzstd-dev
    ```
  - Install rocksdb by source code:
    ```bash
    git clone https://github.com/facebook/rocksdb.git
    cd rocksdb & make shared_lib && make install-shared
    ```

### Building the Source Code

Starting off, you'll have to configure ProtoBuff and cmake by running:

```sh
$> protoc --cpp_out=. abci/abci.proto
$> mkdir build
$> cd build
$> cmake ..
```

Then to compile, under the same `build` directory, run:

```sh
$> make
```

### Running the node

To run the node, make sure you have [Tendermint v0.22.3](https://github.com/tendermint/tendermint/releases/tag/v0.22.3) on your machine, then start it with:

```sh
$> tendermint init # First time only
$> tendermint node
```

Note that you need to keep the Tendermint session running at all time. `tmux` and similar tools are handy for this purpose.

Then we can run the core Band Chain logic via:

```sh
$> cd build
$> ./band -v
```

# Basic Usage

Typically all the commands to Band Chain are sent through Tendermint ABCI, which is exposed on `http://localhost:26657`.

The transactions need to be generated in a specific way. Please check out the [🛠️ Official Python Client](https://github.com/bandprotocol/python-client) and [🛠️ Official Javascript Client](https://github.com/bandprotocol/js-client) to start interacting with the blockchain.

# License

Band Protocol is released under the terms of Apache License 2.0. See
[LICENSE](LICENSE) for more information or see
http://www.apache.org/licenses/LICENSE-2.0.
