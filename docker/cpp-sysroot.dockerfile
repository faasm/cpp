FROM faasm/llvm:13.0.1 as llvm

# faabric-base image is not re-built often, so tag may be behind
FROM faasm/faabric-base:0.1.0
ARG SYSROOT_VERSION

# Copy the toolchain in from the LLVM container
COPY --from=llvm /usr/local/faasm /usr/local/faasm

RUN apt update
RUN apt install -y autotools-dev

# Get the code
WORKDIR /code
RUN git clone -b v${SYSROOT_VERSION} https://github.com/faasm/cpp
WORKDIR /code/cpp

# Update submodules (not LLVM)
RUN git submodule update --init -f third-party/faabric
RUN git submodule update --init -f third-party/faasm-clapack
RUN git submodule update --init -f third-party/libffi
RUN git submodule update --init -f third-party/wasi-libc
RUN git submodule update --init -f third-party/FFmpeg

# Install the faasmtools Python lib
RUN pip3 install -r requirements.txt
RUN pip3 install .

# ---------------------------------
# NATIVE
# ---------------------------------

# Native static libraries
RUN inv libfaasm --native
RUN inv libfaasmp --native
RUN inv libfaasmpi --native

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
RUN inv ffmpeg

# Both static and shared clapack
RUN inv clapack
RUN inv clapack --clean --shared

# Faasm libraries to wasm
RUN inv libfaasm
RUN inv libemscripten
RUN inv libfaasmp
RUN inv libfaasmpi

# CLI setup
ENV TERM xterm-256color
SHELL ["/bin/bash", "-c"]

RUN echo ". /code/cpp/bin/workon.sh" >> ~/.bashrc
CMD ["/bin/bash", "-l"]
