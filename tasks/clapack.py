import os

from os.path import join, exists
from shutil import rmtree
from subprocess import run
from copy import copy

from invoke import task

from faasmtools.env import THIRD_PARTY_DIR, USABLE_CPUS

from faasmtools.build import WASM_SYSROOT, WASM_LIB_INSTALL

HEADERS_DIRS = [join(WASM_SYSROOT, "include", "clapack")]

INSTALLED_LIBS = [
    "libcblas",
    "libf2c",
    "libblas",
    "liblapack",
]


@task
def uninstall(ctx):
    """
    Removes all installed files
    """
    for headers_dir in HEADERS_DIRS:
        if exists(headers_dir):
            print("Removing headers {}".format(headers_dir))
            rmtree(headers_dir)

    for lib_name in INSTALLED_LIBS:
        static_path = join(WASM_LIB_INSTALL, "{}.a".format(lib_name))
        shared_path = join(WASM_LIB_INSTALL, "{}.so".format(lib_name))

        if exists(static_path):
            print("Removing static lib {}".format(static_path))
            os.remove(static_path)

        if exists(shared_path):
            print("Removing shared lib {}".format(shared_path))
            os.remove(shared_path)


@task
def lapacke(ctx, clean=False):
    """
    Builds the LAPACKE interface from LAPACK
    """
    lapacke_dir = join(THIRD_PARTY_DIR, "lapack", "LAPACKE")

    if clean:
        run("make clean", cwd=lapacke_dir, shell=True, check=True)

    run(
        "make -j {}".format(USABLE_CPUS),
        shell=True,
        cwd=lapacke_dir,
        check=True,
    )
    run("make install", shell=True, cwd=lapacke_dir, check=True)


@task(default=True)
def build(ctx, clean=False, shared=False):
    """
    Builds CBLAS, CLAPACK, F2C etc.
    """
    clapack_dir = join(THIRD_PARTY_DIR, "clapack")

    # Build clapack
    if clean:
        run("make clean", cwd=clapack_dir, shell=True, check=True)

    # Set up environment to specify whether we're building static or shared
    env = copy(os.environ)
    env.update({"LIBEXT": ".so" if shared else ".a"})

    # Make libf2c first (needed by others)
    run(
        "make f2clib -j {}".format(USABLE_CPUS),
        shell=True,
        cwd=clapack_dir,
        check=True,
        env=env,
    )

    # Make the rest of CLAPACK
    run(
        "make -j {}".format(USABLE_CPUS),
        shell=True,
        cwd=clapack_dir,
        check=True,
        env=env,
    )
    run("make install", shell=True, cwd=clapack_dir, check=True)
