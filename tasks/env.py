from os.path import dirname, abspath, join

PROJ_ROOT = dirname(dirname(abspath(__file__)))

VERSION_FILE = join(PROJ_ROOT, "VERSION")

TOOLCHAIN_IMAGE_NAME = "faasm/toolchain"
TOOLCHAIN_DOCKERFILE = join(PROJ_ROOT, "docker", "toolchain.dockerfile")
SYSROOT_IMAGE_NAME = "faasm/sysroot"
SYSROOT_DOCKERFILE = join(PROJ_ROOT, "docker", "sysroot.dockerfile")


def get_version():
    with open(VERSION_FILE) as fh:
        ver_str = fh.read()

    ver_str = ver_str.strip()
    return ver_str
