from subprocess import run

from os.path import join, exists
from os import makedirs
from shutil import rmtree

from invoke import task

from faasmtools.env import (
    THIRD_PARTY_DIR,
)

from faasmtools.build import (
    CMAKE_TOOLCHAIN_FILE,
    WASM_SYSROOT,
)


@task(default=True)
def eigen(ctx, verbose=False):
    """
    Compile and install Eigen
    """
    work_dir = join(THIRD_PARTY_DIR, "eigen")
    build_dir = join(work_dir, "build")

    if exists(build_dir):
        rmtree(build_dir)
    makedirs(build_dir)

    verbose_string = "VERBOSE=1" if verbose else ""

    cmd = [
        verbose_string,
        "cmake",
        "-GNinja",
        "-DCMAKE_TOOLCHAIN_FILE={}".format(CMAKE_TOOLCHAIN_FILE),
        "-DCMAKE_INSTALL_PREFIX={}".format(WASM_SYSROOT),
        work_dir,
    ]
    cmd_string = " ".join(cmd)

    run(cmd_string, shell=True, cwd=build_dir, check=True)

    run(
        "{} ninja install".format(verbose_string),
        shell=True,
        cwd=build_dir,
        check=True,
    )
