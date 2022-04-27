#include "encrypt.h"

#define AES128 1
#include "base/encrypt/aes.hpp"
#include "base/encrypt/base64.h"

#define DEBUG_AES 0
#if DEBUG_AES
#include "base/debug.h"
#include "base/log.h"
#endif

#include "base/log.h"

static int exec(const char *cmd, std::string &std_out);

static std::string g_app_path;

void set_encrypt_user_date(void *data)
{
	g_app_path = (const char *) data;
}

void AES_128_ECB_0padding_base64_encrypt(const uint8_t aes_key[16],
										 const uint8_t *origin_data,
										 uint64_t data_size,
										 std::string &base64)
{
	uint64_t count_of_16 = data_size / 16;
	if (data_size % 16 > 0)
		++count_of_16;
	uint64_t aes_enc_size = count_of_16 * 16;

	uint8_t *restore_buf = new uint8_t[aes_enc_size];
	memset(restore_buf, 0, aes_enc_size);
	memcpy(restore_buf, origin_data, data_size);

	struct AES_ctx ctx;
	AES_init_ctx(&ctx, aes_key);
	for (uint64_t i = 0; i < count_of_16; ++i)
	{
		AES_ECB_encrypt(&ctx, restore_buf + (i * 16));
	}

#if DEBUG_AES
	BASE_DEBUG_2_LOG_WITH_MSG2(restore_buf, aes_enc_size, aes_enc_size, "ori->aes: ", info);
#endif

	base64 = base64_encode(restore_buf, aes_enc_size);

#if DEBUG_AES
	base::_info("aes->b64: %s", base64.c_str());
#endif
}

void AES_128_ECB_0padding_base64_decrypt(const uint8_t aes_key[16],
										 const char *base64,
										 uint8_t **decoded_data,
										 uint64_t &decoded_data_size)
{
	uint8_t *aes_data = NULL;
	uint64_t aes_size = base64_decode(std::string(base64), &aes_data);

#if DEBUG_AES
	BASE_DEBUG_2_LOG_WITH_MSG2(aes_data, aes_size, aes_size, "b64->aes: ", info);
#endif

	uint64_t count_of_16 = aes_size / 16;
	if (aes_size % 16 > 0)
		++count_of_16;
	uint64_t aes_dec_size = count_of_16 * 16;

	if (aes_dec_size > aes_size)
	{
		uint8_t *restore_buf = new uint8_t[aes_dec_size];
		memset(restore_buf, 0, aes_dec_size);
		memcpy(restore_buf, aes_data, aes_size);
		delete[] aes_data;

		aes_data = restore_buf;
		aes_size = aes_dec_size;
	}

	struct AES_ctx ctx;
	AES_init_ctx(&ctx, aes_key);
	for (uint64_t i = 0; i < count_of_16; ++i)
	{
		AES_ECB_decrypt(&ctx, aes_data + (i * 16));
	}

#if DEBUG_AES
	BASE_DEBUG_2_LOG_WITH_MSG2(aes_data, aes_size, aes_size, "aes->ori: ", info);
#endif

	*decoded_data = aes_data;
	decoded_data_size = aes_size;
}

int RSA_pkcs1padding_base64_pubkey_encrypt(const char *pub_key_path,
										   const char *plaintext_str,
										   std::string &base64)
{
	char cmd[512] = {0};
	snprintf(cmd, 511, "echo %s | \"%s\\openssl.exe\" pkeyutl -encrypt -pubin -inkey \"%s\" -pkeyopt rsa_padding_mode:pkcs1 | \"%s\\openssl.exe\" enc -A -base64",
			 plaintext_str, g_app_path.c_str(), pub_key_path, g_app_path.c_str());

	base::_debug("cmd: %s", cmd);

	return exec(cmd, base64);
}

int RSA_pkcs1padding_base64_prikey_decrypt(const char *pri_key_path,
										   const char *base64,
										   const char *pass_4_key,
										   std::string &plaintext_str)
{
	char cmd[1024] = {0};
	snprintf(cmd, 1024, "echo %s | \"%s\\openssl.exe\" base64 -d | \"%s\\openssl.exe\" pkeyutl -decrypt -inkey \"%s\" -pkeyopt rsa_padding_mode:pkcs1",
			 base64, g_app_path.c_str(), g_app_path.c_str(), pri_key_path);
	if (pass_4_key != NULL)
	{
		strcat(cmd, " -passin pass:");
		strcat(cmd, pass_4_key);
	}

	base::_debug("cmd: %s", cmd);

	return exec(cmd, plaintext_str);
}

static int exec(const char *cmd, std::string &std_out)
{
	std_out = "";

	FILE *pipe = _popen(cmd, "r");
	if (!pipe)
		return -1;

	int ret = 0;
	try
	{
		char buffer[128];
		while (fgets(buffer, 128, pipe) != NULL)
		{
			std_out += buffer;
		}
	}
	catch (...)
	{
		ret = -2;
	}

	return _pclose(pipe);
}