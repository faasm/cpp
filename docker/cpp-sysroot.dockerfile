# llvm image is not re-built often, so the tag may be behind
FROM ghcr.io/faasm/llvm:0.7.0 AS llvm

# Start from a fresh ubuntu image, cpp-sysroot has little built deps.
FROM ubuntu:24.04

RUN set -eux; \
    apt update; \
    apt install -y --no-install-recommends \
        autoconf \
        automake \
        autopoint \
        autotools-dev \
        clang-17 \
        cmake \
        gettext \
        libtool \
        llvm-17 \
        m4 \
        ninja-build \
        make \
        pkg-config \
        python3-pip \
        python3-venv \
        git \
        vim-tiny; \
  apt autoremove -y; \
  apt clean; rm -rf /var/lib/apt/lists/*

SHELL ["/bin/bash", "-c"]
ENV CPP_DOCKER="on"

# Copy the toolchain and LLVM OpenMP sources necessary to build libfaasmp
COPY --from=llvm /usr/local/faasm /usr/local/faasm
COPY --from=llvm \
    /opt/llvm-project/build/llvm/projects/openmp/runtime/src \
    /opt/llvm-project/build/llvm/projects/openmp/runtime/src

# Get the code and submodules
ARG SYSROOT_VERSION
RUN mkdir -p /code \
    && git clone -b v${SYSROOT_VERSION} \
        https://github.com/faasm/cpp \
        /code/cpp \
    && cd /code/cpp \
    && git submodule update --init -f third-party/faabric \
    && git submodule update --init -f third-party/faasm-clapack \
    && git submodule update --init -f third-party/libffi \
    && git submodule update --init -f third-party/wasi-libc \
    && git submodule update --init -f third-party/zlib

# Python set-up
RUN cd /code/cpp \
    && ./bin/create_venv.sh

# Build all the targets
RUN cd /code/cpp \
    && source venv/bin/activate \
    # Build native Faasm libraries (static and shared)
    && inv \
        libfaasm --native \
        libfaasmp --native \
        libfaasmpi --native \
        libfaasm --native --shared \
        libfaasmp --native --shared \
        libfaasmpi --native --shared \
    # Install toolchain files
    && inv install \
    # Build Faasm WASM libraries for wasm32-wasi target
    && inv \
        libfaasm \
        libemscripten \
        libfaasmpi \
    # Build Faasm WASM libraries for wasm32-wasi-threads target
    && inv \
        libfaasm --threads \
        libemscripten --threads \
        libfaasmp \
    # Lastly, build the libraries that populate the sysroot. For some reason
    # we need to re-run apt update for libtool to work properly.
    && apt update \
    && inv \
        libffi \
        libffi --threads \
        zlib \
        zlib --threads \
    && apt autoremove -y \
    && apt clean; rm -rf /var/lib/apt/lists/*

# CLI setup
WORKDIR /code/cpp
ENV TERM=xterm-256color

RUN echo ". /code/cpp/bin/workon.sh" >> ~/.bashrc
CMD ["/bin/bash", "-l"]
