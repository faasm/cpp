from invoke import Collection

from . import clapack
from . import docker
from . import format_code
from . import func
from . import git
from . import install
from . import libemscripten
from . import libfaasm
from . import libfaasmp
from . import libfaasmpi
from . import libfake
from . import libffi
from . import llvm
from . import zlib

ns = Collection(
    clapack,
    docker,
    format_code,
    func,
    git,
    install,
    libemscripten,
    libfaasm,
    libfaasmp,
    libfaasmpi,
    libfake,
    libffi,
    llvm,
    zlib,
)
