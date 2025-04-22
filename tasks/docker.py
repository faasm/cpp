from faasmtools.docker import (
    build_container,
    push_container,
    CR_NAME,
)
from faasmtools.env import (
    LLVM_VERSION,
    PROJ_ROOT,
    get_version,
)
from invoke import task
from os.path import join

LLVM_IMAGE_NAME = "{}/llvm".format(CR_NAME)
LLVM_DOCKERFILE = join(PROJ_ROOT, "docker", "llvm.dockerfile")
SYSROOT_IMAGE_NAME = "{}/cpp-sysroot".format(CR_NAME)
SYSROOT_DOCKERFILE = join(PROJ_ROOT, "docker", "cpp-sysroot.dockerfile")


def get_sysroot_tag():
    version = get_version()
    return "{}:{}".format(SYSROOT_IMAGE_NAME, version)


def get_llvm_tag():
    version = get_version()
    return "{}:{}".format(LLVM_IMAGE_NAME, version)


@task(iterable=["c"])
def build(ctx, c, nocache=False, push=False):
    """
    Build container images. Possible images are: `llvm`, and `cpp-sysroot`.
    """
    build_args = {"SYSROOT_VERSION": get_version()}
    for ctr in c:
        if ctr == "llvm":
            build_args["LLVM_VERSION_MAJOR"] = LLVM_VERSION.split(".")[0]
            dockerfile = LLVM_DOCKERFILE
            tag = get_llvm_tag()
        elif ctr == "cpp-sysroot":
            dockerfile = SYSROOT_DOCKERFILE
            tag = get_sysroot_tag()
        else:
            print("Unrecognised container image: {}".format(ctr))
            raise RuntimeError("Unrecognised container image")

        build_container(
            tag,
            dockerfile,
            PROJ_ROOT,
            nocache=nocache,
            push=push,
            build_args=build_args,
        )


@task(iterable=["c"])
def push(ctx, c):
    """
    Push container images. Possible images are: `llvm`, and `cpp-sysroot`.
    """
    for ctr in c:
        if ctr == "llvm":
            tag = get_llvm_tag()
        elif ctr == "cpp-sysroot":
            tag = get_sysroot_tag()
        else:
            print("Unrecognised container image: {}".format(ctr))
            raise RuntimeError("Unrecognised container image")

        push_container(tag)
