//#include "util.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>

#include "receiver.h"

#define RDSEED_COUNT_FINE (1 << 8)
#define RDSEED_COUNT_THRESHOLD (1 << 23)
#define TIMING_FACTOR 500

double success_rate = 0.0;
double threshold = 0.0;

static uint64_t pulse_cycles = 0;

inline uint64_t rdtsc() {
    uint64_t a, d;
    asm volatile("lfence; rdtsc" : "=a"(a), "=d"(d));
    return (d << 32) | a;
}

inline unsigned char rdseed() {
    uint32_t seed;
    unsigned char ok;
    asm volatile("rdseed %0; setc %1" : "=r"(seed), "=qm"(ok));
    return ok;
}

void rdseed_loop(int count) {
    for (int i = 0; i < count; i++) {
        rdseed();
    }
}

double probe(int count) {
    int success_count = 0;

    for (int i = 0; i < count; i++) {
        success_count += rdseed();
    }

    return (double)success_count / count;
}

uint64_t time_rdseed(int count) {
    uint64_t before = rdtsc();
    rdseed_loop(count);
    uint64_t after = rdtsc();

    return after - before;
}

void init() {
    pulse_cycles = time_rdseed(RDSEED_COUNT * TIMING_FACTOR) /
                   (RDSEED_POLL_RATE * TIMING_FACTOR);
    success_rate = probe(RDSEED_COUNT_THRESHOLD);
    threshold = success_rate * 3.0 / 4.0;
}

bool probe_bit() {
    uint64_t before = rdtsc();

    int count = 0;
    int success_count = 0;

    do {
        if (rdtsc() - before > pulse_cycles) {
            break;
        }

        success_count += rdseed();
        count++;

    } while (true);

    double percent = (double)success_count / count;
    // printf("%f\n", percent);

    return percent < threshold;
}

/*
int main(int argc, char **argv) {
    init();

    printf("rdseed success rate was %.3f, using threshold %.3f\n", success_rate,
           threshold);

    printf("Please press enter.\n");

    char text_buf[2];
    fgets(text_buf, sizeof(text_buf), stdin);

    printf("Receiver now listening.\n");

    bool listening = true;
    while (listening) {
        // Wait until we see half of a 1
        for (int i = 0; i < RDSEED_POLL_RATE / 2;) {
            bool bit = probe_bit();

            if (bit) {
                i++;
            } else {
                i = 0;
            }
        }

        // We've seen half of samples of the 1, so read and
        // discard the other half.
        for (int i = 0; i < (RDSEED_POLL_RATE / 2); i++) {
            probe(RDSEED_POLL_COUNT);
        }

        char v = 0;

        for (int j = 0; j < 8; j++) {
            int d = 0;
            for (int i = 0; i < RDSEED_POLL_RATE; i++) {
                d += probe_bit() ? 1 : -1;
            }

            char bit = (d > 0) ? 1 : 0;
            v |= (bit << j);
        }

        printf("%c", v);
        fflush(stdout);
    }

    printf("Receiver finished.\n");

    return 0;
}
*/
