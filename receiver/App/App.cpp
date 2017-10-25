#include <stdio.h>
#include <iostream>
#include <iomanip>
#include "Enclave_u.h"
#include "sgx_urts.h"
#include "sgx_utils/sgx_utils.h"

#include "../receiver.h"

/* Global EID shared by multiple threads */
sgx_enclave_id_t global_eid = 0;

extern double success_rate;
extern double threshold;

// OCall implementations
void ocall_print(const char* str) {
    printf("%s\n", str);
}

void ocall_result(uint8_t* cipher) {
	std::cout << std::hex;
	for (int i = 0; i < 16; i++) {
		std::cout << std::setfill('0') << std::setw(2) << (unsigned)cipher[i] << " ";
	}
	std::cout << std::dec << std::endl;
}

int main(int argc, char const *argv[]) {
    if (initialize_enclave(&global_eid, "enclave.token", "enclave.signed.so") < 0) {
        std::cout << "Fail to initialize enclave." << std::endl;
        return 1;
    }

	init();
	
    printf("rdseed success rate was %.3f, using threshold %.3f\n", success_rate,
           threshold);

    printf("Please press enter.\n");

    char text_buf[2];
    fgets(text_buf, sizeof(text_buf), stdin);

    printf("Receiver now listening.\n");

	while (true) {
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

		// decrypt char
		sgx_status_t status = decrypt_msg(global_eid, (uint8_t*)&v, 1);

		printf("%c", v);
		fflush(stdout);
	}

    return 0;
}
