# Threading

We need to be able to compile threaded applications for Faasm, but want to take
control of high-level interfaces like pthreads.

For this reason, we compile everything in musl _except_ the `src/thread`
directory, which contains low-level lock implementations and the bulk of the
pthread interface.

By default WASI also excludes the definitions of `__lockfile`, `flockfile` etc.
This means the interface we must implement in Faasm is at this level (which
makes sense as we manage the filesystem syscalls at roughly this level of
abstraction too). 

We must define `_REENTRANT` to make sure that we compile a thread-safe version
of libc, otherwise we'll have issues when doing things like printing from
multi-threaded code.
