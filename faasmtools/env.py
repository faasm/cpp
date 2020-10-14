from os.path import dirname, abspath, join
from multiprocessing import cpu_count

PROJ_ROOT = dirname(dirname(abspath(__file__)))
THIRD_PARTY_DIR = join(PROJ_ROOT, "third-party")

# Docker
TOOLCHAIN_IMAGE_NAME = "faasm/toolchain"
TOOLCHAIN_DOCKERFILE = join(PROJ_ROOT, "docker", "toolchain.dockerfile")
SYSROOT_IMAGE_NAME = "faasm/sysroot"
SYSROOT_DOCKERFILE = join(PROJ_ROOT, "docker", "sysroot.dockerfile")

# Environment
USABLE_CPUS = int(cpu_count()) - 1

# Versioning
VERSION_FILE = join(PROJ_ROOT, "VERSION")
LLVM_VERSION = "10.0.1"


def get_version():
    with open(VERSION_FILE) as fh:
        ver = fh.read()

    return ver.strip()


def get_sysroot_tag():
    version = get_version()
    return "{}:v{}".format(SYSROOT_IMAGE_NAME, version)


def get_toolchain_tag():
    return "{}:{}".format(TOOLCHAIN_IMAGE_NAME, LLVM_VERSION)
