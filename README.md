<h1 align="center">Dynamic Binary Translation (RISC-V -> x86) 🛠</h1>
<p>
</p>

> Make RISC-V code executable on the x86-64 ISA by means of dynamic binary translation.

[![pipeline status](https://gitlab.lrz.de/lrr-tum/students/eragp-dbt-2020/badges/master/pipeline.svg)](https://gitlab.lrz.de/lrr-tum/students/eragp-dbt-2020/-/commits/master)

### 🏠 [Homepage](https://gitlab.lrz.de/lrr-tum/students/eragp-dbt-2020)

## Building the translator
After checking out the git repository (and running `git submodule update --init` to fetch the dependencies), the translator can be built via
1.  `sudo apt-get -y install gcc g++ cmake make autoconf meson` (required dependencies)
2.  `mkdir build && cd build && cmake .. && make`

We can execute binaries compiled via the [RISC-V toolchain gcc](https://github.com/riscv/riscv-gnu-toolchain) and the options `-static -march=rv64ima -mabi=lp64`.

## Usage

```sh
./translator <translator option(s)> -f <program> <guest option(s)>
```

## Run tests

```sh
./test
```

## Authors

👤 **Noah Dormann, Simon Kammermeier, Johannes Pfannschmidt, Florian Schmidt**

* Github: [@Artemis-Skade](https://github.com/Artemis-Skade), [@flodt](https://github.com/flodt)
