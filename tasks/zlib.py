from faasmtools.env import (
    THIRD_PARTY_DIR,
)
from faasmtools.build import (
    WASM_AR,
    WASM_CC,
    WASM_CFLAGS,
    WASM_CXX,
    WASM_LDFLAGS,
    WASM_LD,
    WASM_LIB_INSTALL,
    WASM_NM,
    WASM_RANLIB,
    WASM_SYSROOT,
)
from invoke import task
from os.path import join
from subprocess import run


@task(default=True)
def zlib(ctx, clean=False):
    """
    Compile and install libpng
    """
    zlib_dir = join(THIRD_PARTY_DIR, "zlib")

    if clean:
        run("make clean", shell=True, cwd=zlib_dir, check=True)

    configure_cmd = [
        "CC={}".format(WASM_CC),
        "CXX={}".format(WASM_CXX),
        "CFLAGS='{}'".format(" ".join(WASM_CFLAGS)),
        "CXXFLAGS='{}'".format(" ".join(WASM_CFLAGS)),
        "LD={}".format(WASM_LD),
        "LDFLAGS='{}'".format(" ".join(WASM_LDFLAGS)),
        "AR={}".format(WASM_AR),
        "RANLIB={}".format(WASM_RANLIB),
        "NM={}".format(WASM_NM),
        "./configure",
        "--prefix={}".format(WASM_SYSROOT),
        "--static",
    ]

    configure_cmd = " ".join(configure_cmd)
    run(configure_cmd, shell=True, cwd=zlib_dir, check=True)

    run("make -j", shell=True, cwd=zlib_dir, check=True)
    run("make install", shell=True, cwd=zlib_dir, check=True)

    # Move the library from /usr/local/faasm/llvm-sysroot/lib into wasm32-wasi
    mv_cmd = "mv {}/lib/libz.a {}".format(WASM_SYSROOT, WASM_LIB_INSTALL)
    print(mv_cmd)
    run(mv_cmd, shell=True, check=True)
