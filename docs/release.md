# Building and Releasing

The toolchain repo is based on two Docker images:

- `faasm/toolchain` - the base image holding just the custom LLVM tools
- `faasm/sysroot` - the image holding both the tools and sysroot

See the [Actions page](https://github.com/faasm/faasm-toolchain/actions) and
[Dockerfiles](docker) for more info.

You only need to rebuild the `toolchain` image when upgrading LLVM (see 
[the docs](docs/upgrade-llvm.md).

The `sysroot` image is rebuilt as part of the CI and tagging process. 

## Development

Tasks related to this repo are run using [Invoke](https://www.pyinvoke.org/).
You just need to set up a virtual environemnt:

```bash
# Virtualenv
python3 -m venv venv
source venv/bin/activate
pip install requirements.txt

# List tasks
inv -l
```

## Release build

The release build will run the `sysroot` build and push the Docker image to
Dockerhub.

To do this:

- Create a branch with your changes
- Update the version in [`VERSION`](../VERSION)
- Push this to your branch
- Run `inv git.tag` to create the tag (from the head of the current branch)
- Let the CI build run through and build the container
- Once done, create a PR, which will again run through the container build to
  check it's ok

## Rebuilding `toolchain`

You should only need to manually rebuild the `toolchain` image, the `sysroot`
image is built through GH Actions.

```bash
# Build the toolchain image
inv container.toolchain` - rebuilds the base toolchain image

# Push the latest toolchain image build
inv container.push-toolchain
```

## Rebuilding `sysroot`

If you do want to build `sysroot` locally (e.g. for debugging issues):

```bash
inv container.sysroot
```

