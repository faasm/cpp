from os.path import dirname, abspath, join
from os import getcwd
from subprocess import run

PROJ_ROOT = dirname(dirname(abspath(__file__)))


def main():
    version_file = join(PROJ_ROOT, "VERSION")
    with open(version_file) as fh:
        sysroot_ver = fh.read().strip()

    image_tag = "faasm/cpp-sysroot:{}".format(sysroot_ver)

    cwd = getcwd()
    print("Running {} at {}".format(image_tag, cwd))

    docker_cmd = [
        'docker run --entrypoint="/bin/bash"',
        '--network="host"',
        '-e "TERM=xterm-256color"',
        "-v {}:/work".format(cwd),
        "-w /work",
        "-it",
        image_tag,
    ]

    docker_cmd = " ".join(docker_cmd)
    run(docker_cmd, shell=True, check=True, cwd=cwd)


if __name__ == "__main__":
    main()
