from subprocess import run

from invoke import task

from tasks.env import (
    get_version,
    PROJ_ROOT,
)


def _do_tag(tag_name):
    run("git tag {}".format(tag_name), shell=True, check=True, cwd=PROJ_ROOT)

    run(
        "git push origin {}".format(tag_name),
        shell=True,
        check=True,
        cwd=PROJ_ROOT,
    )


@task
def tag(ctx):
    """
    Creates git tag from the current tree
    """
    version = get_version()

    # Create tag
    tag_name = "v{}".format(version)
    _do_tag(tag_name)

