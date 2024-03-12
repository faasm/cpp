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
# TODO(shared-libs): consider removing this
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
    "-O3",
    # TODO: may want to use -mrelaxed-simd instead
    "-msimd128",
    "--sysroot={}".format(WASM_SYSROOT),
    "-m32",
    "-DANSI",
    "-D__faasm",
]

# Wasi-libc specific flags that are needed to include certain emulation headers
WASM_WASI_LIBC_CFLAGS = [
    "-D_WASI_EMULATED_GETPID",
    "-D_WASI_EMULATED_PROCESS_CLOCKS",
    "-D_WASI_EMULATED_SIGNAL",
]

WASM_CFLAGS += WASM_WASI_LIBC_CFLAGS

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

# Wasi-libc specific libraries we need to link with to enable certain emulated
# functionalities
WASM_WASI_LIBC_LDFLAGS = [
    "-lc-printscan-long-double",
    "-lwasi-emulated-getpid",
    "-lwasi-emulated-process-clocks",
    "-lwasi-emulated-signal",
]

WASM_LDFLAGS += WASM_WASI_LIBC_LDFLAGS

# Flags for executables
WASM_EXE_LDFLAGS = [
    "-Xlinker --stack-first",
    "-Xlinker --export=__heap_base",
    "-Xlinker --export=__data_end",
    "-Xlinker --export=free",
    "-Xlinker --export=malloc",
    "-Xlinker --export={}".format(FAASM_WASM_CTORS_FUNC_NAME),
    "-Xlinker --export=__stack_pointer",
    "-Xlinker --max-memory={}".format(FAASM_WASM_MAX_MEMORY),
    "-Wl,-z,stack-size={} -Wl".format(FAASM_WASM_STACK_SIZE),
]

WASM_EXE_LDFLAGS += WASM_WASI_LIBC_LDFLAGS

# These are the compiler and linker flags required for functions that will also
# do dynamic linking.  We need to export all symbols to make them available to
# the dynamically loaded modules
WASM_EXE_LDFLAGS_SHARED = [
    "-Xlinker --export-all",
    "-Xlinker --no-gc-sections",
]

WASM_EXE_LDFLAGS_SHARED += WASM_WASI_LIBC_LDFLAGS

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
# WARNING: do NOT import this method directly, instead use the getter method:
# get_faasm_build_env_dict
_FAASM_BUILD_ENV_DICT = {
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
    "FAASM_WASM_CXXFLAGS": " ".join(WASM_CXXFLAGS),
    "FAASM_WASM_CXXFLAGS_SHARED": " ".join(WASM_CXXFLAGS_SHARED),
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


def get_faasm_build_env_dict(is_threads=False):
    """
    This method returns the right set of environment variables needed to use
    our toolchain file as well as most cross-compilation scripts in Faasm.
    """
    build_env_dicts = _FAASM_BUILD_ENV_DICT
    if is_threads:
        wasm_triple = "wasm32-wasi-threads"
        build_env_dicts["FAASM_WASM_TRIPLE"] = wasm_triple
        build_env_dicts["FAASM_WASM_CFLAGS"] += " --target={} -pthread".format(
            wasm_triple
        )
        build_env_dicts[
            "FAASM_WASM_CXXFLAGS"
        ] += " --target={} -pthread".format(wasm_triple)
        linker_features = [
            "atomics",
            "bulk-memory",
            "mutable-globals",
            "sign-ext",
            "simd128",
        ]
    else:
        wasm_triple = "wasm32-wasi"
        build_env_dicts["FAASM_WASM_TRIPLE"] = wasm_triple
        linker_features = [
            "bulk-memory",
            "mutable-globals",
            "sign-ext",
            "simd128",
        ]

    build_env_dicts["FAASM_WASM_HEADER_INSTALL_DIR"] = join(
        WASM_SYSROOT, "include", wasm_triple
    )
    build_env_dicts["FAASM_WASM_LIB_INSTALL_DIR"] = join(
        WASM_SYSROOT, "lib", wasm_triple
    )
    build_env_dicts[
        "FAASM_WASM_EXE_LINKER_FLAGS"
    ] += " -Xlinker --features={}".format(",".join(linker_features))

    return build_env_dicts


def get_dict_as_cmake_vars(env_dict):
    return " ".join(["-D{}={}".format(k, env_dict[k]) for k in env_dict])


# ----------
# Scripts for configure, automake, and autotools
# ----------


def build_config_cmd(env_vars, cmd, shared=False, cxx=False, conf_args=True):
    """
    Wraps an autotools command in the relevant environment variables and
    cross-compilation config. We need to call this method after we get
    the right env. vars using get_faasm_build_env_dict
    """
    base_config_cmd = [
        "CC={}".format(env_vars["FAASM_WASM_CC"]),
        "CXX={}".format(env_vars["FAASM_WASM_CXX"]),
        "AR={}".format(env_vars["FAASM_WASM_AR"]),
        "RANLIB={}".format(env_vars["FAASM_WASM_RANLIB"]),
        'CFLAGS="--target={} {}"'.format(
            env_vars["FAASM_WASM_TRIPLE"], env_vars["FAASM_WASM_CFLAGS"]
        ),
        'CXXFLAGS="--target={} {}"'.format(
            env_vars["FAASM_WASM_TRIPLE"], env_vars["FAASM_WASM_CXXFLAGS"]
        ),
        'CCSHARED="{}"'.format(env_vars["FAASM_WASM_CFLAGS_SHARED"]),
        'CXXSHARED="{}"'.format(env_vars["FAASM_WASM_CXXFLAGS_SHARED"]),
    ]

    if cxx:
        base_config_cmd += [
            "LD={}".format(env_vars["FAASM_WASM_CXX"]),
            'LDFLAGS="-target {} {}"'.format(
                env_vars["FAASM_WASM_TRIPLE"],
                env_vars["FAASM_WASM_STATIC_LINKER_FLAGS"],
            ),
        ]
    else:
        base_config_cmd += [
            "LD={}".format(env_vars["FAASM_WASM_CC"]),
            'LDFLAGS="-target {} {}"'.format(
                env_vars["FAASM_WASM_TRIPLE"],
                env_vars["FAASM_WASM_STATIC_LINKER_FLAGS"],
            ),
        ]

    base_config_cmd += cmd

    return base_config_cmd


def run_autotools(env_vars, proj_dir):
    """
    Runs through autotools set-up on the given directory
    """

    def _run_auto_cmd(cmd):
        print("Running {}".format(cmd))
        auto_cmd = build_config_cmd(env_vars, [cmd], conf_args=False)
        auto_cmd = " ".join(auto_cmd)
        run(auto_cmd, shell=True, check=True, cwd=proj_dir)

    _run_auto_cmd("libtoolize")
    _run_auto_cmd("aclocal")
    _run_auto_cmd("autoheader")
    _run_auto_cmd("autoconf")
    _run_auto_cmd("automake --add-missing")
