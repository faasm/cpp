ARG TOOLCHAIN_VERSION
FROM faasm/toolchain:${TOOLCHAIN_VERSION}
ARG TOOLCHAIN_VERSION

# Update existing code checkout to latest tag
WORKDIR /code/faasm-toolchain
RUN git fetch --all
RUN git checkout v${TOOLCHAIN_VERSION}
RUN git submodule update --init --recursive

# Rebuild libc
RUN make clean-libc
RUN make

