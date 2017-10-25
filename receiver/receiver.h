#ifndef RECEIVER_H
#define RECEIVER_H

#define RDSEED_COUNT (1 << 12)
#define RDSEED_POLL_RATE (1 << 4)
#define RDSEED_POLL_COUNT (RDSEED_COUNT / RDSEED_POLL_RATE)

void init();
bool probe_bit();
double probe(int);

#endif
