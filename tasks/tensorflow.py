from os import cpu_count
from os.path import exists, isfile, join
from shutil import copytree, rmtree
from subprocess import check_output, run
from invoke import task
from faasmtools.env import (
    THIRD_PARTY_DIR,
)
from faasmtools.build import (
    BASE_CONFIG_CMD,
    WASM_CFLAGS,
    WASM_CXXFLAGS,
    WASM_HOST,
    WASM_LDFLAGS,
    WASM_HEADER_INSTALL,
    WASM_LIB_INSTALL,
)


@task(default=True)
def lite(ctx, clean=False):
    """
    Compile and install Tensorflow Lite lib
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

    make_target = "lib"
    make_cmd = ["make -j {}".format(make_cores)]
    make_cmd.extend(BASE_CONFIG_CMD)
    make_cmd.extend(
        [
            'CFLAGS="{} -ftls-model=local-exec"'.format(" ".join(WASM_CFLAGS)),
            'CXXFLAGS="{}"'.format(" ".join(WASM_CXXFLAGS)),
            'LDFLAGS="{} -Xlinker --max-memory=4294967296"'.format(
                " ".join(WASM_LDFLAGS)
            ),
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

    make_cmd = " ".join(make_cmd)
    run(make_cmd, shell=True, check=True, cwd=tf_lite_dir)

    # Install static library
    tf_lib_dir = join(clean_dir, "lib")
    cp_cmd = "cp {}/libtensorflow-lite.a {}/libtensorflow-lite.a".format(
        tf_lib_dir, WASM_LIB_INSTALL
    )
    print(cp_cmd)
    run(cp_cmd, shell=True, check=True)

    # Install header files
    header_install_dir = join(WASM_HEADER_INSTALL, "tensorflow")
    if exists(header_install_dir):
        rmtree(header_install_dir)

    def ignore_func(d, files):
        return [f for f in files if isfile(join(d, f)) and f[-2:] != ".h"]

    copytree(tf_dir, header_install_dir, ignore=ignore_func)
