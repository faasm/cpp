from faasmtools.env import (
    THIRD_PARTY_DIR,
)
from faasmtools.build import (
    WASM_AR,
    WASM_CC,
    WASM_CFLAGS,
    WASM_CXX,
    WASM_EXE_LDFLAGS,
    WASM_HOST,
    WASM_LD,
    WASM_NM,
    WASM_RANLIB,
    WASM_SYSROOT,
)
from faasmtools.compile_util import wasm_copy_upload
from invoke import task
from os.path import join
from subprocess import run


@task(default=True)
def imagemagick(ctx, clean=False):
    """
    Compile and install ImageMagick
    """
    imagemagick_dir = join(THIRD_PARTY_DIR, "ImageMagick")

    if clean:
        run("make clean", shell=True, cwd=imagemagick_dir, check=True)

    # List of flags inspired from the github project:
    # https://github.com/KnicKnic/WASM-ImageMagick
    # For all the configure options, see:
    # https://github.com/ImageMagick/ImageMagick/blob/main/Install-unix.txt
    configure_cmd = [
        "./configure",
        "CC={}".format(WASM_CC),
        "CXX={}".format(WASM_CXX),
        "CFLAGS='{}'".format(" ".join(WASM_CFLAGS)),
        "CXXFLAGS='{}'".format(" ".join(WASM_CFLAGS)),
        "LD={}".format(WASM_LD),
        "LDFLAGS='{}'".format(" ".join(WASM_EXE_LDFLAGS)),
        "CXXLDFLAGS='{}'".format(" ".join(WASM_EXE_LDFLAGS)),
        "AR={}".format(WASM_AR),
        "RANLIB={}".format(WASM_RANLIB),
        "NM={}".format(WASM_NM),
        "PKG_CONFIG_PATH={}".format(join(THIRD_PARTY_DIR, "libpng")),
        "--prefix={}".format(WASM_SYSROOT),
        "--disable-largefile",
        "--disable-openmp",
        "--disable-shared",
        "--host={}".format(WASM_HOST),
        "--with-png=yes",
        "--enable-delegate-build",
        "--without-bzlib",
        "--without-dps",
        "--without-djvu",
        "--without-fftw",
        "--without-flif",
        "--without-fpx",
        "--without-fontconfig",
        "--without-freetype",
        "--without-gslib",
        "--without-gvc",
        "--without-heic",
        "--without-jbig",
        "--without-lcms",
        "--without-lqr",
        "--without-magick-plus-plus",
        "--without-openexr",
        "--without-openjp2",
        "--without-pango",
        "--without-perl",
        "--without-raqm",
        "--without-raw",
        "--without-rsvg",
        "--without-threads",
        "--without-webp",
        "--without-wmf",
        "--without-x",
        "--without-xml",
    ]

    configure_cmd = " ".join(configure_cmd)
    run(configure_cmd, shell=True, cwd=imagemagick_dir, check=True)

    run("make -j", shell=True, cwd=imagemagick_dir, check=True)

    # Instead of installing ImageMagick, we copy the self-contained binary
    # (magick) to /usr/local/faasm/wasm/imagemagick/main/function.wasm
    wasm_copy_upload(
        "imagemagick", "main", join(imagemagick_dir, "utilities", "magick")
    )
