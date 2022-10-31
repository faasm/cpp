FROM faasm/llvm:10.0.1 as llvm

# faabric-base image is not re-built often, so tag may be behind
FROM faasm/faabric-base:0.3.5
SHELL ["/bin/bash", "-c"]
ENV CPP_DOCKER="on"

# Copy the toolchain in from the LLVM container
COPY --from=llvm /usr/local/faasm /usr/local/faasm

# Update APT dependencies
RUN apt update && apt install -y autotools-dev
# Get the code and submodules
ARG SYSROOT_VERSION
RUN mkdir -p /code \
    && git clone -b v${SYSROOT_VERSION} \
        https://github.com/faasm/cpp \
        /code/cpp \
    && cd /code/cpp \
    # Update submodules (not LLVM)
    && git submodule update --init -f third-party/faabric \
    && git submodule update --init -f third-party/faasm-clapack \
    && git submodule update --init -f third-party/libffi \
    && git submodule update --init -f third-party/wasi-libc \
    && git submodule update --init -f third-party/FFmpeg \
    && git submodule update --init -f third-party/zlib \
    && git submodule update --init -f third-party/libpng \
    && git submodule update --init -f third-party/ImageMagick \
    && git submodule update --init -f third-party/tensorflow \
    && git submodule update --init -f third-party/lammps \
    && git submodule update --init -f third-party/LULESH

# Install the faasmtools Python lib
RUN cd /code/cpp \
    && ./bin/create_venv.sh \
    && source venv/bin/activate \
    && pip3 install -r requirements.txt \
    && pip3 install .

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
    # Build Faasm WASM libraries
    && inv \
        libemscripten \
        libfaasm \
        libfaasmp \
        libfaasmpi \
    # Lastly, build the ported third-pary WASM libraries and programs (libc
    # first as it is needed in the others)
    && inv \
        libc \
        clapack \
        clapack --clean --shared \
        ffmpeg \
        libffi \
        # To build imagemagick, we need to build zlib and libpng
        zlib \
        libpng \
        imagemagick \
        tensorflow \
        lammps \
        lulesh

# CLI setup
WORKDIR /code/cpp
ENV TERM xterm-256color

RUN echo ". /code/cpp/bin/workon.sh" >> ~/.bashrc
CMD ["/bin/bash", "-l"]
