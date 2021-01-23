<h1 align="center">Changelog 🛠</h1>
<p>
</p>

> Make RISC-V code executable on the x86-64 ISA by means of dynamic binary translation.

## Version 1.3.2 (latest)
### Release notes
- fix a divide-by-zero spec compliance bug
- add research paper, presentation slides and benchmark results
- rename logging levels for more readable verbose output

## Version 1.3.1
### Release notes
- fix a bug where the replacement register recency was not reset correctly when loading a non-mapped register that was already present
- prevent redundant writes to x0 in the A-extension translation
- reorder patterns for more efficient translations
- various minor cleanups


## Version 1.3.0
### Release notes
- implement support for F-/D-extension including a static register mapping
- expand unit testing coverage to test combinations for floating point instructions
- optimise chaining for conditional branches
- cleanup and fix various small issues in the code base


## Version 1.2.4
### Release notes
- implement a lazy runtime register replacement strategy, keeping the not-statically-mapped values in the replacement registers as long as possible to prevent redundant memory operations inside blocks
- add logging for static and dynamic register mapping
- implement patterns for MV and LI, ADDI with rs1 == x0 and several shifting combinations as well as zero-extensions via ANDI and 0xff
- use the x86 LEA instruction for faster translations of various input instructions
- rewrite and optimise the return address stack
- fix the `-m` short option to include `--optimize=no-fusion`
- use the output of `git describe` for the version string to include the commit hash in the build
- add inline logging for generated assembly with `--log=asm-out`
- split up the analyser command line flags to specify what to analyse
- bump C standard to C11


## Version 1.2.3
### Release notes
- do not page-align the generated code
- implement pattern matching to apply macro operation fusion of multiple RISC-V instructions
- optimise various instructions and clean up legacy code
- gain performance to approx. 1.4x-1.7x faster than QEMU


## Version 1.2.2
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
