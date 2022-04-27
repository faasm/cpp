FROM ubuntu:20.04

RUN apt update

RUN apt-get install -y software-properties-common gpg wget curl

RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key|apt-key add -
RUN add-apt-repository -y -n "deb http://apt.llvm.org/focal/ llvm-toolchain-focal-13 main"
RUN add-apt-repository -y -n ppa:ubuntu-toolchain-r/test

RUN apt update

# Clang is needed here to build bits of LLVM, but we need to have 13
RUN apt install -y \
   autoconf \
   build-essential \
   clang-13 \
   git \
   ninja-build \
   pkg-config \
   wget

# Up-to-date CMake
RUN apt remove --purge --auto-remove cmake
WORKDIR /setup
RUN wget -q -O cmake-linux.sh https://github.com/Kitware/CMake/releases/download/v3.18.2/cmake-3.18.2-Linux-x86_64.sh
RUN sh cmake-linux.sh -- --skip-license --prefix=/usr/local

# Tidy up
RUN apt-get clean autoclean
RUN apt-get autoremove

# Copy the code in
WORKDIR /code
RUN git clone --depth 1 -b simd https://github.com/faasm/cpp
WORKDIR /code/cpp
RUN git submodule update --init third-party/llvm-project
RUN git submodule update --init third-party/wasi-libc

# Run the main make
RUN make

# Print the clang version
RUN /usr/local/faasm/toolchain/bin/clang --version

# Remove the code
WORKDIR /
RUN rm -r /code

CMD /bin/bash

