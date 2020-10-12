from subprocess import run

from invoke import task

from tasks.env import (
    get_version,
    TOOLCHAIN_IMAGE_NAME,
    PROJ_ROOT,
    SYSROOT_IMAGE_NAME,
    TOOLCHAIN_DOCKERFILE,
    SYSROOT_DOCKERFILE,
)


def _do_container_build(img_name, dockerfile, nocache=False, push=False):
    this_version = get_version()

    tag_name = "{}:{}".format(img_name, this_version)

    if nocache:
        no_cache_str = "--no-cache"
    else:
        no_cache_str = ""

    build_cmd = [
        "docker build",
        no_cache_str,
        "-t {}".format(tag_name),
        "-f {}".format(dockerfile),
        "--build-arg TOOLCHAIN_VERSION={}".format(this_version),
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
        _do_container_push(img_name)


def _do_container_push(img_name):
    """
    Push current version of container image
    """
    this_version = get_version()

    cmd = "docker push {}:{}".format(img_name, this_version)

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
    _do_container_build(
        TOOLCHAIN_IMAGE_NAME, TOOLCHAIN_DOCKERFILE, nocache=nocache, push=push
    )


@task
def push_toolchain(ctx):
    """
    Push the current version of the toolchain container
    """
    _do_container_push(TOOLCHAIN_IMAGE_NAME)


@task
def sysroot(ctx, nocache=False, push=False):
    """
    Build current version of the sysroot container
    """
    _do_container_build(
        SYSROOT_IMAGE_NAME, SYSROOT_DOCKERFILE, nocache=nocache, push=push
    )


@task
def push_sysroot(ctx):
    """
    Push the current version of the sysroot container
    """
    _do_container_push(SYSROOT_IMAGE_NAME)
