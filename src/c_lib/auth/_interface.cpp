#include "_interface.hpp"

#include <common/common.hpp>
#include <auth/packets.hpp>

#if DC_CLIENT
#include <auth/client.hpp>
#endif
#if DC_SERVER
#include <auth/server.hpp>
#endif

namespace Auth
{

void init_packets()
{
    auth_token_CtoS::register_server_packet();
    auth_token_valid_StoC::register_client_packet();
    auth_token_invalid_StoC::register_client_packet();
}

void init()
{
    #if DC_CLIENT
    client_init();
    #endif
    #if DC_SERVER
    server_init();
    #endif    
}

void teardown()
{
    #if DC_CLIENT
    client_teardown();
    #endif
    #if DC_SERVER
    server_teardown();
    #endif
}

bool is_valid_username(const char* username, size_t len)
{
    for (unsigned int i=0; i<len; i++)
        if (!is_strict_char(username[i])
           || username[i] == AUTH_TOKEN_DELIMITER || username[i] == '/')
            return false;
    return true;
}

bool parse_auth_token(const char* token, int* user_id, time_t* expiration_time, char** hash, char** username)
{
    // copy token into modifiable buffer
    char* _token = (char*)malloc((AUTH_TOKEN_LENGTH+1)*sizeof(char));
    strncpy(_token, token, AUTH_TOKEN_LENGTH);
    _token[AUTH_TOKEN_LENGTH] = '\0';

    // replace delimiters with NUL
    int delims = 0;
    unsigned int len = 0;
    for (unsigned int i=0; i<AUTH_TOKEN_LENGTH && _token[i] != '\0'; i++)
    {
        if (_token[i] == AUTH_TOKEN_DELIMITER)
        {
            len = 0;
            _token[i] = '\0';
            delims++;
        }
        else
        {
            len++;
            if ((delims == 0 && len > AUTH_TOKEN_ID_LENGTH) ||
                (delims == 1 && len > AUTH_TOKEN_TIMESTAMP_LENGTH) ||
                (delims == 2 && len > AUTH_TOKEN_HASH_LENGTH))
            {
                free(_token);
                return false;
            }
        }
    }

    // username is last token piece; check that it is within bounds
    GS_ASSERT(len >= AUTH_TOKEN_USERNAME_MIN_LENGTH && len <= AUTH_TOKEN_USERNAME_MAX_LENGTH);
    if (len < AUTH_TOKEN_USERNAME_MIN_LENGTH || len > AUTH_TOKEN_USERNAME_MAX_LENGTH)
    {
        free(_token);
        return false;
    }
    int username_len = len;

    // check that all pieces were there
    GS_ASSERT(delims == AUTH_TOKEN_PIECES-1);
    if (delims != AUTH_TOKEN_PIECES-1)
    {
        free(_token);
        return false;
    }

    // convert user id to integer
    *user_id = atoi(_token);
    GS_ASSERT(user_id > 0);
    if (user_id <= 0)
    {
        free(_token);
        return false;
    }

    // convert expiration time to time_t
    *expiration_time = atott(&token[AUTH_TOKEN_ID_LENGTH + 1]);

    // verify hash and username
    const char* phash = &token[AUTH_TOKEN_ID_LENGTH + 1 + AUTH_TOKEN_TIMESTAMP_LENGTH + 1]; 
    const char* pusername = &phash[AUTH_TOKEN_HASH_LENGTH + 1];
    if (!is_valid_username(pusername, username_len))
    {
        free(_token);
        return false;
    }

    // copy hash
    char* _hash = (char*)malloc((AUTH_TOKEN_HASH_LENGTH+1)*sizeof(char));
    strncpy(_hash, phash, AUTH_TOKEN_HASH_LENGTH);
    _hash[AUTH_TOKEN_HASH_LENGTH] = '\0';
    *hash = _hash;

    // copy username
    char* _username = (char*)malloc((username_len+1) * sizeof(char));
    strncpy(_username, pusername, username_len);
    _username[username_len] = '\0';
    *username = _username;

    free(_token);
    return true;
}

bool auth_token_expired(const time_t timestamp, const time_t expiration_window)
{   // check if token needs to be refreshed
    if (timestamp < AUTH_TOKEN_LIFETIME) return true;   // bad data

    time_t now = utc_now();
    time_t created_at = timestamp - AUTH_TOKEN_LIFETIME;
    double dt = difftime(now, created_at);
    return (dt < 0 || dt >= (double)expiration_window);
}

}   // Auth
