#include <stdio.h>
#include <time.h>
#include <inttypes.h>
#include "sender.hpp"

#define RDSEED_COUNT (1 << 12)
#define RDSEED_COUNT_FINE (1 << 8)
#define TIMING_FACTOR 500

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

uint64_t time_rdseed(int count) {
    uint64_t before = rdtsc();
    rdseed_loop(count);
    uint64_t after = rdtsc();

    return after - before;
}

void init() {
    pulse_cycles = time_rdseed(RDSEED_COUNT * TIMING_FACTOR) / TIMING_FACTOR;
}

void tdiff(struct timespec *begin, struct timespec *end,
           struct timespec *diff) {
    diff->tv_sec = end->tv_sec - begin->tv_sec;
    diff->tv_nsec = end->tv_nsec - begin->tv_nsec;

    if (diff->tv_nsec < 0) {
        diff->tv_sec--;
        diff->tv_nsec += 1000000000;
    }
}

inline void low() {
    uint64_t before = rdtsc();

    do {
        if (rdtsc() - before > pulse_cycles) {
            break;
        }
    } while (true);
}

inline void high() {
    uint64_t before = rdtsc();

    do {
        if (rdtsc() - before > pulse_cycles) {
            break;
        }

        rdseed_loop(RDSEED_COUNT_FINE);

    } while (true);
}

void send(char v) {
    high();

    for (int i = 0; i < 8; i++) {
        bool bit = v & 1;
        v >>= 1;

        if (bit) {
            high();
        } else {
            low();
        }
    }

    low();
    low();
    low();
}

/*
int main(int argc, char **argv) {
    init();

    printf("Please type a message.\n");

    bool sending = true;
    while (sending) {
        char text_buf[BUF_SIZE];
        fgets(text_buf, sizeof(text_buf), stdin);

        struct timespec begin, end, diff;
        clock_gettime(CLOCK_MONOTONIC, &begin);

        int count = 0;
        for (int i = 0; i < BUF_SIZE; i++) {
            char v = text_buf[i];
            if (v == 0) {
                break;
            }

            // printf("%c\n", v);

            send(v);
            count++;
        }

        clock_gettime(CLOCK_MONOTONIC, &end);
        tdiff(&begin, &end, &diff);

        printf("%d bytes in %lds %ldns\n", count, diff.tv_sec, diff.tv_nsec);
    }

    printf("Sender finished.\n");

    return 0;
}
*/
