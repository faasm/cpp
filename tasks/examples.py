from os import makedirs
from shutil import rmtree
from os.path import join, exists
from subprocess import run

from faasmtools.env import PROJ_ROOT

from invoke import task

EXAMPLES_DIR = join(PROJ_ROOT, "examples")
BASE_BUILD_DIR = "/build/cpp-examples"
STATIC_BUILD_DIR = join(BASE_BUILD_DIR, "build-shared")
SHARED_BUILD_DIR = join(BASE_BUILD_DIR, "build-static")
EXAMPLE_TARGET = "native_exe"


@task(default=True)
def build(ctx, clean=False, shared=False):
    """
    Builds the examples
    """
    build_dir = SHARED_BUILD_DIR if shared else STATIC_BUILD_DIR

    if clean and exists(build_dir):
        rmtree(build_dir)

    if not exists(build_dir):
        makedirs(build_dir)

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
        cwd=build_dir,
    )

    # Build
    run(
        "cmake --build . --target {}".format(EXAMPLE_TARGET),
        cwd=build_dir,
        shell=True,
        check=True,
    )


@task
def execute(ctx, shared=False):
    """
    Runs the given example
    """
    build_dir = SHARED_BUILD_DIR if shared else STATIC_BUILD_DIR
    exe_path = join(build_dir, EXAMPLE_TARGET)

    if not exists(exe_path):
        raise RuntimeError("Did not find {} as expected".format(exe_path))

    run(exe_path, shell=True, check=True)
