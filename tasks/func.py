from base64 import b64encode
from faasmctl.util.flush import flush_workers
from faasmctl.util.invoke import invoke_wasm
from faasmctl.util.upload import upload_wasm
from faasmtools.env import PROJ_ROOT
from faasmtools.compile_util import wasm_cmake, wasm_copy_upload
from invoke import task
from os import makedirs, listdir
from os.path import join, exists, splitext
from shutil import rmtree
from subprocess import run

FUNC_DIR = join(PROJ_ROOT, "func")
FUNC_BUILD_DIR = join(PROJ_ROOT, "build", "func")
NATIVE_FUNC_BUILD_DIR = join(PROJ_ROOT, "build", "native-func")


def _is_threaded_func(user, func):
    """
    Work out if the function requires using the wasm32-wasi or the
    wasm32-wasi-threads target
    """
    return user in ["threads", "omp"]


def _get_all_user_funcs(user):
    # Work out all the functions for this user (that we assume will have been
    # built)
    funcs = list()
    for func_file in listdir(join(FUNC_BUILD_DIR, user)):
        name, ext = splitext(func_file)
        if ext != ".wasm":
            continue

        funcs.append(name)

    return funcs


def _copy_built_function(user, func):
    src_file = join(FUNC_BUILD_DIR, user, ".".join([func, "wasm"]))
    wasm_copy_upload(user, func, src_file)


@task(default=True, name="compile")
def compile(ctx, user, func, clean=False, debug=False, native=False):
    """
    Compile a function
    """
    if native:
        if exists(NATIVE_FUNC_BUILD_DIR) and clean:
            rmtree(NATIVE_FUNC_BUILD_DIR)

        makedirs(NATIVE_FUNC_BUILD_DIR, exist_ok=True)

        build_cmd = ["cmake", "-GNinja", FUNC_DIR]

        build_cmd = " ".join(build_cmd)
        print(build_cmd)
        run(
            "cmake -GNinja {}".format(FUNC_DIR),
            check=True,
            shell=True,
            cwd=NATIVE_FUNC_BUILD_DIR,
        )

        run(
            "ninja {}".format(func),
            shell=True,
            check=True,
            cwd=NATIVE_FUNC_BUILD_DIR,
        )
    else:
        # Build the function (gets written to the build dir)
        wasm_cmake(FUNC_DIR, FUNC_BUILD_DIR, func, clean, debug, _is_threaded_func(user, func))

        # Copy into place
        _copy_built_function(user, func)


@task
def upload(ctx, user, func):
    """
    Upload a compiled function
    """
    func_file = join(FUNC_BUILD_DIR, user, "{}.wasm".format(func))
    upload_wasm(user, func, func_file)


@task
def upload_user(ctx, user):
    """
    Upload all compiled functions for a user
    """
    funcs = _get_all_user_funcs(user)
    for f in funcs:
        upload(ctx, user, f)


@task
def invoke(ctx, user, func, input_data=None, mpi=None, graph=False):
    """
    Invoke a given function
    """
    # Prepare Faasm message
    data = {
        "function": func,
        "user": user,
    }

    if input_data:
        data["input_data"] = b64encode(input_data.encode("utf-8")).decode(
            "utf-8"
        )

    if mpi is not None:
        data["mpi_world_size"] = int(mpi)

    if graph:
        data["record_exec_graph"] = True
        data["async"] = True

    # Invoke message
    response = invoke_wasm(data)

    print("Success:\n{}".format(response.messageResults[0].outputData))


@task
def update(ctx, user, func, clean=False, debug=False, native=False):
    """
    Combined compile, upload, flush
    """
    compile(ctx, user, func, clean=clean)

    upload(ctx, user, func)

    flush(ctx)


@task
def flush(ctx):
    """
    Flush the Faasm cluster
    """
    flush_workers()


@task
def user(ctx, user, clean=False, debug=False):
    """
    Compile all functions belonging to the given user
    """
    # Build all funcs for this user (will fail if any builds fail)
    target = "{}_all_funcs".format(user)
    wasm_cmake(FUNC_DIR, FUNC_BUILD_DIR, target, clean, debug, _is_threaded_func(user, ""))

    funcs = _get_all_user_funcs(user)
    for f in funcs:
        _copy_built_function(user, f)


@task
def local(ctx, clean=False, debug=False):
    """
    Compile all functions used in the tests
    """
    user(ctx, "demo", clean, debug)
    user(ctx, "errors", clean, debug)
    user(ctx, "mpi", clean, debug)
    user(ctx, "omp", clean, debug)
