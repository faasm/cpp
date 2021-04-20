from invoke import task
from tasks.lib import build_faasm_lib


@task(default=True)
def build(ctx, clean=False, native=False, shared=False):
    """
    Builds faasm C/C++ lib
    """
    build_faasm_lib("libfaasm", clean=clean, native=native, shared=shared)
