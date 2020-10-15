from os.path import exists, join
from os import makedirs
from invoke import task
from shutil import copyfile

from faasmtools.build import (
    WASM_TOOLCHAIN_TOOLS,
)

from faasmtools.env import (
    PROJ_ROOT,
)

INSTALL_FILES = [
    "WasiToolchain.cmake",
    "Makefile.envs",
]


@task(default=True)
def install(ctx):
    """
    Installs tool files in toolchain root
    """
    if not exists(WASM_TOOLCHAIN_TOOLS):
        makedirs(WASM_TOOLCHAIN_TOOLS)

    for filename in INSTALL_FILES:
        file_path = join(PROJ_ROOT, filename)
        dest_path = join(WASM_TOOLCHAIN_TOOLS, filename)

        print("Copying from {} to {}".format(file_path, dest_path))
        copyfile(file_path, dest_path)
