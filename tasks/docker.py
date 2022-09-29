from invoke import task

from os.path import join

from faasmtools.docker import (
    build_container,
    push_container,
)

from faasmtools.env import (
    get_version,
    PROJ_ROOT,
    LLVM_VERSION,
)

LLVM_IMAGE_NAME = "faasm/llvm"
LLVM_DOCKERFILE = join(PROJ_ROOT, "docker", "llvm.dockerfile")
SYSROOT_IMAGE_NAME = "faasm/cpp-sysroot"
SYSROOT_DOCKERFILE = join(PROJ_ROOT, "docker", "cpp-sysroot.dockerfile")


def get_sysroot_tag():
    version = get_version()
    return "{}:{}".format(SYSROOT_IMAGE_NAME, version)


def get_llvm_tag():
    return "{}:{}".format(LLVM_IMAGE_NAME, LLVM_VERSION)


@task(iterable=["c"])
def build(ctx, c, nocache=False, push=False):
    """
    Build container images. Possible images are: `llvm`, and `cpp-sysroot`.
    """
    build_args = {"SYSROOT_VERSION": get_version()}
    for ctr in c:
        if ctr == "llvm":
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
