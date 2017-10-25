#include <stdio.h>
#include <iostream>
#include <iomanip>
#include "Enclave_u.h"
#include "sgx_urts.h"
#include "sgx_utils/sgx_utils.h"

#include "../sender.hpp"

/* Global EID shared by multiple threads */
sgx_enclave_id_t global_eid = 0;

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

	while (true) {
        char text_buf[128];
        fgets(text_buf, sizeof(text_buf), stdin);

        struct timespec begin, end, diff;
        clock_gettime(CLOCK_MONOTONIC, &begin);

        int count = 0;
        for (int i = 0; i < BUF_SIZE; i++) {
            char v = text_buf[i];

			if (v == '\0')
				break;

			encrypt_msg(global_eid, (uint8_t*)&v, 1);

			send(v);
			count++;
        }

        clock_gettime(CLOCK_MONOTONIC, &end);
        tdiff(&begin, &end, &diff);

        printf("%d bytes in %lds %ldns\n", count, diff.tv_sec, diff.tv_nsec);
	}

    return 0;
}
