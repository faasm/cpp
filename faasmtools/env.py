from os.path import dirname, abspath, join
from multiprocessing import cpu_count

PROJ_ROOT = dirname(dirname(abspath(__file__)))
THIRD_PARTY_DIR = join(PROJ_ROOT, "third-party")
WASM_DIR = "/usr/local/faasm/wasm"
FAASM_RUNTIME_ROOT = "/usr/local/faasm/runtime_root"

# Environment
USABLE_CPUS = int(cpu_count()) - 1

# Versioning
VERSION_FILE = join(PROJ_ROOT, "VERSION")
LLVM_VERSION = "10.0.1"


def get_version():
    with open(VERSION_FILE) as fh:
        ver = fh.read()

    return ver.strip()
