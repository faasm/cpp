from invoke import task
from tasks.lib import build_faasm_lib


@task(default=True)
def build(ctx, clean=False):
    """
    Builds Faasm emscripten support library
    """
    build_faasm_lib("libemscripten", clean=clean, native=False, shared=False)
