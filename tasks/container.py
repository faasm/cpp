from invoke import task

from faasmtools.docker import (
    build_container,
    push_container,
)

from faasmtools.env import (
    get_toolchain_tag,
    get_sysroot_tag,
    PROJ_ROOT,
    TOOLCHAIN_DOCKERFILE,
    SYSROOT_DOCKERFILE,
)


@task
def toolchain(ctx, nocache=False, push=False):
    """
    Build current version of the toolchain container
    """
    build_container(
        get_toolchain_tag(),
        TOOLCHAIN_DOCKERFILE,
        PROJ_ROOT,
        nocache=nocache,
        push=push,
    )


@task
def push_toolchain(ctx):
    """
    Push the current version of the toolchain container
    """
    push_container(get_toolchain_tag())


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
    )


@task
def push_sysroot(ctx):
    """
    Push the current version of the sysroot container
    """
    push_container(get_sysroot_tag())
