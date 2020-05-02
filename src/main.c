//
// Created by flo on 24.04.20.
//

#include <stdio.h>
#include "parser.h"
#include <stdbool.h>
#include "util.h"

//just temporary - we need some way to control transcoding globally?
bool finalize = false;

//prototypes
int transcode_loop();

int main() {
    printf("Hello World!\n");
    test_parsing();
    transcode_loop();
    return 0;
}

int transcode_loop() {
    t_risc_pc pc = NULL;

    while (!finalize) {

    }

    return 0;
}