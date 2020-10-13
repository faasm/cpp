from invoke import Collection

from . import container
from . import eigen
from . import git
from . import libc

ns = Collection(
    container,
    eigen,
    git,
    libc,
)
