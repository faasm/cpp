from copy import copy
from os.path import join
from subprocess import run
from os import environ

# -----------------------------------------
# This file is the single source of truth for all build-related environment
# variables for Faasm and Faasm's C/C++ support. There are several crucial
# modifications, used all across the project, so be **very** careful when
# changing it, and be ready to fix problems.
# -----------------------------------------

# Directories
FAASM_LOCAL_DIR = environ.get("FAASM_LOCAL_DIR", "/usr/local/faasm")
FAASM_NATIVE_DIR = join(FAASM_LOCAL_DIR, "native")
FAASM_CMAKE_ROOT = "/usr/share/cmake-3.24"
WASM_SYSROOT = join(FAASM_LOCAL_DIR, "llvm-sysroot")
WASM_HEADER_INSTALL = "{}/include".format(WASM_SYSROOT)
WASM_LIB_INSTALL = "{}/lib/wasm32-wasi".format(WASM_SYSROOT)
WASM_TOOLCHAIN_ROOT = join(FAASM_LOCAL_DIR, "toolchain")
WASM_TOOLCHAIN_TOOLS = join(WASM_TOOLCHAIN_ROOT, "tools")
WASM_TOOLCHAIN_BIN = join(WASM_TOOLCHAIN_ROOT, "bin")

# Toolchain files
CMAKE_TOOLCHAIN_FILE = join(WASM_TOOLCHAIN_TOOLS, "WasiToolchain.cmake")
MAKE_TOOLCHAIN_FILE = join(WASM_TOOLCHAIN_TOOLS, "Makefile.envs")

# Executables
WASM_CC = join(WASM_TOOLCHAIN_BIN, "clang")
WASM_CXX = join(WASM_TOOLCHAIN_BIN, "clang++")
WASM_CPP = join(WASM_TOOLCHAIN_BIN, "clang-cpp")
WASM_AR = join(WASM_TOOLCHAIN_BIN, "llvm-ar")
WASM_NM = join(WASM_TOOLCHAIN_BIN, "llvm-nm")
WASM_RANLIB = join(WASM_TOOLCHAIN_BIN, "llvm-ranlib")

# Use top-level clang as the linker rather than invoking wasm-ld directly
WASM_LD = WASM_CC
WASM_LDXX = WASM_CXX

# Host triple
WASM_BUILD = "wasm32"
WASM_HOST = "wasm32-unknown-wasi"
WASM_HOST_STATIC = "wasm32-wasi"
WASM_HOST_SHARED = "wasm32-unknown-emscripten"
WASM_HOST_UNKNOWN = "wasm32-unknown-unknown"

# WASM Constants
# NOTE: we have to set the max memory here but want it to be as close to the
# max (4GB) as possible. If we set it to the max though, this input is
# ignored and the function is set with no maximum memory size.  Therefore we
# set it to 4GB - 1 page, i.e.  max-memory = (4*1024*1024*1024) - (64*1024)
# = 4294901760.
#
# WARNING: the code may also set limits on the max memory size, so must be
# changed there too (IRModuleCache.h)
#
# This max memory limit can stop benchmarks working so make sure it's big
# and consider restricting in a more specific manner for certain functions.
FAASM_WASM_MAX_MEMORY = 4 * 1024 * 1024 * 1024 - 64 * 1024
FAASM_WASM_STACK_SIZE = 4 * 1024 * 1024
FAASM_WASM_BYTES_PER_PAGE = 65536
FAASM_WASM_INITIAL_MEMORY_SIZE = 4 * FAASM_WASM_STACK_SIZE

# C/C++ compilation flags
# Note: see Clang wasm-specific flags at
# https://clang.llvm.org/docs/ClangCommandLineReference.html#webassembly Note
# the optimisation level. We want to keep vectorization so stick with O3 Also
# note that the optimisation is crucial to getting any kind of decent
# performance.
#
# We must explicitly exclude atomics here just in case we've accidentally
# introduced them upstream. Use of atomics means we can't link things together:
# https://reviews.llvm.org/D59281
WASM_CFLAGS = [
    "-O3 -mno-atomics",
    "--sysroot={}".format(WASM_SYSROOT),
    "-m32",
    "-DANSI",
    "-D__faasm",
]

WASM_CXXFLAGS = WASM_CFLAGS

# Flags for shared libraries
WASM_CFLAGS_SHARED = [
    "-D__wasi__",
    "-nostdlib",
    "-nostdlib++",
    "-fPIC",
    "--target={}".format(WASM_HOST_SHARED),
]
WASM_CXXFLAGS_SHARED = WASM_CFLAGS_SHARED

# ----------
# LDFLAGS
# ----------

# Important symbol names
FAASM_WASM_CTORS_FUNC_NAME = "__wasm_call_ctors"
FAASM_WASM_ZYGOTE_FUNC_NAME = "_faasm_zygote"

# Flags for static libraries
# The stack-first here is really important to help detect stack overflow
# issues. Without it the stack will overflow into the global data.
# stack-size is also crucial to bigger functions not messing up
WASM_LDFLAGS = [
    "-static",
    "-Xlinker --no-gc-sections",
    "-Xlinker --stack-first",
    "-Xlinker --no-check-features",
]

# Flags for executables
WASM_EXE_LDFLAGS = [
    "-Xlinker --stack-first",
    "-Xlinker --export=__heap_base",
    "-Xlinker --export=__data_end",
    "-Xlinker --export={}".format(FAASM_WASM_CTORS_FUNC_NAME),
    "-Xlinker --max-memory={}".format(FAASM_WASM_MAX_MEMORY),
    "-Wl,-z,stack-size={} -Wl".format(FAASM_WASM_STACK_SIZE),
    "-Wl,--initial-memory={}".format(FAASM_WASM_INITIAL_MEMORY_SIZE),
]

# These are the compiler and linker flags required for functions that will also
# do dynamic linking.  We need to export all symbols to make them available to
# the dynamically loaded modules
WASM_EXE_LDFLAGS_SHARED = [
    "-Xlinker --export-all",
    "-Xlinker --no-gc-sections",
]

# Flags for shared libraries
# See notes in README about WebAssembly and shared libraries
WASM_LDFLAGS_SHARED = [
    "-nostdlib",
    "-nostdlib++",
    "-Xlinker --no-entry",
    "-Xlinker --shared",
    "-Xlinker --export-all",
    "-Xlinker --no-gc-sections",
]

WASM_CCSHARED = " ".join([WASM_CC] + WASM_CFLAGS_SHARED)
WASM_CXXSHARED = " ".join([WASM_CXX] + WASM_CFLAGS_SHARED)
WASM_LDSHARED = " ".join([WASM_CC] + WASM_LDFLAGS_SHARED)
WASM_LDXXSHARED = " ".join([WASM_CXX] + WASM_LDFLAGS_SHARED)

# Lib order very important here
# TODO: remove this when BLAS support is moved to a task
WASM_BLAS_LIBS = [
    "lapack",
    "blas",
    "f2c",
]

# ----------
# Env. variables for different environments
# ----------

# Env. variables as a dictionary: prefix with FAASM_WASM or FAASM_NATIVE
# depending on the build type variables target
FAASM_BUILD_ENV_DICT = {
    "CMAKE_ROOT": FAASM_CMAKE_ROOT,
    "FAASM_NATIVE_INSTALL_DIR": FAASM_NATIVE_DIR,
    "FAASM_WASM_MAKE_TOOLCHAIN_FILE": MAKE_TOOLCHAIN_FILE,
    "FAASM_WASM_CC": WASM_CC,
    "FAASM_WASM_CXX": WASM_CXX,
    "FAASM_WASM_AR": WASM_AR,
    "FAASM_WASM_NM": WASM_NM,
    "FAASM_WASM_RANLIB": WASM_RANLIB,
    "FAASM_WASM_HOST_SHARED": WASM_HOST_SHARED,
    "FAASM_WASM_HOST_STATIC": WASM_HOST_STATIC,
    "FAASM_WASM_HOST_UNKNOWN": WASM_HOST_UNKNOWN,
    "FAASM_WASM_INSTALL_DIR": WASM_TOOLCHAIN_BIN,
    "FAASM_WASM_SYSROOT": WASM_SYSROOT,
    "FAASM_WASM_CFLAGS": " ".join(WASM_CFLAGS),
    "FAASM_WASM_CFLAGS_SHARED": " ".join(WASM_CFLAGS_SHARED),
    "FAASM_WASM_EXE_LINKER_FLAGS": " ".join(WASM_EXE_LDFLAGS),
    "FAASM_WASM_EXE_LINKER_FLAGS_SHARED": " ".join(WASM_EXE_LDFLAGS_SHARED),
    "FAASM_WASM_SHARED_LINKER_FLAGS": " ".join(WASM_LDFLAGS_SHARED),
    "FAASM_WASM_STATIC_LINKER_FLAGS": " ".join(WASM_LDFLAGS),
    "FAASM_WASM_BLAS_LIBS": " ".join(WASM_BLAS_LIBS),
}

# Note that values in the environment dictionary can only be strings, bytes, or
# Path-like objects, otherwise python's subprocess module complains
FAASM_RUNTIME_ENV_DICT = {
    "FAASM_WASM_BYTES_PER_PAGE": str(FAASM_WASM_BYTES_PER_PAGE),
    "FAASM_WASM_CTORS_FUNC_NAME": FAASM_WASM_CTORS_FUNC_NAME,
    "FAASM_WASM_MAX_MEMORY": str(FAASM_WASM_MAX_MEMORY),
    "FAASM_WASM_STACK_SIZE": str(FAASM_WASM_STACK_SIZE),
    "FAASM_WASM_ZYGOTE_FUNC_NAME": FAASM_WASM_ZYGOTE_FUNC_NAME,
}


def get_dict_as_cmake_vars(env_dict):
    return " ".join(["-D{}={}".format(k, env_dict[k]) for k in env_dict])


# ----------
# Scripts for configure, automake, and autotools
# ----------

# Variables for 'configure' scripts
_BASE_CONFIG_CMD = [
    "CC={}".format(WASM_CC),
    "CXX={}".format(WASM_CXX),
    "CPP={}".format(WASM_CPP),
    "AR={}".format(WASM_AR),
    "RANLIB={}".format(WASM_RANLIB),
    'CFLAGS="{}"'.format(" ".join(WASM_CFLAGS)),
    'CPPFLAGS="{}"'.format(" ".join(WASM_CFLAGS)),
    'CXXFLAGS="{}"'.format(" ".join(WASM_CXXFLAGS)),
    'CCSHARED="{}"'.format(WASM_CCSHARED),
    'CXXSHARED="{}"'.format(WASM_CXXSHARED),
]

_BASE_CONFIG_ARGS = [
    "--build={}".format(WASM_BUILD),
    "--host={}".format(WASM_HOST),
]

_BASE_CONFIG_ARGS_SHARED = [
    "--build={}".format(WASM_BUILD),
    "--host={}".format(WASM_HOST_SHARED),
]

BASE_CONFIG_CMD = _BASE_CONFIG_CMD + [
    "LD={}".format(WASM_LD),
    'LDSHARED="{}"'.format(WASM_LDSHARED),
]

BASE_CONFIG_CMDXX = _BASE_CONFIG_CMD + [
    "LD={}".format(WASM_LDXX),
    'LDSHARED="{}"'.format(WASM_LDXXSHARED),
]

BASE_CONFIG_FLAGS = [
    'CFLAGS="{}"'.format(" ".join(WASM_CFLAGS)),
    'CPPFLAGS="{}"'.format(" ".join(WASM_CXXFLAGS)),
    'LDFLAGS="{}"'.format(" ".join(WASM_LDFLAGS)),
]

BASE_CONFIG_FLAGS_SHARED = [
    'CFLAGS="{}"'.format(" ".join(WASM_CFLAGS_SHARED)),
    'CPPFLAGS="{}"'.format(" ".join(WASM_CXXFLAGS_SHARED)),
    'LDFLAGS="{}"'.format(" ".join(WASM_LDFLAGS_SHARED)),
]


def build_config_cmd(cmd, shared=False, cxx=False, conf_args=True):
    """
    Wraps an autotools command in the relevant environment variables and
    cross-compilation config
    """
    result = copy(BASE_CONFIG_CMDXX if cxx else BASE_CONFIG_CMD)

    result += BASE_CONFIG_FLAGS_SHARED if shared else BASE_CONFIG_FLAGS

    result += cmd

    if conf_args:
        result += _BASE_CONFIG_ARGS_SHARED if shared else _BASE_CONFIG_ARGS

    return result


def run_autotools(proj_dir):
    """
    Runs through autotools set-up on the given directory
    """

    def _run_auto_cmd(cmd):
        print("Running {}".format(cmd))
        auto_cmd = build_config_cmd([cmd], conf_args=False)
        auto_cmd = " ".join(auto_cmd)
        run(auto_cmd, shell=True, check=True, cwd=proj_dir)

    _run_auto_cmd("libtoolize")
    _run_auto_cmd("aclocal")
    _run_auto_cmd("autoheader")
    _run_auto_cmd("autoconf")
    _run_auto_cmd("automake --add-missing")
