from faasmtools.build import (
    FAASM_BUILD_ENV_DICT,
    FAASM_LOCAL_DIR,
    WASM_LIB_INSTALL,
    WASM_SYSROOT,
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
    "FAASM_SYSROOT": WASM_SYSROOT,
}


def do_llvm_build(target, clean_target=None):
    build_env = environ.copy()
    # These env. variables are required for LLVM's build configuration (and
    # cleaning)
    build_env.update(LLVM_BUILD_ENV_VARS)

    if clean_target:
        run(
            "make -f {} {}".format(LLVM_MAKEFILE, clean_target),
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
        target,
    ]
    make_cmd = " ".join(make_cmd)
    run(make_cmd, shell=True, check=True, cwd=PROJ_ROOT, env=build_env)


@task(default=True)
def build(ctx, clean=False):
    """
    Build LLVM and all targets (including libc)
    """
    do_llvm_build("", "clean-all" if clean else None)


@task()
def libs(ctx, clean=False):
    """
    Build the compiler-rt, libcxx, and libcxxabi for our LLVM fork
    """
    do_llvm_build(
        "libcxx libcxxabi compiler-rt", "clean-libs" if clean else None
    )


@task()
def patch_wasi_libc(ctx):
    """
    Patch wasi-libc with the changes required for Faasm
    """
    patch_dir = join(WASI_LIBC_DIR, "faasm-patches")
    patches = [
        "Faasm_switch_on_mman.patch",
    ]

    for patch in patches:
        git_cmd = "git apply {}".format(join(patch_dir, patch))
        run(git_cmd, shell=True, check=True, cwd=WASI_LIBC_DIR)


@task()
def libc(ctx, clean=False, purge=False):
    """
    Builds the wasi libc fork in this directory

    Clean removes the built targets but does not wipe the sysroot. Purge wipes
    the sysroot altogether.
    """
    clean_args = None
    if purge:
        if clean:
            print(
                "WARNING: set --clean and --purge flag. The latter takes preference"
            )
        clean_args = "very-clean-libc"
    elif clean:
        clean_args = "clean-libc"

    do_llvm_build("libc", clean_args)

    # Copy the import files into place
    copy_cmd = "cp -r sysroot_extras/* {}".format(WASM_LIB_INSTALL)
    print("\nCopying undefined symbols into place: \n{}".format(copy_cmd))
    run(copy_cmd, check=True, shell=True, cwd=PROJ_ROOT)
