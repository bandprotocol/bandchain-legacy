apt-get update
apt-get upgrade -y
apt-get install -y build-essential
apt-get install -y autoconf automake curl libbz2-dev libtool wget vim unzip
apt-get install -y cxxtest
apt-get install -y libboost-all-dev
apt-get install -y protobuf-compiler
apt-get install -y libprotobuf-dev
apt-get install -y cmake
apt-get install -y libsodium-dev
apt-get install -y python3-venv

cd ~

apt-get install -y git libgflags-dev libsnappy-dev zlib1g-dev liblz4-dev libzstd-dev
git clone https://github.com/facebook/rocksdb.git
cd rocksdb && make shared_lib && make install-shared && cd ~

cd bandchain
protoc --cpp_out=. abci/abci.proto
mkdir -p build && cd build
cmake ..
make

cd ~
git clone https://github.com/bandprotocol/band-build.git

cd bandchain/script
python3 -m venv venv
source venv/bin/activate
pip install toml