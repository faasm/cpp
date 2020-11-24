from os.path import join, exists
from subprocess import run
from shutil import rmtree

from faasmtools.build import (
    build_config_cmd,
    run_autotools,
    WASM_SYSROOT,
    WASM_LIB_INSTALL,
    CMAKE_TOOLCHAIN_FILE,
)

from invoke import task

from faasmtools.env import THIRD_PARTY_DIR, USABLE_CPUS, PROJ_ROOT


@task(default=True)
def build(ctx, clean=False):
    """
    Builds faasm C/C++ lib
    """
    work_dir = join(PROJ_ROOT, "libfaasm")
    build_dir = join(work_dir, "build")

    if exists(build_dir):
        rmtree(build_dir)

    build_cmd = [
        "cmake",
        "-GNinja",
        "-DCMAKE_BUILD_TYPE=Release",
        "-DCMAKE_TOOLCHAIN_FILE={}".format(CMAKE_TOOLCHAIN_FILE),
        work_dir,
    ]

    build_cmd_str = " ".join(build_cmd)
    print(build_cmd_str)

    run(build_cmd_str, shell=True, cwd=build_dir, check=True)

    run("ninja", shell=True, cwd=build_dir, check=True)
    run("ninja install", shell=True, cwd=build_dir, check=True)
