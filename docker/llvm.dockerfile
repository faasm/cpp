FROM ubuntu:20.04

SHELL ["/bin/bash", "-c"]

# Install APT dependencies
ARG DEBIAN_FRONTEND=noninteractive
RUN apt update \
    && apt install -y \
        curl \
        gpg \
        software-properties-common \
        wget \
    && wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - \
    && add-apt-repository -y -n "deb http://apt.llvm.org/focal/ llvm-toolchain-focal-13 main" \
    && add-apt-repository -y -n ppa:ubuntu-toolchain-r/test \
    && apt update \
    && apt upgrade -y \
    && apt install -y \
        autoconf \
        clang-13 \
        build-essential \
        git \
        ninja-build \
        pkg-config \
        python3-dev \
        python3-pip \
        python3-venv

# Install up-to-date CMake
RUN apt remove --purge --auto-remove cmake \
    && mkdir -p /setup \
    && cd /setup \
    && wget -q -O cmake-linux.sh \
        https://github.com/Kitware/CMake/releases/download/v3.24.2/cmake-3.24.2-linux-x86_64.sh \
    && sh cmake-linux.sh -- --skip-license --prefix=/usr/local \
    && apt clean autoclean -y \
    && apt autoremove -y

# Get the code, build the main targets, and remove the code
ARG SYSROOT_VERSION
RUN mkdir -p /code \
    && git clone -b v${SYSROOT_VERSION} \
        https://github.com/faasm/cpp \
        /code/cpp \
    && cd /code/cpp \
    && git submodule update --init -f third-party/llvm-project \
    && git submodule update --init -f third-party/wasi-libc \
    && ./bin/create_venv.sh \
    && source venv/bin/activate \
    && inv llvm.build \
    && /usr/local/faasm/toolchain/bin/clang --version \
    && rm -rf /code
