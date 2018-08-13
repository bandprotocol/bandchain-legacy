FROM ubuntu:18.04
MAINTAINER Band Protocol <dev@bandprotocol.com>

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
