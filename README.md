<h1 align="center">Dynamic Binary Translation (RISC-V -> x86) 🛠</h1>
<p>
</p>

> Make RISC-V code executable on the x86-64 ISA by means of dynamic binary translation.

[![pipeline status](https://gitlab.lrz.de/lrr-tum/students/eragp-dbt-2020/badges/develop/pipeline.svg)](https://gitlab.lrz.de/lrr-tum/students/eragp-dbt-2020/-/commits/develop)

### 🏠 [Homepage](https://gitlab.lrz.de/lrr-tum/students/eragp-dbt-2020)

## Building the translator
After checking out the git repository (and running `git submodule update --init` to fetch the dependencies), the translator can be built via
1.  `sudo apt-get -y install gcc g++ cmake make autoconf meson` (required dependencies)
2.  `mkdir build && cd build && cmake .. && make`

We can execute binaries compiled via the [RISC-V toolchain gcc](https://github.com/riscv/riscv-gnu-toolchain) and the options `-static -march=rv64ima -mabi=lp64`.

## Usage

```sh
Usage: ./translator [translator option(s)] -f <filename> [guest options]

Options:
	-v, --version
		Show translator version.
	-f, --file <executable>
		Specify executable. All options after the file path are passed to the guest.
	-a, --analyze-all
	--analyze-mnem, --analyze-reg, --analyze-pattern
		Analyze the binary. Does not execute the guest program.
		Inspects passed program binary and shows the selected statistics.
	-b, --benchmark
		Benchmark execution. Times the execution of the program,
		excluding mapping the binary into memory.
	-p, --profile
		Profile register usage. Display dynamic register usage statistics.
	--perf
		Log the generated blocks to /tmp/perf-<pid>.map for externally profiling
		the execution in perf.
	-s, --fail-silently
		Fail silently for some error conditions.
		Allows continued execution, but the client program may enter undefined states.
	-h, --help
		Show this help.

Logging:
	--log=category,[...]
		Enable logging for certain categories. See --log=help for more info.
	-g	Display general verbose info (--log=general,strace)
	-i	Display parsed RISC-V input assembly (--log=asm_in)
	-o	Display translated output x86 assembly (--log=asm_out,verbose_disasm)
	-r	Dump registers on basic block boundaries (--log=reg)
	-c	Display cache info (--log=cache)

Optimization:
	--optimize=category,[...]
		Disable certain optimization categories. See --optimize=help for more info.
	-d	Enable Single stepping mode.
		Each instruction will be its own block. (--optimize=singlestep)
	-m	Disable all translation optimization features. (--optimize=none)
```

## Run tests

```sh
./test
```

## Authors

👤 **Noah Dormann, Simon Kammermeier, Johannes Pfannschmidt, Florian Schmidt**

* Github: [@Artemis-Skade](https://github.com/Artemis-Skade), [@Simon04090](https://github.com/Simon04090), [@johaza](https://github.com/johaza), [@flodt](https://github.com/flodt)
