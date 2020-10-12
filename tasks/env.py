from os.path import dirname, abspath, join
from multiprocessing import cpu_count

PROJ_ROOT = dirname(dirname(abspath(__file__)))

VERSION_FILE = join(PROJ_ROOT, "VERSION")

TOOLCHAIN_IMAGE_NAME = "faasm/toolchain"
TOOLCHAIN_DOCKERFILE = join(PROJ_ROOT, "docker", "toolchain.dockerfile")
SYSROOT_IMAGE_NAME = "faasm/sysroot"
SYSROOT_DOCKERFILE = join(PROJ_ROOT, "docker", "sysroot.dockerfile")

USABLE_CPUS = int(cpu_count()) - 1

TOOLCHAIN_BIN = "/usr/local/faasm/toolchain/bin"
SYSROOT = "/usr/local/faasm/llvm-sysroot"

WASM_CC = join(TOOLCHAIN_BIN, "clang")
WASM_AR = join(TOOLCHAIN_BIN, "llvm-ar")
WASM_NM = join(TOOLCHAIN_BIN, "llvm-nm")


def get_version():
    with open(VERSION_FILE) as fh:
        ver_str = fh.read()

    ver_str = ver_str.strip()
    return ver_str
