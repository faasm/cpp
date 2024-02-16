from faasmtools.build import (
    CMAKE_TOOLCHAIN_FILE,
    WASM_SYSROOT,
    get_faasm_build_env_dict,
)
from faasmtools.env import PROJ_ROOT, FAASM_RUNTIME_ROOT
from invoke import task
from os import environ, makedirs
from os.path import exists, join
from shutil import rmtree
from subprocess import run


@task(default=True)
def fake(ctx, clean=False):
    """
    Compile and install the fake library used for testing
    """
    work_dir = join(PROJ_ROOT, "func", "dynlink")
    build_dir = join(PROJ_ROOT, "build", "libfake")

    if exists(build_dir) and clean:
        rmtree(build_dir)

    makedirs(build_dir, exist_ok=True)

    build_cmd = [
        "cmake",
        "-GNinja",
        "-DFAASM_BUILD_SHARED=ON",
        "-DCMAKE_TOOLCHAIN_FILE={}".format(CMAKE_TOOLCHAIN_FILE),
        "-DCMAKE_BUILD_TYPE=Release",
        "-DCMAKE_INSTALL_PREFIX={}".format(WASM_SYSROOT),
        work_dir,
    ]

    work_env = environ.copy()
    work_env.update(get_faasm_build_env_dict())
    run(
        " ".join(build_cmd),
        shell=True,
        cwd=build_dir,
        check=True,
        env=work_env,
    )
    run("ninja", shell=True, cwd=build_dir, check=True, env=work_env)
    run("ninja install", shell=True, cwd=build_dir, check=True, env=work_env)

    # Copy shared object into place
    sysroot_files = join(WASM_SYSROOT, "lib", "wasm32-wasi", "libfake*.so")

    runtime_lib_dir = join(FAASM_RUNTIME_ROOT, "lib", "fake")
    makedirs(runtime_lib_dir, exist_ok=True)

    run(
        "cp {} {}".format(sysroot_files, runtime_lib_dir),
        shell=True,
        check=True,
    )
