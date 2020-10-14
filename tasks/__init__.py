from invoke import Collection

from . import clapack
from . import container
from . import eigen
from . import git
from . import install
from . import libc
from . import libffi

ns = Collection(
    clapack,
    container,
    eigen,
    git,
    install,
    libc,
    libffi,
)
