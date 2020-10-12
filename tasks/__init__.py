from invoke import Collection

from . import container
from . import git

ns = Collection(
    container,
    git,
)
