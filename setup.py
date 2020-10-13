import setuptools
import pyyaml
from os.path import join, abspath, dirname


PROJ_ROOT = dirname(abspath(__file__))

with open(join(PROJ_ROOT, "versions.yml")) as fh:
    version = pyyaml.load(fh)["toolchain"]


setuptools.setup(
    name="faasm-toolchain",
    version=version,
    author="Simon S",
    author_email="blah@foo.com",
    description="Utilities related to the Faasm toolchain",
    url="https://github.com/faasm/faasm-toolchain",
    python_requires=">=3.6",
)
