from invoke import Collection

from . import clapack
from . import container
from . import dev
from . import eigen
from . import git
from . import install
from . import libc
from . import libfaasm
from . import libfaasmp
from . import libfaasmpi
from . import libffi

ns = Collection(
    clapack,
    container,
    dev,
    eigen,
    git,
    install,
    libc,
    libfaasm,
    libfaasmp,
    libfaasmpi,
    libffi,
)
