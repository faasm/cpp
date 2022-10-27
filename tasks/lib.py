from faasmtools.build import (
    CMAKE_TOOLCHAIN_FILE,
    WASM_SYSROOT,
    FAASM_NATIVE_DIR,
    get_serialised_faasm_env_vars,
)
from faasmtools.env import PROJ_ROOT
from os.path import join, exists
from os import makedirs
from shutil import rmtree
from subprocess import run


def build_faasm_lib(subdir, clean=False, native=False, shared=False):
    """
    Builds one of the libraries included in this repo
    """
    work_dir = join(PROJ_ROOT, subdir)
    install_dir = FAASM_NATIVE_DIR if native else WASM_SYSROOT

    if native and shared:
        build_dir = "build-native-shared"
    elif native:
        build_dir = "build-native"
    else:
        build_dir = "build-wasm"

    build_dir = join(work_dir, build_dir)

    if exists(build_dir) and clean:
        rmtree(build_dir)

    makedirs(build_dir, exist_ok=True)

    if native:
        extras = [
            "-DCMAKE_C_COMPILER=/usr/bin/clang-10",
            "-DCMAKE_CXX_COMPILER=/usr/bin/clang++-10",
        ]
    else:
        extras = [
            "-DCMAKE_TOOLCHAIN_FILE={}".format(CMAKE_TOOLCHAIN_FILE),
        ]

    build_cmd = [
        "{}".format(
            get_serialised_faasm_env_vars("build") if not native else ""
        ),
        "cmake",
        "-GNinja",
        "-DCMAKE_BUILD_TYPE=Release",
        "-DBUILD_SHARED_LIBS={}".format("ON" if shared else "OFF"),
        "-DCMAKE_INSTALL_PREFIX={}".format(install_dir),
    ]

    build_cmd.extend(extras)
    build_cmd.append(work_dir)

    build_cmd_str = " ".join(build_cmd)
    print(build_cmd_str)

    run(build_cmd_str, shell=True, cwd=build_dir, check=True)

    run("ninja", shell=True, cwd=build_dir, check=True)
    run("ninja install", shell=True, cwd=build_dir, check=True)
