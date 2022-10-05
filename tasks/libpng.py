from faasmtools.env import (
    THIRD_PARTY_DIR,
)
from faasmtools.build import (
    WASM_AR,
    WASM_CC,
    WASM_CFLAGS,
    WASM_LDFLAGS,
    WASM_LIB_INSTALL,
    WASM_RANLIB,
    WASM_SYSROOT,
)
from invoke import task
from os import listdir, makedirs
from os.path import exists, join
from subprocess import run


@task(default=True)
def libpng(ctx, clean=False):
    """
    Compile and install libpng
    """
    libpng_dir = join(THIRD_PARTY_DIR, "libpng")

    if clean:
        run("make clean", shell=True, cwd=libpng_dir, check=True)

    # Instead of running a complicated configure, we use a simplified makefile
    # under `faasm/libpng/scripts/makefile.wasm` to build _only_ libpng
    make_cmd = [
        "WASM_CC={}".format(WASM_CC),
        "WASM_AR={}".format(WASM_AR),
        "WASM_RANLIB={}".format(WASM_RANLIB),
        "WASM_CFLAGS='{}'".format(" ".join(WASM_CFLAGS)),
        "WASM_LDFLAGS='{}'".format(" ".join(WASM_LDFLAGS)),
        "WASM_SYSROOT={}".format(WASM_SYSROOT),
        "make -j",
    ]
    make_cmd = " ".join(make_cmd)
    run(make_cmd, shell=True, cwd=libpng_dir, check=True)

    # Install static library
    cp_cmd = "cp {}/libpng.a {}/libpng16.a".format(
        libpng_dir, WASM_LIB_INSTALL
    )
    run(cp_cmd, shell=True, check=True)
    print(cp_cmd)

    # Install headers
    libpng_header_install_dir = join(WASM_SYSROOT, "include", "libpng16")
    if not exists(libpng_header_install_dir):
        makedirs(libpng_header_install_dir)
    header_files = [
        join(libpng_dir, hf) for hf in listdir(libpng_dir) if hf.endswith(".h")
    ]
    cp_cmd = "cp {} {}/".format(
        " ".join(header_files), libpng_header_install_dir
    )
    print(cp_cmd)
    run(cp_cmd, shell=True, check=True)
