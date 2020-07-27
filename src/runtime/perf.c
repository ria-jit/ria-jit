//
// Created by Flo Schmidt on 27.07.20.
//

#include "perf.h"
#include <util/log.h>

struct timespec begin_measure() {
    log_benchmark("Starting measurement...\n");
    struct timespec retval;
    clock_gettime(CLOCK_MONOTONIC, &retval);
    return retval;
}

void end_display_measure(struct timespec *start) {
    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC, &end);

    log_benchmark("Stopped measurement.\n");

    double secs = (end.tv_sec - start->tv_sec) + 1e-9 * (end.tv_nsec - start->tv_nsec);
    double nanos = 1e9 * (end.tv_sec - start->tv_sec) + (end.tv_nsec - start->tv_nsec);

    //todo: the minilib printf does not yet handle %f
    log_benchmark("Execution time in seconds: %f\n", secs);
    log_benchmark("Execution time in nanoseconds: %f\n", nanos);
}
