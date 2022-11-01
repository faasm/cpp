from faasmtools.build import CMAKE_TOOLCHAIN_FILE, FAASM_BUILD_ENV_DICT
from faasmtools.compile_util import wasm_copy_upload
from faasmtools.env import THIRD_PARTY_DIR
from invoke import task
from os import environ
from os.path import join
from subprocess import run


@task(default=True)
def build(ctx, clean=False, native=False):
    """
    Build the different kernels functions for OpenMP and MPI

    Note that LAMMPS is a self-contained binary, and different workloads are
    executed by passing different command line arguments. As a consequence,
    we cross-compile it and copy the binary (lmp) to lammps/main/function.wasm
    """
    kernels_dir = join(THIRD_PARTY_DIR, "Kernels")

    if clean:
        run("make clean", shell=True, check=True, cwd=kernels_dir)

    work_env = environ.copy()
    work_env.update(FAASM_BUILD_ENV_DICT)

    # Build the MPI kernels
    mpi_kernel_targets = [
        ("MPI1/Synch_global", "global"),
        ("MPI1/Synch_p2p", "p2p"),
        ("MPI1/Sparse", "sparse"),
        ("MPI1/Transpose", "transpose"),
        ("MPI1/Stencil", "stencil"),
        ("MPI1/DGEMM", "dgemm"),
        ("MPI1/Nstream", "nstream"),
        ("MPI1/Reduce", "reduce"),
        ("MPI1/Random", "random"),
    ]
    for subdir, make_target in mpi_kernel_targets:
        make_cmd = "make {}".format(make_target)
        make_dir = join(kernels_dir, subdir)
        run(make_cmd, shell=True, check=True, cwd=make_dir, env=work_env)
        wasm_copy_upload("kernels-mpi", make_target, join(kernels_dir, "wasm", "{}.wasm".format(make_target)))

    # Clean MPI wasm files
    run("make clean", shell=True, check=True, cwd=kernels_dir)

    # Build the OMP kernels
    omp_kernel_targets = [
        ("OPENMP/Synch_global", "global"),
        ("OPENMP/Synch_p2p", "p2p"),
        ("OPENMP/Sparse", "sparse"),
        ("OPENMP/DGEMM", "dgemm"),
        ("OPENMP/Nstream", "nstream"),
        ("OPENMP/Reduce", "reduce"),
        # TODO: build fails for these kernels
        # ("OPENMP/Stencil", "stencil"),
        # ("OPENMP/Random", "random"),
        # ("OPENMP/Transpose", "transpose"),
        # ("OPENMP/PIC", "pic"),

    ]
    for subdir, make_target in omp_kernel_targets:
        make_cmd = "make {}".format(make_target)
        make_dir = join(kernels_dir, subdir)
        run(make_cmd, shell=True, check=True, cwd=make_dir, env=work_env)
        wasm_copy_upload("kernels-omp", make_target, join(kernels_dir, "wasm", "{}.wasm".format(make_target)))
