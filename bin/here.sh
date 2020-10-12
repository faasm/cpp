#!/bin/bash

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"                  
TOOLCHAIN_ROOT=${SCRIPT_DIR}/..

WORK_DIR=$(pwd)
echo "Starting toolchain at ${WORK_DIR}"

VERSION=$(cat "${TOOLCHAIN_ROOT}/VERSION")
VERSION=0.0.3

docker run --entrypoint="/bin/bash" \
    --network="host" \
    -e "TERM=xterm-256color" \
    -v ${WORK_DIR}:/work \
    -w /work \
    -it \
    faasm/sysroot:${VERSION}

