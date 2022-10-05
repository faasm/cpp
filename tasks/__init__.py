from invoke import Collection

from . import clapack
from . import docker
from . import ffmpeg
from . import func
from . import git
from . import install
from . import libc
from . import libemscripten
from . import libfaasm
from . import libfaasmp
from . import libfaasmpi
from . import libfake
from . import libffi
from . import tensorflow

ns = Collection(
    clapack,
    docker,
    ffmpeg,
    func,
    git,
    install,
    libc,
    libemscripten,
    libfaasm,
    libfaasmp,
    libfaasmpi,
    libfake,
    libffi,
    tensorflow,
)
