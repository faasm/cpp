from subprocess import run

from invoke import task

from faasmtools.env import (
    get_version,
    get_llvm_version,
    TOOLCHAIN_IMAGE_NAME,
    PROJ_ROOT,
    SYSROOT_IMAGE_NAME,
    TOOLCHAIN_DOCKERFILE,
    SYSROOT_DOCKERFILE,
)


def _do_container_build(tag_name, dockerfile, nocache=False, push=False):
    if nocache:
        no_cache_str = "--no-cache"
    else:
        no_cache_str = ""

    build_cmd = [
        "docker build",
        no_cache_str,
        "-t {}".format(tag_name),
        "-f {}".format(dockerfile),
        ".",
    ]
    build_cmd = " ".join(build_cmd)

    print(build_cmd)
    run(
        build_cmd,
        shell=True,
        check=True,
        env={"DOCKER_BUILDKIT": "1"},
        cwd=PROJ_ROOT,
    )

    if push:
        _do_container_push(tag_name)


def _do_container_push(tag_name):
    """
    Push current version of container image
    """
    cmd = "docker push {}".format(tag_name)

    print(cmd)
    run(
        cmd,
        shell=True,
        check=True,
        cwd=PROJ_ROOT,
    )


@task
def toolchain(ctx, nocache=False, push=False):
    """
    Build current version of the toolchain container
    """
    llvm_version = get_llvm_version()
    tag_name = "{}:{}".format(TOOLCHAIN_IMAGE_NAME, llvm_version)
    _do_container_build(
        tag_name, TOOLCHAIN_DOCKERFILE, nocache=nocache, push=push
    )


@task
def push_toolchain(ctx):
    """
    Push the current version of the toolchain container
    """
    llvm_version = get_llvm_version()
    tag_name = "{}:{}".format(TOOLCHAIN_IMAGE_NAME, llvm_version)
    _do_container_push(tag_name)


@task
def sysroot(ctx, nocache=False, push=False):
    """
    Build current version of the sysroot container
    """
    toolchain_version = get_version()
    tag_name = "{}:{}".format(SYSROOT_IMAGE_NAME, toolchain_version)
    _do_container_build(
        tag_name, SYSROOT_DOCKERFILE, nocache=nocache, push=push
    )


@task
def push_sysroot(ctx):
    """
    Push the current version of the sysroot container
    """
    toolchain_version = get_version()
    tag_name = "{}:{}".format(SYSROOT_IMAGE_NAME, toolchain_version)
    _do_container_push(tag_name)
