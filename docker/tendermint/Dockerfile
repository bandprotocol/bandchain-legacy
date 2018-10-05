FROM ubuntu:18.04
MAINTAINER Band Protocol <dev@bandprotocol.com>

EXPOSE 26656 26657 26658

# Use your best mirror
ARG country_code=us
RUN sed --in-place --regexp-extended "s/(\/\/)(archive\.ubuntu)/\1${country_code}.\2/" /etc/apt/sources.list

RUN cd /usr/local/bin

RUN apt-get update
RUN apt-get upgrade -y
RUN apt-get install -y unzip wget

RUN wget https://github.com/tendermint/tendermint/releases/download/v0.22.3/tendermint_0.22.3_linux_amd64.zip
RUN unzip tendermint_0.22.3_linux_amd64.zip tendermint

RUN ./tendermint init
CMD ./tendermint node
