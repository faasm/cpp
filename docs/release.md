# Building and Releasing

The toolchain repo is based on two Docker images:

- `faasm/llvm` - the base image holding just the custom LLVM tools
- `faasm/cpp-sysroot` - the image holding both the tools and sysroot

See the [Actions page](https://github.com/faasm/cpp/actions) and
[Dockerfiles](docker) for more info.

You only need to rebuild the `llvm` image when upgrading LLVM (see 
[the docs](docs/upgrade-llvm.md).

The `cpp-sysroot` image is rebuilt as part of the CI and tagging process. 

## Release build

The release build will run the `cpp-sysroot` build and push the Docker image to
Dockerhub.

To do this:

- Create a branch with your changes
- Update the version in `VERSION`, `.env` and `.github/workflows`
- Push this to your branch
- Run `inv git.tag` to create the tag (from the head of the current branch)
- Let the CI build run through and build the container
- Once done, create a PR, which will run the tests against the new image

## Rebuilding `llvm`

You should only need to manually rebuild the `llvm` image, the `cpp-sysroot`
image is built through GH Actions.

```bash
# Build the llvm image
inv container.llvm

# Push it
inv container.push-llvm
```

## Rebuilding `cpp-sysroot`

If you do want to build `cpp-sysroot` locally (e.g. for debugging issues):

```bash
inv container.sysroot
```
