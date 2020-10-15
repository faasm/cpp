from subprocess import run


def tag_project(tag_name, proj_dir):
    run("git tag {}".format(tag_name), shell=True, check=True, cwd=proj_dir)

    run(
        "git push origin {}".format(tag_name),
        shell=True,
        check=True,
        cwd=proj_dir,
    )
