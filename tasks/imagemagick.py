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
def imagemagick(ctx, clean=False):
    """
    Compile and install ImageMagick
    """
    imagemagick_dir = join(THIRD_PARTY_DIR, "ImageMagick")

    if clean:
        run("make clean", shell=True, cwd=ffmpeg_dir, check=True)

    # List of flags inspired from the github project:
    # https://github.com/KnicKnic/WASM-ImageMagick
    # For all the configure options, see:
    # https://github.com/ImageMagick/ImageMagick/blob/main/Install-unix.txt
#     configure_cmd = [
#         "./configure",
#         "--prefix={}".format(WASM_SYSROOT),
#         "--libdir={}".format(WASM_LIB_INSTALL),
#         "--target-os=none",
#         "--arch=x86_32",
#         "--enable-cross-compile",
#         "--disable-x86asm",
#         "--disable-inline-asm",
#         "--disable-network",
#         "--disable-stripping",
#         "--disable-programs",
#         "--disable-doc",
#         "--extra-cflags='{}'".format(" ".join(WASM_CFLAGS)),
#         "--extra-cxxflags='{}'".format(" ".join(WASM_CFLAGS)),
#         "--extra-ldflags='{}'".format(" ".join(WASM_LDFLAGS)),
#         "--nm={}".format(WASM_NM),
#         "--ar={}".format(WASM_AR),
#         "--ranlib={}".format(WASM_RANLIB),
#         "--cc={}".format(WASM_CC),
#         "--cxx={}".format(WASM_CXX),
#         "--objcc={}".format(WASM_CC),
#         "--dep-cc={}".format(WASM_CC),
#     ]
    configure_cmd = [
        "./configure",
        "CC={}".format(WASM_CC),
        "CXX={}".format(WASM_CXX),
        "CFLAGS='{}'".format(" ".join(WASM_CFLAGS)),
        "CXXFLAGS='{}'".format(" ".join(WASM_CFLAGS)),
        "LDFLAGS='{}'".format(" ".join(WASM_LDFLAGS)),
        "AR={}".format(WASM_AR),
        "RANLIB={}".format(WASM_RANLIB),
        "NM={}".format(WASM_NM),
        "--prefix={}".format(WASM_SYSROOT),
        "--disable-largefile",
        "--disable-openmp",
        "--disable-shared",
        "--host=none",
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
        "--without-zlib",
    ]
        # --disable-shared --without-threads --without-magick-plus-plus --without-perl --without-x --disable-largefile --disable-openmp --without-bzlib --without-dps --without-freetype --without-jbig --without-openjp2 --without-lcms --without-wmf --without-xml --without-fftw --without-flif --without-fpx --without-djvu --without-fontconfig --without-raqm --without-gslib --without-gvc --without-heic --without-lqr --without-openexr --without-pango --without-raw --without-rsvg --without-webp --without-xml PKG_CONFIG_PATH="/code/libpng:/code/zlib:/code/libjpeg:/code/libtiff:/code/libtiff/libtiff:"

    configure_cmd = " ".join(configure_cmd)
    run(configure_cmd, shell=True, cwd=imagemagick_dir, check=True)

    run("make -j", shell=True, cwd=imagemagick_dir, check=True)

    # run("make install", shell=True, cwd=ffmpeg_dir, check=True)
