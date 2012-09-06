#include "server.hpp"

#if DC_CLIENT
dont_include_this_file_in_client
#endif

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

void print_digest(uint8_t* digest)
{   // print the digest
    printf("Digest: ");
    for (int i=0; i<32; i++)
        printf("%02lx", (unsigned long)digest[i]);
    printf("\n");
}

void sprint_digest(char* dest, uint8_t* digest)
{   // sprint the digest
    for (int i=0; i<32; i++)
        sprintf(&dest[i*2], "%02lx", (unsigned long)digest[i]);
}

void server_init()
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
    print_digest(digest);
    free(digest);
}

// Teardown
void server_teardown()
{
    if (secret_key_str != NULL) free(secret_key_str);
    if (secret_key != NULL) free(secret_key);
}

bool verify_token(const char* _token)
{
    printf("Verifying token: %s\n", _token);
    
    GS_ASSERT(secret_key != NULL);
    if (secret_key == NULL) return false;
    
    int user_id = 0;
    time_t expiration_time = 0;
    char* token = NULL;
    bool ok = parse_auth_token(_token, &user_id, &expiration_time, &token);
    if (!ok) return false;
    printf("   User token: %s\n", token);
    
    const unsigned int payload_len = AUTH_TOKEN_ID_LENGTH + AUTH_TOKEN_TIMESTAMP_LENGTH + 1;
    char payload[payload_len];
    snprintf(payload, payload_len, "%09d%lld", user_id, (long long)expiration_time);
    payload[payload_len] = '\0';
    printf("Adjusted Payload: %s\n", payload);
    uint8_t* _hash = compute_hash(secret_key, payload, payload_len-1);
    print_digest(_hash);
    char* hash = (char*)malloc((AUTH_TOKEN_HASH_LENGTH+1)*sizeof(char));
    sprint_digest(hash, _hash);
    free(_hash);
    
    //printf("Computed hash: %s\n", hash);

    bool match = (strcmp(token, hash) == 0);
    bool expired = auth_token_expired(expiration_time, AUTH_TOKEN_LIFETIME);
    if (expired) printf("Token expired\n");

    free(token);
    free(hash);
    
    return match && !expired;
}

void received_auth_token(int client_id, const char* token)
{
    printf("Received token %s from client %d\n", token, client_id);
    bool ok = verify_token(token);
    if (ok) printf("Token is valid\n");
}

}   // Auth
