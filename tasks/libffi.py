from faasmtools.build import (
    build_config_cmd,
    get_faasm_build_env_dict,
    run_autotools,
)
from faasmtools.env import THIRD_PARTY_DIR, USABLE_CPUS
from invoke import task
from os.path import join, exists
from subprocess import run

LIBFFI_DIR = join(THIRD_PARTY_DIR, "libffi")


@task(default=True)
def build(ctx, clean=False, threads=False):
    """
    Builds libffi
    """
    if exists(join(LIBFFI_DIR, "Makefile")) and clean:
        run("make clean", cwd=LIBFFI_DIR, shell=True, check=True)

    build_env = get_faasm_build_env_dict(threads)

    # Autotools
    run_autotools(build_env, LIBFFI_DIR)

    # Configure
    configure_cmd = build_config_cmd(
        build_env,
        [
            "./configure",
            "--build=wasm32",
            "--host=wasm32-wasi",
            "--includedir={}".format(
                build_env["FAASM_WASM_HEADER_INSTALL_DIR"]
            ),
            "--libdir={}".format(build_env["FAASM_WASM_LIB_INSTALL_DIR"]),
            "--disable-multi-os-directory",
            "--disable-docs",
        ],
    )
    configure_cmd = " ".join(configure_cmd)
    run(configure_cmd, check=True, shell=True, cwd=LIBFFI_DIR)

    # Run make
    make_cmd = ["make", "-j {}".format(USABLE_CPUS)]
    run(" ".join(make_cmd), shell=True, check=True, cwd=LIBFFI_DIR)

    # Run install
    run("make install", shell=True, check=True, cwd=LIBFFI_DIR)
