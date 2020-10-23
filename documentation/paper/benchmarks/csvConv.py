#!/usr/bin/env python3

import csv
import sys


def escape(string):
    return '{' + string.replace('{', "\\{").replace("}", "\\}").replace("#", "No.").replace("_s", "") + '}'


def main():
    in_file = open(sys.argv[1], "r")
    out_file = open(sys.argv[2], "w")
    csv_file = csv.reader(in_file)
    line_count = 0
    for row in csv_file:
        line_count += 1
        if 7 <= line_count < 18:
            out_file.write(",".join(map(escape, row)) + "\n")


if __name__ == '__main__':
    main()
