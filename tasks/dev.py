from os import makedirs
from os.path import exists
from subprocess import run
from shutil import rmtree

from invoke import task

from faasmtools.env import PROJ_ROOT
from faasmtools.build import FAASM_NATIVE_DIR

BUILD_DIR = "/build/faasm-toolchain"


@task
def cmake(ctx, clean=False, build="Debug"):
    """
    Configures the CMake build
    """
    if clean and exists(BUILD_DIR):
        rmtree(BUILD_DIR)

    if not exists(BUILD_DIR):
        makedirs(BUILD_DIR)

    cmd = [
        "cmake",
        "-GNinja",
        "-DCMAKE_BUILD_TYPE={}".format(build),
        "-DCMAKE_CXX_COMPILER=/usr/bin/clang++-10",
        "-DCMAKE_C_COMPILER=/usr/bin/clang-10",
        "-DCMAKE_INSTALL_PREFIX={}".format(FAASM_NATIVE_DIR),
        PROJ_ROOT,
    ]

    cmd_str = " ".join(cmd)
    print(cmd_str)
    run(cmd_str, shell=True, check=True, cwd=BUILD_DIR)


@task
def cc(ctx, target, clean=False):
    """
    Compiles the given CMake target
    """
    if clean:
        cmake(ctx, clean=True)

    if target == "all":
        target = ""
    else:
        target = "--target {}".format(target)

    run(
        "cmake --build . {}".format(target),
        cwd=BUILD_DIR,
        shell=True,
        check=True,
    )


@task
def install(ctx, target):
    """
    Installs the given target
    """
    run(
        "ninja install {}".format(target),
        cwd=BUILD_DIR,
        shell=True,
        check=True,
    )
