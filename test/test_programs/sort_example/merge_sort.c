//
// Created by flo on 15.07.20.
//

#include "../riscvminilib/rv64minilibc.h"

void sort(int numbers[], size_t len);
void sort_internal(int numbers[], int buf[], size_t len, int l, int r);
void merge(int numbers[], int buf[], size_t len, int l, int m, int r);

int main(int argc, char **argv) {
    int values[] = {7, 29, 137, 61, 0, 249, 1328, 5, 93, 8};
    int len = 10;

    print_arr(values, len);
    sort(values, len);
    print_arr(values, len);

    m_exit(0);
}

void sort(int numbers[], size_t len) {
    //allocate buffer for merging
    int buf[len];
    sort_internal(numbers, buf, len, 0, len - 1);
}

void sort_internal(int numbers[], int buf[], size_t len, int l, int r) {
    size_t part = r - l + 1;
    if (part <= 1) return;

    size_t ctr = (l + r) / 2;
    sort_internal(numbers, buf, len, l, ctr);
    sort_internal(numbers, buf, len, ctr + 1, r);
    merge(numbers, buf, len, l, ctr, r);

    //do some logging
    print_arr(numbers, len);
}

void merge(int numbers[], int buf[], size_t len, int l, int m, int r) {
    size_t part = r - l + 1;
    size_t lo = l;
    size_t hi = m + 1;
    size_t idx = 0;

    if (part < 1) return;

    while(lo <= m && hi <= r) {
        if (numbers[lo] <= numbers[hi]) {
            buf[idx++] = numbers[lo++];
        } else {
            buf[idx++] = numbers[hi++];
        }
    }

    while(lo <= m) {
        buf[idx++] = numbers[lo++];
    }

    while(hi <= r) {
        buf[idx++] = numbers[hi++];
    }

    for(size_t i = 0; i < part; ++i) {
        numbers[l + i] = buf[i];
    }
}
