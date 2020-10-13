
@task
def eigen(ctx, verbose=False):
    """
    Compile and install Eigen
    """
    work_dir = join(THIRD_PARTY_DIR, "eigen")
    build_dir = join(PROJ_ROOT, "build", "eigen")

    if exists(build_dir):
        rmtree(build_dir)
    makedirs(build_dir)

    verbose_string = "VERBOSE=1" if verbose else ""

    cmd = [
        verbose_string,
        "cmake",
        "-DFAASM_BUILD_TYPE=wasm",
        "-DCMAKE_TOOLCHAIN_FILE={}".format(FAASM_TOOLCHAIN_FILE),
        "-DCMAKE_BUILD_TYPE=Release",
        "-DCMAKE_INSTALL_PREFIX={}".format(SYSROOT_INSTALL_PREFIX),
        work_dir,
    ]
    cmd_string = " ".join(cmd)

    run(cmd_string, shell=True, cwd=build_dir, check=True)

    run(
        "{} make install".format(verbose_string),
        shell=True,
        cwd=build_dir,
        check=True,
    )
