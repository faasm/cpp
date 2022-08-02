from os import cpu_count, makedirs, walk

from os.path import exists, join

from shutil import rmtree

from subprocess import call, check_output

from invoke import task

from faasmtools.env import (
    THIRD_PARTY_DIR,
    PROJ_ROOT,
)

from faasmtools.compile_util import wasm_cmake, wasm_copy_upload


from faasmtools.build import (
    BASE_CONFIG_CMD,
    WASM_CFLAGS,
    WASM_CXXFLAGS,
    WASM_HOST,
    WASM_LDFLAGS,
)

FUNC_DIR = join(PROJ_ROOT, "func")
FUNC_BUILD_DIR = join(PROJ_ROOT, "build", "func")

@task
def func(ctx, clean=False):
    wasm_cmake(
        FUNC_DIR, FUNC_BUILD_DIR, "image", clean=clean
    )

    wasm_file = join(FUNC_BUILD_DIR, "tf", "image.wasm")
    wasm_copy_upload("tf", "image", wasm_file)

@task
def lib(ctx, clean=False):
    """
    Compile and install Tensorflow Lite
    """
    tf_dir = join(THIRD_PARTY_DIR, "tensorflow")
    tf_lite_dir = join(tf_dir, "tensorflow", "lite")
    tf_make_dir = join(tf_lite_dir, "tools", "make")

    download_check_dir = join(tf_make_dir, "downloads", "absl")
    if not exists(download_check_dir):
        download_script = join(tf_make_dir, "download_dependencies.sh")
        check_output(download_script, shell=True)

    cores = cpu_count()
    make_cores = int(cores) - 1

    """
    new stuff
    """
    make_target = "lib"
    make_cmd = ["make -j {}".format(make_cores)]
    make_cmd.extend(BASE_CONFIG_CMD)
    make_cmd.extend(
        [
            'CFLAGS="{} -ftls-model=local-exec"'.format(" ".join(WASM_CFLAGS)),
            'CXXFLAGS="{}"'.format(" ".join(WASM_CXXFLAGS)),
            'LDFLAGS="{} -Xlinker --max-memory=4294967296"'.format(" ".join(
                WASM_LDFLAGS
            )),
            "MINIMAL_SRCS=",
            "TARGET={}".format(WASM_HOST),
            "BUILD_WITH_MMAP=false",
            'LIBS="-lstdc++"',
            '-C "{}"'.format(tf_dir),
            "-f tensorflow/lite/tools/make/Makefile",
        ]
    )

    make_cmd.append(make_target)

    clean_dir = join(tf_make_dir, "gen", "wasm32-unknown-wasi_x86_64")
    if clean and exists(clean_dir):
        rmtree(clean_dir)

    res = call(" ".join(make_cmd), shell=True, cwd=tf_lite_dir)
    if res != 0:
        raise RuntimeError("Failed to compile Tensorflow lite")
