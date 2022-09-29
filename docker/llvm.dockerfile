FROM ubuntu:22.04

# Clang-10 is not listed in ubuntu 22.04's base APT repositories, so we need
# to fetch it from 22.04's. Once we upgrade to a newer clang for WASM we can
# get rid of this step.
RUN apt update \
    && apt upgrade -y \
    && apt install -y \
        curl \
        gpg \
        software-properties-common \
        wget \
    && wget -O /tmp/llvm-snapshot.gpg.key \
        https://apt.llvm.org/llvm-snapshot.gpg.key \
    && gpg --no-default-keyring \
        --keyring /tmp/tmp-key.gpg \
        --import /tmp/llvm-snapshot.gpg.key \
    && gpg --no-default-keyring \
        --keyring /tmp/tmp-key.gpg \
        --export --output /etc/apt/keyrings/llvm-snapshot.gpg \
    && rm /tmp/tmp-key.gpg \
    && echo \
        "deb [signed-by=/etc/apt/keyrings/llvm-snapshot.gpg] http://apt.llvm.org/focal/ llvm-toolchain-focal-10 main" \
        >> /etc/apt/sources.list.d/archive_uri-http_apt_llvm_org_focal_-jammy.list

# Install APT dependencies
RUN apt update \
    && apt install -y \
       autoconf \
       clang-10 \
       build-essential \
       git \
       ninja-build \
       pkg-config

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
    && make \
    && /usr/local/faasm/toolchain/bin/clang --version \
    && rm -rf /code
