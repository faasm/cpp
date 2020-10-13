from os.path import dirname, abspath, join
from multiprocessing import cpu_count
import yaml

PROJ_ROOT = dirname(dirname(abspath(__file__)))
THIRD_PARTY_DIR = join(PROJ_ROOT, "third-party")

VERSIONS_FILE = join(PROJ_ROOT, "versions.yml")

# Docker
TOOLCHAIN_IMAGE_NAME = "faasm/toolchain"
TOOLCHAIN_DOCKERFILE = join(PROJ_ROOT, "docker", "toolchain.dockerfile")
SYSROOT_IMAGE_NAME = "faasm/sysroot"
SYSROOT_DOCKERFILE = join(PROJ_ROOT, "docker", "sysroot.dockerfile")

# Environment
USABLE_CPUS = int(cpu_count()) - 1


def _get_version_yaml_value(key):
    with open(VERSIONS_FILE) as fh:
        versions = yaml.load(fh)

    return versions[key]


def get_version():
    return _get_version_yaml_value("toolchain")


def get_llvm_version():
    return _get_version_yaml_value("llvm")
