#include <stdint.h>
#include <string>

void set_encrypt_user_date(void *data);

void AES_128_ECB_0padding_base64_encrypt(const uint8_t aes_key[16],
                                         const uint8_t *origin_data,
                                         uint64_t data_size,
                                         std::string &base64);

void AES_128_ECB_0padding_base64_decrypt(const uint8_t aes_key[16],
                                         const char *base64,
                                         uint8_t **decoded_data,
                                         uint64_t &decoded_data_size);

int RSA_pkcs1padding_base64_pubkey_encrypt(const char *pub_key_path,
                                           const char *origin_str,
                                           std::string &base64);

int RSA_pkcs1padding_base64_prikey_decrypt(const char *pri_key_path,
                                           const char *base64,
                                           const char *pass_4_key,
                                           std::string &plaintext_str);