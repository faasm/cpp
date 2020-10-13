from invoke import Collection

from . import container
from . import eigen
from . import git
from . import libc
from . import libffi

ns = Collection(
    container,
    eigen,
    git,
    libc,
    libffi,
)
