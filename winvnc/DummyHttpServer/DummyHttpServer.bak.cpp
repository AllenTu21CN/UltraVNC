// DummyHttpServer.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <httplib.h>
#include <stdio.h>

#include "base/encrypt/base64.h"
#include "base/debug.h"
#include "base/log.h"

#include "../VNCManager/utils/encrypt.h"

#define AES128 1
#ifdef __cplusplus
extern "C"
{
#endif
#include "aes.h"
#ifdef __cplusplus
}
#endif

static uint8_t g_key[16] = {'1', '2', '3'};

static void decodeB64AES(const char *b64)
{
	unsigned char *out = NULL;
	int out_size = base64_decode(std::string(b64), &out);

	BASE_DEBUG_2_LOG_WITH_MSG2(out, out_size, out_size, "b64->aes: ", info);

	int count_of_128 = out_size / 16;
	if (out_size % 16 > 0)
		++count_of_128;
	int restort_buf_len = count_of_128 * 16;

	unsigned char *restort_buf = new unsigned char[restort_buf_len];
	memset(restort_buf, 0, restort_buf_len);
	memcpy(restort_buf, out, out_size);
	delete[] out;
	out = NULL;

	struct AES_ctx ctx;
	AES_init_ctx(&ctx, g_key);
	for (int i = 0; i < count_of_128; ++i)
	{
		AES_ECB_decrypt(&ctx, restort_buf + (i * 16));
	}

	BASE_DEBUG_2_LOG_WITH_MSG2(restort_buf, restort_buf_len, restort_buf_len, "aes->ori: ", info);

	delete[] restort_buf;
	fprintf(stderr, "\n");
}

int main()
{
	{
		uint8_t in[16] = {'1', '2', '3'};
		std::string b64;
		AES_128_ECB_0padding_base64_encrypt(g_key, in, 16, b64);
	}

	{
		uint8_t in[16] = {'1', '2', '3'};

		struct AES_ctx ctx;
		AES_init_ctx(&ctx, g_key);
		AES_ECB_encrypt(&ctx, in);

		BASE_DEBUG_2_LOG_WITH_MSG2(in, 16, 16, "ori(123)->aes: ", info);

		std::string b64 = base64_encode(in, 16);
		fprintf(stderr, "aes->b64: %s\n", b64.c_str());
		fprintf(stderr, "\n");
	}

	{
		const char *b64 = "xdBVZKEFnH/gDT6e3JDe7g==";
		decodeB64AES(b64);
	}

	{
		uint8_t in[32] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g'};

		struct AES_ctx ctx;
		AES_init_ctx(&ctx, g_key);
		AES_ECB_encrypt(&ctx, in);
		AES_ECB_encrypt(&ctx, in + 16);

		BASE_DEBUG_2_LOG_WITH_MSG2(in, 32, 32, "ori(0..g)->aes: ", info);

		std::string b64 = base64_encode(in, 32);
		fprintf(stderr, "aes->b64: %s\n", b64.c_str());
		fprintf(stderr, "\n");
	}

	{
		const char *b64 = "38g8UmB/CO4AzV2EUXs5xBQmNVXQIjaAmLg451TK0Nw=";
		decodeB64AES(b64);
	}

	httplib::Server server;

	server.Get("/", [](const auto &req, auto &res)
			   { res.set_content("Welcome!", "text/plain"); });

	server.Get("/update_status", [](const httplib::Request &req, auto &res)
			   {
				   fprintf(stderr, "update_status:\n");
				   for (auto iter = req.params.begin(); iter != req.params.end(); ++iter)
				   {
					   fprintf(stderr, "  %s=%s\n", iter->first.c_str(), iter->second.c_str());
				   }
				   fprintf(stderr, "\n");

				   res.set_content("OK!", "text/plain");
			   });

	server.listen("0.0.0.0", 18080);

	return 0;
}
