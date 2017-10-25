#include "Enclave_t.h"

uint8_t key[17] = "CS598CLF is hard";

/* Poor man's OTP */
void encrypt_msg(uint8_t* buf, size_t len) {
	if (len > 16)
		return;

	for (int i = 0; i < len; i++)
		buf[i] ^= key[i];
}
