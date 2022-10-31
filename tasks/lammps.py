from faasmtools.build import CMAKE_TOOLCHAIN_FILE, FAASM_BUILD_ENV_DICT
from faasmtools.compile_util import wasm_copy_upload
from faasmtools.env import THIRD_PARTY_DIR
from invoke import task
from os import environ, makedirs
from os.path import exists, join
from shutil import rmtree
from subprocess import run


@task(default=True)
def build(ctx, clean=False):
    """
    Build the LAMMPS molecule dynamics simulator.

    Note that LAMMPS is a self-contained binary, and different workloads are
    executed by passing different command line arguments. As a consequence,
    we cross-compile it and copy the binary (lmp) to lammps/main/function.wasm
    """
    lammps_dir = join(THIRD_PARTY_DIR, "lammps")
    cmake_dir = join(lammps_dir, "cmake")
    build_dir = join(lammps_dir, "build")

    if clean and exists(build_dir):
        rmtree(build_dir)

    if not exists(build_dir):
        makedirs(build_dir)

    cmake_cmd = [
        "cmake",
        "-GNinja",
        "-DLAMMPS_FAASM=ON",
        "-DCMAKE_TOOLCHAIN_FILE={}".format(CMAKE_TOOLCHAIN_FILE),
        "-DCMAKE_BUILD_TYPE=Release",
        cmake_dir,
    ]
    cmake_cmd = " ".join(cmake_cmd)

    work_env = environ.copy()
    work_env.update(FAASM_BUILD_ENV_DICT)

    run(cmake_cmd, shell=True, check=True, cwd=build_dir, env=work_env)
    run("ninja", shell=True, check=True, cwd=build_dir)

    # Copy the binary to lammps/main/function.wasm
    wasm_copy_upload("lammps", "main", join(build_dir, "lmp"))
