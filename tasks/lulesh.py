from faasmtools.build import CMAKE_TOOLCHAIN_FILE, FAASM_BUILD_ENV_DICT
from faasmtools.compile_util import wasm_copy_upload
from faasmtools.env import THIRD_PARTY_DIR
from invoke import task
from os import environ, makedirs
from os.path import exists, join
from shutil import rmtree
from subprocess import run


@task(default=True)
def build(ctx, clean=False)
    """
    Build the LAMMPS dynamic molecule simulator.

    Note that LAMMPS is a self-contained binary, and different workloads are
    executed by passing different command line arguments. As a consequence,
    we cross-compile it and copy the binary (lmp) to lammps/main/function.wasm
    """
    pass
