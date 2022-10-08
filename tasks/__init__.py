from invoke import Collection

from . import clapack
from . import docker
from . import ffmpeg
from . import func
from . import git
from . import imagemagick
from . import install
from . import libc
from . import libemscripten
from . import libfaasm
from . import libfaasmp
from . import libfaasmpi
from . import libfake
from . import libffi
from . import libpng
from . import tensorflow
from . import zlib

ns = Collection(
    clapack,
    docker,
    ffmpeg,
    func,
    git,
    imagemagick,
    install,
    libc,
    libemscripten,
    libfaasm,
    libfaasmp,
    libfaasmpi,
    libfake,
    libffi,
    libpng,
    tensorflow,
    zlib,
)
