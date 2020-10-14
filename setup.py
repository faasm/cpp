from setuptools import setup, find_packages
from os.path import join, abspath, dirname


PROJ_ROOT = dirname(abspath(__file__))

with open(join(PROJ_ROOT, "VERSION")) as fh:
    version = fh.read().strip()


setup(
    name="faasm-toolchain",
    version=version,
    packages=find_packages(),
    author="Simon S",
    author_email="blah@foo.com",
    description="Utilities related to the Faasm toolchain",
    url="https://github.com/faasm/faasm-toolchain",
    python_requires=">=3.6",
)
