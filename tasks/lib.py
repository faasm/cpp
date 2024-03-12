from faasmtools.build import (
    CMAKE_TOOLCHAIN_FILE,
    FAASM_NATIVE_DIR,
    get_faasm_build_env_dict,
)
from faasmtools.env import LLVM_NATIVE_VERSION, PROJ_ROOT
from os.path import exists, join
from os import environ, makedirs
from shutil import rmtree
from subprocess import run


def build_faasm_lib(
    subdir, clean=False, native=False, shared=False, threads=False
):
    """
    Builds one of the libraries included in this repo
    """
    work_dir = join(PROJ_ROOT, subdir)
    install_dir = (
        FAASM_NATIVE_DIR
        if native
        else get_faasm_build_env_dict(threads)["FAASM_WASM_LIB_INSTALL_DIR"]
    )

    if native and shared:
        build_dir = "build-native-shared"
    elif native:
        build_dir = "build-native"
    elif threads:
        build_dir = "build-wasm32-wasi-threads"
    else:
        build_dir = "build-wasm32-wasi"

    build_dir = join(work_dir, build_dir)

    if exists(build_dir) and clean:
        rmtree(build_dir)

    makedirs(build_dir, exist_ok=True)

    if native:
        llvm_major = LLVM_NATIVE_VERSION.split(".")[0]
        extras = [
            "-DCMAKE_C_COMPILER=/usr/bin/clang-{}".format(llvm_major),
            "-DCMAKE_CXX_COMPILER=/usr/bin/clang++-{}".format(llvm_major),
            "-DCMAKE_AR=/usr/bin/llvm-ar-{}".format(llvm_major),
            "-DCMAKE_NM=/usr/bin/llvm-nm-{}".format(llvm_major),
            "-DCMAKE_RANLIB=/usr/bin/llvm-ranlib-{}".format(llvm_major),
        ]
    else:
        extras = [
            "-DCMAKE_TOOLCHAIN_FILE={}".format(CMAKE_TOOLCHAIN_FILE),
        ]

    build_cmd = [
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

    work_env = environ.copy()
    if not native:
        work_env.update(get_faasm_build_env_dict(is_threads=threads))
    run(build_cmd_str, shell=True, cwd=build_dir, check=True, env=work_env)

    run("ninja", shell=True, cwd=build_dir, check=True)
    run("ninja install", shell=True, cwd=build_dir, check=True)
