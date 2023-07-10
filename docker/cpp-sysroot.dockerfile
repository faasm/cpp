# llvm image is not re-built often, so the tag may be behind
FROM faasm.azurecr.io/llvm:0.2.4 as llvm

# faabric-base image is not re-built often, so tag may be behind
FROM faasm.azurecr.io/faabric-base:0.4.2
SHELL ["/bin/bash", "-c"]
ENV CPP_DOCKER="on"

# Copy the toolchain and LLVM sources from the LLVM container
COPY --from=llvm /usr/local/faasm /usr/local/faasm
COPY --from=llvm /opt/llvm-project /opt/llvm-project

# Update APT dependencies
RUN apt update && apt install -y autotools-dev
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
    # Build wasi-libc and reset the sysroot. The second call to LLVM just
    # installs some headers that are purged
    && inv llvm.libc --purge llvm \
    # Build Faasm WASM libraries
    && inv \
        libemscripten \
        libfaasm \
        libfaasmp \
        libfaasmpi \
    # Lastly, build the libraries that populate the sysroot
    && inv \
        clapack \
        clapack --clean --shared \
        libffi \
        zlib

# CLI setup
WORKDIR /code/cpp
ENV TERM xterm-256color

RUN echo ". /code/cpp/bin/workon.sh" >> ~/.bashrc
CMD ["/bin/bash", "-l"]
