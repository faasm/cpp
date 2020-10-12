from invoke import Collection

from . import container
from . import git
from . import libc

ns = Collection(
    container,
    git,
    libc,
)
