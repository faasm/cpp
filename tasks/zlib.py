from faasmtools.env import THIRD_PARTY_DIR
from faasmtools.build import get_faasm_build_env_dict
from invoke import task
from os.path import join
from subprocess import run


@task(default=True)
def zlib(ctx, clean=False, threads=False):
    """
    Compile and install zlib
    """
    zlib_dir = join(THIRD_PARTY_DIR, "zlib")

    if clean:
        run("make clean", shell=True, cwd=zlib_dir, check=True)

    build_env = get_faasm_build_env_dict(threads)

    configure_cmd = [
        "CC={}".format(build_env["FAASM_WASM_CC"]),
        "CXX={}".format(build_env["FAASM_WASM_CXX"]),
        "CFLAGS='{}'".format(build_env["FAASM_WASM_CFLAGS"]),
        "CXXFLAGS='{}'".format(build_env["FAASM_WASM_CXXFLAGS"]),
        "LD={}".format(build_env["FAASM_WASM_CC"]),
        "LDFLAGS='{}'".format(build_env["FAASM_WASM_STATIC_LINKER_FLAGS"]),
        "AR={}".format(build_env["FAASM_WASM_AR"]),
        "RANLIB={}".format(build_env["FAASM_WASM_RANLIB"]),
        "NM={}".format(build_env["FAASM_WASM_NM"]),
        # TODO: can we add the target here too so that install just works?
        "./configure",
        "--includedir={}".format(build_env["FAASM_WASM_HEADER_INSTALL_DIR"]),
        "--libdir={}".format(build_env["FAASM_WASM_LIB_INSTALL_DIR"]),
        "--static",
    ]

    configure_cmd = " ".join(configure_cmd)
    run(configure_cmd, shell=True, cwd=zlib_dir, check=True)

    # Only build the static library, not the examples nor tests
    run("make -j libz.a", shell=True, cwd=zlib_dir, check=True)
    run("make install", shell=True, cwd=zlib_dir, check=True)
