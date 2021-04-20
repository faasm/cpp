from subprocess import run

from os.path import join, exists
from shutil import rmtree

from invoke import task

from faasmtools.env import (
    USABLE_CPUS,
    THIRD_PARTY_DIR,
    PROJ_ROOT,
)

from faasmtools.build import (
    WASM_CC,
    WASM_AR,
    WASM_LIB_INSTALL,
    WASM_NM,
    WASM_SYSROOT,
)


@task(default=True)
def build(ctx, clean=False):
    """
    Builds the wasi libc fork in this directory
    """
    libc_dir = join(THIRD_PARTY_DIR, "wasi-libc")
    libc_build_dir = join(libc_dir, "build")

    if clean:
        run("make clean", shell=True, check=True, cwd=libc_dir)

        if exists(libc_build_dir):
            rmtree(libc_build_dir)

    make_cmd = [
        "make",
        "-j {}".format(USABLE_CPUS),
        "THREAD_MODEL=faasm",
        "WASM_CC={}".format(WASM_CC),
        "WASM_AR={}".format(WASM_AR),
        "WASM_NM={}".format(WASM_NM),
        "SYSROOT={}".format(WASM_SYSROOT),
    ]
    make_cmd = " ".join(make_cmd)
    print(make_cmd)

    # Run the build
    run(make_cmd, check=True, shell=True, cwd=libc_dir)

    # Copy the import files into place
    copy_cmd = "cp -r sysroot_extras/* {}".format(WASM_LIB_INSTALL)
    print("\nCopying undefined symbols into place: \n{}".format(copy_cmd))
    run(copy_cmd, check=True, shell=True, cwd=PROJ_ROOT)
