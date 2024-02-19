from invoke import task
from tasks.lib import build_faasm_lib


@task(default=True)
def build(ctx, clean=False, native=False, shared=False):
    """
    Builds Faasm OpenMP library
    """
    build_faasm_lib("libfaasmp", clean=clean, native=native, shared=shared, threads=True)
