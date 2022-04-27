// DummyHttpServer.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <httplib.h>
#include <stdio.h>

#include "base/log.h"

#include "../VNCManager/utils/encrypt.h"

static uint8_t g_key[16] = {'1', '2', '3'};

int main_rsa();

int main()
{
	{
		const char *plaintext = "12345678 12345678";
		const char *pub_key_path = "plt_rsa_pub_pkcs8.pem";
		std::string b64;
		int ret = RSA_pkcs1padding_base64_pubkey_encrypt(
			pub_key_path, plaintext, b64);
		if (ret == 0)
		{
			base::_info("encoded: %s", b64.c_str());
		}
		else
		{
			base::_info("encoded failed");
		}
	}

	{
		const char *b64 = "pCchnkZZcjkzyTAz7gxJfuYlwXeLHT+3gclWjlccalTIz4US1Ugpjs7GeIZeattXtW3eWw9fr5Dr4TGgcvpmzg==";
		const char *pri_key_path = "plt_rsa_pri_pkcs8.pem";
		const char *pass_4_key = "dlxx";
		std::string plaintext_str;
		int ret = RSA_pkcs1padding_base64_prikey_decrypt(
			pri_key_path, b64, pass_4_key, plaintext_str);
		if (ret == 0)
		{
			base::_info("decoded: %s", plaintext_str.c_str());
		}
		else
		{
			base::_info("decoded failed");
		}
	}

	{
		uint8_t in[16] = {'1', '2', '3'};
		std::string b64;
		base::_info("encode: '123': ");
		AES_128_ECB_0padding_base64_encrypt(g_key, in, 16, b64);
	}

	{
		uint8_t *data;
		uint64_t data_size;
		const char *b64 = "xdBVZKEFnH/gDT6e3JDe7g==";
		base::_info("decode '%s': ", b64);
		AES_128_ECB_0padding_base64_decrypt(g_key, b64, &data, data_size);
		delete[] data;
	}

	{
		uint8_t in[32] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g'};
		std::string b64;
		base::_info("encode: '0123..efg': ");
		AES_128_ECB_0padding_base64_encrypt(g_key, in, 32, b64);
	}

	{
		uint8_t *data;
		uint64_t data_size;
		const char *b64 = "38g8UmB/CO4AzV2EUXs5xBQmNVXQIjaAmLg451TK0Nw=";
		base::_info("decode '%s': ", b64);
		AES_128_ECB_0padding_base64_decrypt(g_key, b64, &data, data_size);
		delete[] data;
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
