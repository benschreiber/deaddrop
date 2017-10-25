#include "Enclave_t.h"
#include "aes.h"

uint8_t key[17] = "CS598CLF is hard";

void decrypt_msg(uint8_t* buf, size_t len) {
	for (int i = 0; i < len; i++)
		buf[i] ^= key[i];
}
