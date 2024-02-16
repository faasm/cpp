from copy import copy
from faasmtools.env import THIRD_PARTY_DIR, USABLE_CPUS
from faasmtools.build import (
    WASM_SYSROOT,
    get_faasm_build_env_dict,
)
from invoke import task
from os import environ, remove
from os.path import join, exists
from shutil import rmtree
from subprocess import run

HEADERS_DIRS = [join(WASM_SYSROOT, "include", "clapack")]

INSTALLED_LIBS = [
    "libcblas",
    "libf2c",
    "libblas",
    "liblapack",
]

CLAPACK_THIRD_PARTY_DIR = join(THIRD_PARTY_DIR, "faasm-clapack", "third-party")
LAPACKE_DIR = join(CLAPACK_THIRD_PARTY_DIR, "lapack", "LAPACKE")
CLAPACK_DIR = join(CLAPACK_THIRD_PARTY_DIR, "clapack")


@task(default=True)
def build(ctx, clean=False, shared=False):
    """
    Builds CBLAS, CLAPACK, F2C etc.
    """
    # Build clapack
    if clean:
        run("make clean", cwd=CLAPACK_DIR, shell=True, check=True)

    # Set up environment to specify whether we're building static or shared
    env = copy(environ)
    env.update({"LIBEXT": ".so" if shared else ".a"})
    env.update(get_faasm_build_env_dict())

    # Make libf2c first (needed by others)
    run(
        "make f2clib -j {}".format(USABLE_CPUS),
        shell=True,
        cwd=CLAPACK_DIR,
        check=True,
        env=env,
    )

    # Make the rest of CLAPACK
    run(
        "make -j {}".format(USABLE_CPUS),
        shell=True,
        cwd=CLAPACK_DIR,
        check=True,
        env=env,
    )
    run(
        "make install",
        shell=True,
        cwd=CLAPACK_DIR,
        check=True,
        env=env,
    )


@task
def uninstall(ctx):
    """
    Removes all installed files
    """
    build_env = get_faasm_build_env_dict()

    for headers_dir in build_env["FAASM_WASM_HEADER_INSTALL_DIR"]:
        if exists(headers_dir):
            print("Removing headers {}".format(headers_dir))
            rmtree(headers_dir)

    for lib_name in INSTALLED_LIBS:
        static_path = join(
            build_env["FAASM_WASM_LIB_INSTALL_DIR"], "{}.a".format(lib_name)
        )
        shared_path = join(
            build_env["FAASM_WASM_LIB_INSTALL_DIR"], "{}.so".format(lib_name)
        )

        if exists(static_path):
            print("Removing static lib {}".format(static_path))
            remove(static_path)

        if exists(shared_path):
            print("Removing shared lib {}".format(shared_path))
            remove(shared_path)


@task
def lapacke(ctx, clean=False):
    """
    Builds the LAPACKE interface from LAPACK
    """
    if clean:
        run("make clean", cwd=LAPACKE_DIR, shell=True, check=True)

    run(
        "make -j {}".format(USABLE_CPUS),
        shell=True,
        cwd=LAPACKE_DIR,
        check=True,
    )
    run("make install", shell=True, cwd=LAPACKE_DIR, check=True)
