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
    char* _secret_key_str = strip_whitespace(buf);
    GS_ASSERT(strlen(_secret_key_str) == 128);
    unsigned char* _secret_key = hexstr_to_char(_secret_key_str);
    free(buf);

    GS_ASSERT(_secret_key_str != NULL);
    GS_ASSERT(_secret_key != NULL);

    if (_secret_key_str != NULL && _secret_key != NULL)
    {
        if (secret_key_str != NULL) free(secret_key_str);
        if (secret_key != NULL) free(secret_key);
        secret_key_str = _secret_key_str;
        secret_key = _secret_key;
        
        int len = (int)strlen(secret_key_str);
        GS_ASSERT(len == SECRET_KEY_SIZE*2);
        for (int i=0; i<len; i++)
            GS_ASSERT(isxdigit(secret_key_str[i]));
            
        GS_ASSERT(strlen((char*)secret_key) == SECRET_KEY_SIZE);
    }
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
    for (int i=0; i<32; i++)
        printf("%02lx", (unsigned long)digest[i]);
}

void sprint_digest(char* dest, uint8_t* digest)
{   // sprint the digest
    for (int i=0; i<32; i++)
        sprintf(&dest[i*2], "%02lx", (unsigned long)digest[i]);
}

void server_init()
{
    if (!Options::auth) return;
    load_secret_key();
}

void server_update()
{   // check for a new secret key on disk
    if (!Options::auth) return;
    static int _tick = 0;
    if ((_tick++) % SECRET_KEY_REFRESH_RATE != 0) return;
    const char new_key_fn[] = SECRET_KEY_PATH ".new";
    // check if file exists
    if (!file_exists(new_key_fn)) return;
    // rename the new key to the main key file name
    rename(new_key_fn, SECRET_KEY_PATH);
    // load the new key
    load_secret_key();
}

// Teardown
void server_teardown()
{
    if (secret_key_str != NULL) free(secret_key_str);
    if (secret_key != NULL) free(secret_key);
}

bool verify_token(const char* _token, int* user_id, time_t* expiration_time, char** username)
{
    GS_ASSERT(secret_key != NULL);
    if (secret_key == NULL) return false;
    
    char* token = NULL;
    bool ok = parse_auth_token(_token, user_id, expiration_time, &token, username);
    if (!ok) return false;
    
    const unsigned int payload_len = AUTH_TOKEN_ID_LENGTH + AUTH_TOKEN_TIMESTAMP_LENGTH + strlen(*username);
    char* payload = (char*)malloc((payload_len+1)*sizeof(char));
    snprintf(payload, payload_len+1, "%09d%lld%s", *user_id, (long long)*expiration_time, *username);
    payload[payload_len] = '\0';

    uint8_t* _hash = compute_hash(secret_key, payload, payload_len);
    char* hash = (char*)malloc((AUTH_TOKEN_HASH_LENGTH+1)*sizeof(char));
    sprint_digest(hash, _hash);
    free(_hash);
    free(payload);

    bool match = (strcmp(token, hash) == 0);
    bool expired = auth_token_expired(*expiration_time, AUTH_TOKEN_LIFETIME);

    free(token);
    free(hash);
    
    ok = (match && !expired);
    if (!ok) free(*username);

    return ok;
}

bool verify_token(const char* _token)
{
    int user_id = 0;
    time_t expiration_time = 0;
    char* username = NULL;
    bool ok = verify_token(_token, &user_id, &expiration_time, &username);
    if (ok) free(username);
    return ok;
}

void received_auth_token(int client_id, const char* token)
{
    if (!Options::auth)
    {
        send_auth_token_valid(client_id);
        return;
    }
    int user_id = 0;
    time_t expiration_time = 0;
    char* username = NULL;
    bool ok = verify_token(token, &user_id, &expiration_time, &username);
    if (ok)
    {
        send_auth_token_valid(client_id);
        NetServer::client_authorized(client_id, user_id, expiration_time, username);
        free(username);
    }
    else
    {
        send_auth_token_invalid(client_id);
        NetServer::client_authorization_failed(client_id);
    }
}

void send_auth_token_valid(int client_id)
{
    auth_token_valid_StoC msg;
    msg.sendToClient(client_id);
}

void send_auth_token_invalid(int client_id)
{
    auth_token_invalid_StoC msg;
    msg.sendToClient(client_id);
}

}   // Auth
