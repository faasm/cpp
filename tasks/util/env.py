from os.path import dirname, join, realpath

PROJ_ROOT = dirname(dirname(dirname(realpath(__file__))))

PATCHES_DIR = join(PROJ_ROOT, "patches")
