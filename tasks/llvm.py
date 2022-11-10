from faasmtools.build import (
    FAASM_BUILD_ENV_DICT,
    FAASM_LOCAL_DIR,
    WASM_LIB_INSTALL,
)
from faasmtools.env import (
    LLVM_DIR,
    LLVM_MAKEFILE,
    LLVM_VERSION,
    PROJ_ROOT,
    USABLE_CPUS,
    WASI_LIBC_DIR,
)
from invoke import task
from os import environ
from os.path import join
from subprocess import run

LLVM_BUILD_DIR = join(LLVM_DIR, "build")
LLVM_BUILD_ENV_VARS = {
    "FAASM_CPP_PROJ_ROOT": PROJ_ROOT,
    "FAASM_LLVM_DIR": LLVM_DIR,
    "FAASM_LLVM_BUILD_DIR": LLVM_BUILD_DIR,
    "FAASM_LLVM_VERSION": LLVM_VERSION,
    "FAASM_LOCAL_DIR_ENV": FAASM_LOCAL_DIR,
    "FAASM_WASI_LIBC_DIR": WASI_LIBC_DIR,
}


@task(default=True)
def build(ctx, clean=False):
    """
    Build LLVM
    """
    build_env = environ.copy()
    # These env. variables are required for LLVM's build configuration (and
    # cleaning)
    build_env.update(LLVM_BUILD_ENV_VARS)

    if clean:
        run(
            "make -f {} clean-all".format(LLVM_MAKEFILE),
            shell=True,
            check=True,
            cwd=PROJ_ROOT,
            env=build_env,
        )

    # These env. variables are to actually build LLVM
    build_env.update(FAASM_BUILD_ENV_DICT)
    make_cmd = [
        "make",
        "-j {}".format(USABLE_CPUS),
        "-f {}".format(LLVM_MAKEFILE),
    ]
    make_cmd = " ".join(make_cmd)
    run(make_cmd, shell=True, check=True, cwd=PROJ_ROOT, env=build_env)


@task()
def libc(ctx, clean=False):
    """
    Builds the wasi libc fork in this directory
    """
    build_env = environ.copy()
    build_env.update(LLVM_BUILD_ENV_VARS)

    if clean:
        run(
            "make clean-libc",
            shell=True,
            check=True,
            cwd=PROJ_ROOT,
            env=build_env,
        )

    build_env.update(FAASM_BUILD_ENV_DICT)
    make_cmd = [
        "make",
        "-j {}".format(USABLE_CPUS),
        "-f {}".format(LLVM_MAKEFILE),
        "libc",
    ]
    make_cmd = " ".join(make_cmd)
    run(make_cmd, check=True, shell=True, cwd=PROJ_ROOT, env=build_env)

    # Copy the import files into place
    copy_cmd = "cp -r sysroot_extras/* {}".format(WASM_LIB_INSTALL)
    print("\nCopying undefined symbols into place: \n{}".format(copy_cmd))
    run(copy_cmd, check=True, shell=True, cwd=PROJ_ROOT)
