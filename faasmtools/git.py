from subprocess import run


def tag_project(tag_name, proj_dir, force=False):
    run(
        "git tag {} {}".format("--force" if force else "", tag_name),
        shell=True,
        check=True,
        cwd=proj_dir,
    )

    run(
        "git push {} origin {}".format("--force" if force else "", tag_name),
        shell=True,
        check=True,
        cwd=proj_dir,
    )
