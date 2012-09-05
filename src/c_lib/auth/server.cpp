#include "server.hpp"

#include <auth/hmac-sha256.h>

namespace Auth
{

// Globals
char* secret_key_str = NULL;          // hex str representation of secret key. read from secret_key_path
unsigned char* secret_key = NULL;   // hex str converted to bytes

// Init loaders
void load_secret_key()
{
    int size = 0;
    char* buf = read_file_to_buffer(SECRET_KEY_PATH, &size);
    GS_ASSERT(buf != NULL);
    if (buf == NULL) return;
    GS_ASSERT(size > 0);
    if (size <= 0)
    {
        free(buf);
        return;
    }
    secret_key_str = strip_whitespace(buf);
    secret_key = hexstr_to_char(secret_key_str);
    free(buf);
}

uint8_t* compute_hash(const unsigned char* secret_key, const char* msg, const size_t msg_len)
{
    hmac_sha256 hmac;
    sha256 sha;
    unsigned char key[64];

    // init sha256
    sha256_initialize(&sha);

    // hash the key if necessary (SECRET_KEY_SIZE > 64)
    unsigned int i=0;
    for (; i<SECRET_KEY_SIZE; i++)
    {
        if (i > 0 && i % 64 == 0)
            sha256_update(&sha, key, 64);
        key[i % 64] = secret_key[i];
    }

    // initialize hmac with the key, or the key hash if needed
    if (i <= 64)
        hmac_sha256_initialize(&hmac, key, i);
    else
    {   // using hash
        sha256_finalize(&sha, key, i%64);
        hmac_sha256_initialize(&hmac, sha.hash, 64);
    }

    // compute digest for test msg
    hmac_sha256_update(&hmac, (uint8_t*)msg, msg_len);

    // finalize hmac
    hmac_sha256_finalize(&hmac, NULL, 0);

    uint8_t* digest = (uint8_t*)malloc(32 * sizeof(uint8_t));
    memcpy(digest, hmac.digest, 32);
    return digest;
}

void init()
{
    load_secret_key();
    GS_ASSERT(secret_key_str != NULL);
    GS_ASSERT(secret_key != NULL);

    int len = (int)strlen(secret_key_str);
    GS_ASSERT(len == SECRET_KEY_SIZE*2);
    for (int i=0; i<len; i++)
        GS_ASSERT(isxdigit(secret_key_str[i]));

    GS_ASSERT(strlen((char*)secret_key) == SECRET_KEY_SIZE);

    const char test_msg[] = "Hello gnomescroll";
    uint8_t* digest = compute_hash(secret_key, test_msg, sizeof(test_msg)-1);


    printf("Secret key: %s\n", secret_key_str);
    printf("Payload: %s\n", test_msg);
    
    // print the digest
    printf("Digest: ");
    for (int i=0; i<32; i++)
        printf("%02lx", (unsigned long)digest[i]);
    printf("\n");

    free(digest);
}

// Teardown
void teardown()
{
    if (secret_key_str != NULL) free(secret_key_str);
    if (secret_key != NULL) free(secret_key);
}

}   // Auth
