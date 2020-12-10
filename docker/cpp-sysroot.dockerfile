FROM faasm/llvm:10.0.1 as llvm

FROM faasm/grpc-root:0.0.16
ARG SYSROOT_VERSION

# Copy the toolchain in from the LLVM container
COPY --from=llvm /usr/local/faasm /usr/local/faasm

RUN apt install -y \
    autoconf \
    autotools-dev \
    libtool \
    python3-dev \
    python3-venv \
    python3-pip

# Get the code
WORKDIR /code
RUN git clone -b v${SYSROOT_VERSION} https://github.com/faasm/faasm-toolchain
WORKDIR /code/faasm-toolchain

# Update submodules (not LLVM)
RUN git submodule update --init -f third-party/eigen
RUN git submodule update --init -f third-party/faabric
RUN git submodule update --init -f third-party/faasm-clapack
RUN git submodule update --init -f third-party/libffi
RUN git submodule update --init -f third-party/wasi-libc

# Install the faasmtools Python lib
RUN pip3 install -r requirements.txt
RUN pip3 install .

# ---------------------------------
# NATIVE
# ---------------------------------

# Install eigen
RUN inv eigen --native

# CPP emulator static build
ENV LD_LIBRARY_PATH=/usr/local/lib
RUN inv dev.cmake
RUN inv dev.cc emulator
RUN inv dev.install emulator

# Native static libraries
RUN inv libfaasm --native
RUN inv libfaasmp --native
RUN inv libfaasmpi --native

# CPP emulator shared build
RUN inv dev.cmake --shared
RUN inv dev.cc emulator --shared
RUN inv dev.install emulator --shared

# Native shared libraries
RUN inv libfaasm --native --shared
RUN inv libfaasmp --native --shared
RUN inv libfaasmpi --native --shared

# ---------------------------------
# WASM
# ---------------------------------

# Install toolchain files
RUN inv install

# Libraries
RUN inv libc
RUN inv libffi
RUN inv eigen

# Both static and shared clapack
RUN inv clapack
RUN inv clapack --clean --shared

# Faasm libraries to wasm
RUN inv libfaasm
RUN inv libfaasmp
RUN inv libfaasmpi
