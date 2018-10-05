FROM ubuntu:18.04
MAINTAINER Band Protocol <dev@bandprotocol.com>

# Use your best mirror
ARG country_code=us
RUN sed --in-place --regexp-extended "s/(\/\/)(archive\.ubuntu)/\1${country_code}.\2/" /etc/apt/sources.list

RUN cd ~

RUN apt-get update
RUN apt-get upgrade -y
RUN apt-get install -y build-essential
RUN apt-get install -y autoconf automake curl libbz2-dev libtool wget vim unzip
RUN apt-get install -y cxxtest
RUN apt-get install -y libboost-all-dev
RUN apt-get install -y protobuf-compiler
RUN apt-get install -y libprotobuf-dev
RUN apt-get install -y cmake
RUN apt-get install -y libsodium-dev

RUN apt-get install -y git libgflags-dev libsnappy-dev zlib1g-dev liblz4-dev libzstd-dev
RUN git clone https://github.com/facebook/rocksdb.git
RUN cd rocksdb && make shared_lib && make install-shared && cd ~

# Build & Run Band Chain on Container Run
CMD cd bandprotocol && protoc --cpp_out=. abci/abci.proto && mkdir -p build && cd build && cmake .. && make -j4 && ./band -v