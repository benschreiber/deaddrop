#ifndef SENDER_H
#define SENDER_H

#include <time.h>
#define BUF_SIZE 128

void init();
void send(char);
void tdiff(struct timespec*,struct timespec*,struct timespec*);

#endif
