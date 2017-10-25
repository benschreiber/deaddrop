#ifndef _AES_H_
#define _AES_H_

#include <stdint.h>


// #define the macros below to 1/0 to enable/disable the mode of operation.
//
// CBC enables AES encryption in CBC-mode of operation.
// ECB enables the basic ECB 16-byte block algorithm. Both can be enabled simultaneously.

// The #ifndef-guard allows it to be configured before #include'ing or at compile time.
//
#define ECB 1

#define AES128 1
//#define AES192 1
//#define AES256 1

#if defined(ECB) && (ECB == 1)

void AES_ECB_encrypt(const uint8_t* buffer, const uint8_t* key, const uint32_t length);
void AES_ECB_decrypt(const uint8_t* buffer, const uint8_t* key, const uint32_t length);

#endif // #if defined(ECB) && (ECB == !)

#endif //_AES_H_
