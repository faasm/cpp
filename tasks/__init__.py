from invoke import Collection

from . import clapack
from . import container
from . import eigen
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

ns = Collection(
    clapack,
    container,
    eigen,
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
)
