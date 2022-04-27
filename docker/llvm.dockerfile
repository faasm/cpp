FROM ubuntu:20.04

RUN apt update
RUN apt install -y software-properties-common
RUN apt update
RUN apt upgrade -y

# Clang is needed here to compile bits of LLVM, it is not the final version
# we'll use to compile wasm
RUN apt install -y \
   autoconf \
   clang-10 \
   build-essential \
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
COPY . .

# Run the main make
RUN make

# Print the clang version
RUN /usr/local/faasm/toolchain/bin/clang --version

# Remove the code
WORKDIR /
RUN rm -r /code

CMD /bin/bash

