from os import makedirs
from os.path import exists
from subprocess import run
from shutil import rmtree

from invoke import task

from faasmtools.env import PROJ_ROOT
from faasmtools.build import FAASM_NATIVE_DIR

# Slightly inefficient to have two builds, but we need to make sure all
# dependencies are also build as shared/ static accordingly
SHARED_BUILD_DIR = "/build/faasm-toolchain/shared"
STATIC_BUILD_DIR = "/build/faasm-toolchain/static"


@task
def cmake(ctx, clean=False, build="Debug", shared=False):
    """
    Configures the CMake build
    """
    build_dir = SHARED_BUILD_DIR if shared else STATIC_BUILD_DIR

    if clean and exists(build_dir):
        rmtree(build_dir)

    if not exists(build_dir):
        makedirs(build_dir)

    cmd = [
        "cmake",
        "-GNinja",
        "-DCMAKE_BUILD_TYPE={}".format(build),
        "-DCMAKE_CXX_COMPILER=/usr/bin/clang++-10",
        "-DCMAKE_C_COMPILER=/usr/bin/clang-10",
        "-DCMAKE_INSTALL_PREFIX={}".format(FAASM_NATIVE_DIR),
        "-DBUILD_SHARED_LIBS={}".format("ON" if shared else "OFF"),
        PROJ_ROOT,
    ]

    cmd_str = " ".join(cmd)
    print(cmd_str)
    run(cmd_str, shell=True, check=True, cwd=build_dir)


@task
def cc(ctx, target, clean=False, shared=False):
    """
    Compiles the given CMake target
    """
    build_dir = SHARED_BUILD_DIR if shared else STATIC_BUILD_DIR

    if clean:
        cmake(ctx, clean=True)

    if target == "all":
        target = ""
    else:
        target = "--target {}".format(target)

    run(
        "cmake --build . {}".format(target),
        cwd=build_dir,
        shell=True,
        check=True,
    )


@task
def install(ctx, target, shared=False):
    """
    Installs the given target
    """
    build_dir = SHARED_BUILD_DIR if shared else STATIC_BUILD_DIR

    run(
        "ninja install {}".format(target),
        cwd=build_dir,
        shell=True,
        check=True,
    )
