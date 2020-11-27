from os.path import join, exists
from os import makedirs
from subprocess import run
from shutil import rmtree

from faasmtools.build import CMAKE_TOOLCHAIN_FILE
from invoke import task

from faasmtools.env import PROJ_ROOT


@task(default=True)
def build(ctx, clean=False):
    """
    Builds Faabric MPI C/C++ lib
    """
    work_dir = join(PROJ_ROOT, "libfaasmpi")
    build_dir = join(work_dir, "build")

    if exists(build_dir):
        if clean:
            rmtree(build_dir)
            makedirs(build_dir)
    else:
        makedirs(build_dir)

    build_cmd = [
        "cmake",
        "-GNinja",
        "-DFAASM_BUILD_TYPE=wasm",
        "-DCMAKE_BUILD_TYPE=Release",
        "-DCMAKE_TOOLCHAIN_FILE={}".format(CMAKE_TOOLCHAIN_FILE),
        work_dir,
    ]

    build_cmd_str = " ".join(build_cmd)
    print(build_cmd_str)

    run(build_cmd_str, shell=True, cwd=build_dir, check=True)

    run("ninja", shell=True, cwd=build_dir, check=True)
    run("ninja install", shell=True, cwd=build_dir, check=True)
