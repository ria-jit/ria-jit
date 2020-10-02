<h1 align="center">Changelog 🛠</h1>
<p>
</p>

> Make RISC-V code executable on the x86-64 ISA by means of dynamic binary translation.

[![pipeline status](https://gitlab.lrz.de/lrr-tum/students/eragp-dbt-2020/badges/develop/pipeline.svg)](https://gitlab.lrz.de/lrr-tum/students/eragp-dbt-2020/-/commits/develop)


## Version 1.2.2 (latest)
### Release notes

* expand the static register mapping for better performance overall
* reallocate the code cache index and rehash all values when it is 50 % full for better lookup performance on capacity overflow
* rewrite command line options parsing to allow for long options (see `./translator -h`)
* allow finer control of specific optimisation features via `--optimize`
* add instruction pattern matching to the binary analyser to gather data for macro operation fusing
* add a profiler for counting register accesses
* implement emulation for syscalls `faccessat, getrandom, renameat2`
* remove emulation for the syscall `clone`
* fix crash when the code cache fills up by increasing the memory space available for translated blocks



## Version 1.2.1

### Release notes

* add implementations for `AMOMIN` and `AMOMAX` instructions
* add extensive unit testing for atomic and arithmetic instructions, as well as the parser
* fix several issues to enable the SPEC CPU 2017 benchmark suite to run
* implement emulation for syscalls `chdir, pipe2, getdents64, munmap, clone, execve, wait4`
* fix `ORI` instruction being parsed as `XORI`
* fix instruction semantics for `SUB(W)`
* finalize implementation of the return address stack



## Version 1.2.0

### Release notes

* enable register mapping for translated instructions
* add context switching from host to guest programs
* rework instruction translator function for flexibility
* implement a return address stack
* implement a TLB for cache lookup of blocks
* flip `-m` translation optimiser flag (enabled by default, flag now turns off optimisations)



## Version 1.1.0

### Release notes

*  cleanup and refactor project files
*  remove all C++ usage from translator code
*  eliminate standard library usage
*  add performance measuring flags to the translator



## Version 1.0.1

### Release notes

*  Fix an issue with the `read` system call that could cause blocking problems with gzip.



## Version 1.0

### Release notes

The initial release of the translator capable of executing gzip.

This release supports
*  the RISC-V integer instruction set
*  the multiplication extension instructions (M)
*  the atomic extension instructions (A).

The latter are not yet implemented atomically, however they make the translator compatible with binaries compiled for the architecture `rv64ima`, with the ABI `lp64`.

We currently support the system calls `ioctl, unlinkat, fchmod, fchown, openat, close, lseek, read, write, readlinkat, fstat, utimensat, exit, exit_group, futex, clock_gettime, rt_sigaction (ignored), rt_sigprocmask, uname, gettimeofday, brk`.
