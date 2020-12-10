from os import makedirs
from shutil import rmtree
from os.path import join, exists
from subprocess import run

from faasmtools.env import PROJ_ROOT

from invoke import task

EXAMPLES_DIR = join(PROJ_ROOT, "examples")
BUILD_DIR = join(EXAMPLES_DIR, "build")
EXAMPLE_TARGET = "native_exe"


@task(default=True)
def build(ctx, clean=False):
    """
    Builds the examples
    """
    if clean and exists(BUILD_DIR):
        rmtree(BUILD_DIR)

    if not exists(BUILD_DIR):
        makedirs(BUILD_DIR)

    # Cmake
    run(
        " ".join(
            [
                "cmake",
                "-GNinja",
                "-DCMAKE_BUILD_TYPE=Release",
                "-DCMAKE_CXX_COMPILER=/usr/bin/clang++-10",
                "-DCMAKE_C_COMPILER=/usr/bin/clang-10",
                EXAMPLES_DIR,
            ]
        ),
        shell=True,
        cwd=BUILD_DIR,
    )

    # Build
    run(
        "cmake --build . --target {}".format(EXAMPLE_TARGET),
        cwd=BUILD_DIR,
        shell=True,
    )


@task
def execute(ctx):
    """
    Runs the given example
    """
    exe_path = join(BUILD_DIR, EXAMPLE_TARGET)

    if not exists(exe_path):
        raise RuntimeError("Did not find {} as expected".format(exe_path))

    run(exe_path, shell=True)
