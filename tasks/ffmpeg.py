from subprocess import run

from os.path import join

from invoke import task

from faasmtools.env import (
    THIRD_PARTY_DIR,
)

from faasmtools.build import (
    WASM_SYSROOT,
    WASM_LIB_INSTALL,
    WASM_CC,
    WASM_CXX,
    WASM_AR,
    WASM_NM,
    WASM_RANLIB,
    WASM_CFLAGS,
    WASM_LDFLAGS,
)


@task(default=True)
def ffmpeg(ctx, clean=False):
    """
    Compile and install FFmpeg
    """
    ffmpeg_dir = join(THIRD_PARTY_DIR, "FFmpeg")

    if clean:
        run("make clean", shell=True, cwd=ffmpeg_dir, check=True)

    # List of flags inspired from the github project:
    # https://github.com/ffmpegwasm/ffmpeg.wasm-core
    configure_cmd = [
        "./configure",
        "--prefix={}".format(WASM_SYSROOT),
        "--libdir={}".format(WASM_LIB_INSTALL),
        "--target-os=none",
        "--arch=x86_32",
        "--enable-cross-compile",
        "--disable-x86asm",
        "--disable-inline-asm",
        "--disable-network",
        "--disable-stripping",
        "--disable-programs",
        "--disable-doc",
        "--extra-cflags='{}'".format(" ".join(WASM_CFLAGS)),
        "--extra-cxxflags='{}'".format(" ".join(WASM_CFLAGS)),
        "--extra-ldflags='{}'".format(" ".join(WASM_LDFLAGS)),
        "--nm={}".format(WASM_NM),
        "--ar={}".format(WASM_AR),
        "--ranlib={}".format(WASM_RANLIB),
        "--cc={}".format(WASM_CC),
        "--cxx={}".format(WASM_CXX),
        "--objcc={}".format(WASM_CC),
        "--dep-cc={}".format(WASM_CC),
    ]

    configure_cmd = " ".join(configure_cmd)
    run(configure_cmd, shell=True, cwd=ffmpeg_dir, check=True)

    run("make -j", shell=True, cwd=ffmpeg_dir, check=True)

    run("make install", shell=True, cwd=ffmpeg_dir, check=True)
