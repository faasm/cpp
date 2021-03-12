from invoke import Collection

from . import clapack
from . import container
from . import dev
from . import eigen
from . import examples
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
    dev,
    eigen,
    examples,
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
