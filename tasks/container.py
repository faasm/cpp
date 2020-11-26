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


@task
def llvm(ctx, nocache=False, push=False):
    """
    Build current version of the llvm container
    """
    build_container(
        get_llvm_tag(),
        LLVM_DOCKERFILE,
        PROJ_ROOT,
        nocache=nocache,
        push=push,
    )


@task
def push_llvm(ctx):
    """
    Push the current version of the llvm container
    """
    push_container(get_llvm_tag())


@task
def sysroot(ctx, nocache=False, push=False):
    """
    Build current version of the sysroot container
    """
    build_container(
        get_sysroot_tag(),
        SYSROOT_DOCKERFILE,
        PROJ_ROOT,
        nocache=nocache,
        push=push,
        build_args={"SYSROOT_VERSION": get_version()},
    )


@task
def push_sysroot(ctx):
    """
    Push the current version of the sysroot container
    """
    push_container(get_sysroot_tag())
