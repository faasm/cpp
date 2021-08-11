from os import listdir
from os.path import join, splitext

from invoke import task

import requests

from faasmtools.env import PROJ_ROOT
from faasmtools.compile_util import wasm_cmake, wasm_copy_upload

FUNC_DIR = join(PROJ_ROOT, "func")
FUNC_BUILD_DIR = join(PROJ_ROOT, "build", "func")


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
def compile(ctx, user, func, clean=False, debug=False):
    """
    Compile a function
    """
    # Build the function (gets written to the build dir)
    wasm_cmake(FUNC_DIR, FUNC_BUILD_DIR, func, clean, debug)

    # Copy into place
    _copy_built_function(user, func)


@task()
def upload(ctx, user, func, host="upload", port=8002):
    """
    Upload a compiled function
    """
    func_file = join(FUNC_BUILD_DIR, user, "{}.wasm".format(func))
    url = "http://{}:{}/f/{}/{}".format(host, port, user, func)
    response = requests.put(url, data=open(func_file, "rb"))

    print("Response {}: {}".format(response.status_code, response.text))


@task()
def upload_user(ctx, user, host="upload", port=8002):
    """
    Upload all compiled functions for a user
    """
    funcs = _get_all_user_funcs(user)
    for f in funcs:
        upload(ctx, user, f, host=host, port=port)


@task()
def invoke(ctx, user, func, input_data=None, host="worker", port=8080):
    """
    Invoke a given function
    """
    url = "http://{}:{}".format(host, port)
    data = {
        "function": func,
        "user": user,
    }

    if input_data:
        data["input_data"] = input_data

    response = requests.post(url, json=data)

    if response.status_code != 200:
        print("Error ({}):\n{}".format(response.status_code, response.text))
        exit(1)

    print("Success:\n{}".format(response.text))


@task()
def flush(host="worker", port=8080):
    url = "http://{}:{}".format(host, port)
    data = {"type": "flush"}
    response = requests.post(url, json=data)

    print("Response {}: {}".format(response.status_code, response.text))


@task
def user(ctx, user, clean=False, debug=False):
    """
    Compile all functions belonging to the given user
    """
    # Build all funcs for this user (will fail if any builds fail)
    target = "{}_all_funcs".format(user)
    wasm_cmake(FUNC_DIR, FUNC_BUILD_DIR, target, clean, debug)

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
    user(ctx, "ffmpeg", clean, debug)
    user(ctx, "mpi", clean, debug)
    user(ctx, "omp", clean, debug)
