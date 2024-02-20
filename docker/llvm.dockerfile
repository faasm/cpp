FROM ubuntu:22.04

SHELL ["/bin/bash", "-c"]
ENV CPP_DOCKER="on"

# Configure APT repositories
ARG LLVM_VERSION_MAJOR
RUN apt update \
    && apt upgrade -y \
    && apt install -y \
        curl \
        gpg \
        software-properties-common \
        wget \
    && wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key | tee /etc/apt/trusted.gpg.d/apt.llvm.org.asc \
    && add-apt-repository -y "deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy-${LLVM_VERSION_MAJOR} main"

# Install APT dependencies
ARG DEBIAN_FRONTEND=noninteractive
RUN apt update \
    && apt install -y \
        autoconf \
        build-essential \
        # This clang version is needed to help us build LLVM from source below.
        # Pin it to one number as it can drift away from the WASM-compiling
        # LLVM version
        clang-17 \
        curl \
        git \
        gpg \
        ninja-build \
        pkg-config \
        python3-dev \
        python3-pip \
        python3-venv \
        software-properties-common \
        wget

# Install up-to-date CMake
RUN apt remove --purge --auto-remove cmake \
    && mkdir -p /setup \
    && cd /setup \
    && wget -q -O cmake-linux.sh \
        https://github.com/Kitware/CMake/releases/download/v3.28.0/cmake-3.28.0-linux-x86_64.sh \
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
    && git submodule update --init -f third-party/wasi-libc \
    && ./bin/create_venv.sh \
    && source venv/bin/activate \
    && inv install \
    # This task builds LLVM from source and reads the version from faasmtools/env.py
    && inv llvm.build \
    && /usr/local/faasm/toolchain/bin/clang --version \
    && rm -rf /code
