#!/usr/bin/env python3

import subprocess
import sys
import os

def main():
	readelf = "/u/home/schmidtf/riscv-gnu-toolchain/build/bin/riscv64-unknown-linux-gnu-readelf"
	emulator = "/u/home/schmidtf/qemu/build/qemu-riscv64"
	#emulator = "/u/home/schmidtf/dbt-candidate/translator -f"

	# get path as command line argument
	if len(sys.argv) < 2:
		sys.stderr.write("Bad. No executable specified.\n")
		sys.exit(1)

	proc = subprocess.Popen([readelf, "-h", sys.argv[1]], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
	out, err = proc.communicate()

	if err is not None:
		sys.stderr.write("Bad. Error in reading ELF header.\n")
		sys.exit(2)

	risc = "RISC-V" in str(out)
	x86 = "Advanced Micro Devices X86-64" in str(out)
	sys.argv[1] = ("./" if not sys.argv[1].startswith("/") else "") + sys.argv[1]

	if risc and not x86:
		os.system(emulator + " " + " ".join(sys.argv[1:]))
	elif x86 and not risc:
		os.system(" ".join(sys.argv[1:]))
	else:
		sys.stderr.write("Bad. Unable to detect executable architecture.\n")
		sys.exit(3)

	sys.exit(0)

if __name__ == '__main__':
	main()
