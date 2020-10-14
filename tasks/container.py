from invoke import task

from faasmtools.docker import (
    build_container,
    push_container,
)

from faasmtools.env import (
    get_version,
    LLVM_VERSION,
    PROJ_ROOT,
    TOOLCHAIN_IMAGE_NAME,
    SYSROOT_IMAGE_NAME,
    TOOLCHAIN_DOCKERFILE,
    SYSROOT_DOCKERFILE,
)


@task
def toolchain(ctx, nocache=False, push=False):
    """
    Build current version of the toolchain container
    """
    tag_name = "{}:{}".format(TOOLCHAIN_IMAGE_NAME, LLVM_VERSION)
    build_container(
        tag_name, TOOLCHAIN_DOCKERFILE, PROJ_ROOT, nocache=nocache, push=push
    )


@task
def push_toolchain(ctx):
    """
    Push the current version of the toolchain container
    """
    tag_name = "{}:{}".format(TOOLCHAIN_IMAGE_NAME, LLVM_VERSION)
    push_container(tag_name)


@task
def sysroot(ctx, nocache=False, push=False):
    """
    Build current version of the sysroot container
    """
    toolchain_version = get_version()
    tag_name = "{}:{}".format(SYSROOT_IMAGE_NAME, toolchain_version)
    build_container(
        tag_name, SYSROOT_DOCKERFILE, PROJ_ROOT, nocache=nocache, push=push
    )


@task
def push_sysroot(ctx):
    """
    Push the current version of the sysroot container
    """
    toolchain_version = get_version()
    tag_name = "{}:{}".format(SYSROOT_IMAGE_NAME, toolchain_version)
    push_container(tag_name)
