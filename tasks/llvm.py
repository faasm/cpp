from faasmtools.build import FAASM_BUILD_ENV_DICT, FAASM_LOCAL_DIR
from faasmtools.env import (
    LLVM_DIR,
    LLVM_MAKEFILE,
    LLVM_VERSION,
    PROJ_ROOT,
    WASI_LIBC_DIR,
)
from invoke import task
from os import environ
from os.path import join
from subprocess import run


@task(default=True)
def build(ctx, clean=False):
    """
    Build LLVM
    """
    llvm_build_dir = join(LLVM_DIR, "build")
    llvm_build_env_vars = {
        "FAASM_CPP_PROJ_ROOT": PROJ_ROOT,
        "FAASM_LLVM_DIR": LLVM_DIR,
        "FAASM_LLVM_BUILD_DIR": llvm_build_dir,
        "FAASM_LLVM_VERSION": LLVM_VERSION,
        "FAASM_LOCAL_DIR_ENV": FAASM_LOCAL_DIR,
        "FAASM_WASI_LIBC_DIR": WASI_LIBC_DIR,
    }

    build_env = environ.copy()
    # These env. variables are required for LLVM's build configuration (and
    # cleaning)
    build_env.update(llvm_build_env_vars)

    if clean:
        run("make -f {} clean-all".format(LLVM_MAKEFILE), shell=True, check=True, cwd=PROJ_ROOT, env=build_env)

    # These env. variables are to actually build LLVM
    build_env.update(FAASM_BUILD_ENV_DICT)
    run("make -f {}".format(LLVM_MAKEFILE), shell=True, check=True, cwd=PROJ_ROOT, env=build_env)
