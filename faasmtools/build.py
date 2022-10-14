from copy import copy
from os.path import join
from subprocess import run
from os import environ

# Directories
FAASM_LOCAL_DIR = environ.get("FAASM_LOCAL_DIR", "/usr/local/faasm")
FAASM_NATIVE_DIR = join(FAASM_LOCAL_DIR, "native")
WASM_SYSROOT = join(FAASM_LOCAL_DIR, "llvm-sysroot")
WASM_HEADER_INSTALL = "{}/include".format(WASM_SYSROOT)
WASM_LIB_INSTALL = "{}/lib/wasm32-wasi".format(WASM_SYSROOT)
WASM_TOOLCHAIN_ROOT = "/usr/local/faasm/toolchain"
WASM_TOOLCHAIN_TOOLS = join(WASM_TOOLCHAIN_ROOT, "tools")
WASM_TOOLCHAIN_BIN = join(WASM_TOOLCHAIN_ROOT, "bin")

# Toolchain files
CMAKE_TOOLCHAIN_FILE = join(WASM_TOOLCHAIN_TOOLS, "WasiToolchain.cmake")

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
WASM_HOST_SHARED = "wasm32-unknown-emscripten"
WASM_HOST_UNKNOWN = "wasm32-unknown-unknown"

# CFLAGS
WASM_CFLAGS = [
    "-O3 -mno-atomics",
    "--sysroot={}".format(WASM_SYSROOT),
    "-m32",
    "-DANSI",
    "-D__faasm",
]

WASM_CXXFLAGS = WASM_CFLAGS

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

# Flags for static libraries
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
    "-Xlinker --export=__wasm_call_ctors",
    "-Xlinker --max-memory=4294901760",
    "-Wl,-z,stack-size=4194304 -Wl",
]

# Flags for shared libraries
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
