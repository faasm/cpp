#!/bin/bash

set -e

python3 -m black --check $(git ls-files "*.py")

