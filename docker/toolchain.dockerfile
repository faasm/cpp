FROM ubuntu:20.04

RUN apt update
RUN apt install -y software-properties-common
RUN apt update 
RUN apt upgrade -y

RUN apt install -y \
   autoconf \
   clang-10 \
   clang-format-10 \
   build-essential \
   git \
   ninja-build \
   python3-dev \
   python3-pip \
   pkg-config \
   wget

# Python deps
RUN pip3 install black

# Up-to-date CMake
RUN apt remove --purge --auto-remove cmake
WORKDIR /setup
RUN wget -q -O cmake-linux.sh https://github.com/Kitware/CMake/releases/download/v3.18.2/cmake-3.18.2-Linux-x86_64.sh
RUN sh cmake-linux.sh -- --skip-license --prefix=/usr/local

# Run the build from the master branch
WORKDIR /code
RUN git clone https://github.com/faasm/faasm-toolchain/
WORKDIR /code/faasm-toolchain
RUN git submodule update --init --recursive
RUN make

# Tidy up
WORKDIR /
RUN rm -r /setup
RUN apt-get clean autoclean
RUN apt-get autoremove

CMD /bin/bash

