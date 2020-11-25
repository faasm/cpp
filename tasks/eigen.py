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
def eigen(ctx, verbose=False, native=False):
    """
    Compile and install Eigen
    """
    work_dir = join(THIRD_PARTY_DIR, "eigen")

    if native:
        build_dir = join(work_dir, "build", "native")
    else:
        build_dir = join(work_dir, "build", "wasm")

    if exists(build_dir):
        rmtree(build_dir)
    makedirs(build_dir)

    verbose_string = "VERBOSE=1" if verbose else ""

    cmake_cmd = [
        verbose_string,
        "cmake",
        "-GNinja",
    ]

    if not native:
        cmake_cmd.extend(
            [
                "-DCMAKE_TOOLCHAIN_FILE={}".format(CMAKE_TOOLCHAIN_FILE),
                "-DCMAKE_INSTALL_PREFIX={}".format(WASM_SYSROOT),
            ]
        )

    cmake_cmd.append(work_dir)

    cmake_cmd = " ".join(cmake_cmd)
    run(cmake_cmd, shell=True, cwd=build_dir, check=True)

    run(
        "{} ninja install".format(verbose_string),
        shell=True,
        cwd=build_dir,
        check=True,
    )
