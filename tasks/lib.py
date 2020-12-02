from os.path import join, exists
from os import makedirs
from subprocess import run
from shutil import rmtree

from faasmtools.build import CMAKE_TOOLCHAIN_FILE, WASM_SYSROOT

from faasmtools.env import PROJ_ROOT


def build_faasm_lib(subdir, clean=False, native=False):
    """
    Builds one of the libraries included in this repo
    """
    work_dir = join(PROJ_ROOT, subdir)
    build_dir = join(work_dir, "build-native" if native else "build-wasm")

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
            "-DCMAKE_INSTALL_PREFIX={}".format(WASM_SYSROOT),
        ]

    build_cmd = ["cmake", "-GNinja", "-DCMAKE_BUILD_TYPE=Release"]

    build_cmd.extend(extras)
    build_cmd.append(work_dir)

    build_cmd_str = " ".join(build_cmd)
    print(build_cmd_str)

    run(build_cmd_str, shell=True, cwd=build_dir, check=True)

    run("ninja", shell=True, cwd=build_dir, check=True)
    run("ninja install", shell=True, cwd=build_dir, check=True)
