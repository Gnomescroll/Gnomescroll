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
    run_tests();
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

bool is_valid_name_character(char c) __attribute__((always_inline));
bool is_valid_name_character(char c)
{
    return (isalnum(c) || c == '_');
}

bool is_valid_username(const char* username, size_t len)
{
    // length
    if (len < AUTH_TOKEN_USERNAME_MIN_LENGTH || len > AUTH_TOKEN_USERNAME_MAX_LENGTH)
        return false;

    // starts with alpha
    if (!isalnum(username[0]))
        return false;

    // valid characters
    bool has_alphabet = false;
    for (unsigned int i=0; i<len; i++)
    {
        if (!is_valid_name_character(username[i]))
            return false;
        if (isalpha(username[i]))
            has_alphabet = true;
    }

    // contains at least one alpha character
    if (!has_alphabet)
        return false;
        
    // not equal to placeholder name
    if (strcmp(UNDEFINED_NAME, username) == 0)
        return false;
        
    return true;
}

bool is_valid_username(const char* username)
{
    size_t len = strlen(username);
    return is_valid_username(username, len);
}

bool parse_auth_token(const char* token, int* user_id, time_t* expiration_time, char** hash, char** username)
{
    // copy token into modifiable buffer
    char* _token = (char*)malloc((AUTH_TOKEN_LENGTH+1)*sizeof(char));
    strncpy(_token, token, AUTH_TOKEN_LENGTH);
    _token[AUTH_TOKEN_LENGTH] = '\0';

    // replace delimiters with NUL
    int delims = 0;

    unsigned int id_len = 0;
    unsigned int timestamp_len = 0;
    unsigned int hash_len = 0;
    unsigned int username_len = 0;
    
    for (unsigned int i = 0; i<AUTH_TOKEN_LENGTH && _token[i] != '\0'; i++)
    {
        if (_token[i] == AUTH_TOKEN_DELIMITER)
        {
            _token[i] = '\0';
            delims++;
        }
        else
        {
            if (delims == 0)
            {   // user_id should be digits only
                id_len++;
                if (!isdigit(_token[i]))
                {
                    free(_token);
                    return false;
                }
            }
            else
            if (delims == 1)
            {   // timestamp should be digits only
                timestamp_len++;
                if (!isdigit(_token[i]))
                {
                    free(_token);
                    return false;
                }
            }
            else
            if (delims == 2)
            {   // hash should be hex chars only
                hash_len++;
                if (!isxdigit(_token[i]))
                {
                    free(_token);
                    return false;
                }
            }
            else
            if (delims == 3)
            {
                username_len++;
                if (!is_valid_name_character(_token[i]))
                {
                    free(_token);
                    return false;
                }
            }
        }
    }
    
    // check length of pieces
    if (id_len        != AUTH_TOKEN_ID_LENGTH
     || timestamp_len != AUTH_TOKEN_TIMESTAMP_LENGTH
     || hash_len      != AUTH_TOKEN_HASH_LENGTH
     || username_len   > AUTH_TOKEN_USERNAME_MAX_LENGTH
     || username_len   < AUTH_TOKEN_USERNAME_MIN_LENGTH)
    {
        free(_token);
        return false;
    }

    // check that all pieces were there
    if (delims != AUTH_TOKEN_PIECES-1)
    {
        free(_token);
        return false;
    }

    // convert user id to integer
    int _user_id = atoi(_token);
    if (!is_valid_user_id(_user_id))
    {
        free(_token);
        return false;
    }

    // convert expiration time to time_t
    time_t _expiration_time = atott(&token[AUTH_TOKEN_ID_LENGTH + 1]);

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

    // set remaining pointers
    *user_id = _user_id;
    *expiration_time = _expiration_time;

    free(_token);
    return true;
}

bool auth_token_expired(const time_t timestamp, const time_t expiration_window)
{   // check if token needs to be refreshed
    printf("Auth token expired? Timestamp: %lld Expiration window: %lld\n", (long long)timestamp, (long long)expiration_window);
    if (timestamp < AUTH_TOKEN_LIFETIME) return true;   // bad data

    time_t now = utc_now();
    printf("NOW: %lld\n", (long long)now);
    time_t created_at = difftime(timestamp, AUTH_TOKEN_LIFETIME);
    printf("CREATED AT: %lld\n", (long long)created_at);
    double dt = difftime(now, created_at);
    printf("DT: %f\n", dt);
    bool expired = (dt < 0 || (time_t)dt >= expiration_window);
    printf("EXPIRED: %d\n", expired);
    return expired;
}

inline bool is_valid_user_id(const int user_id)
{
    return (user_id > 0);
}

void run_tests()
{
    #if DC_SERVER
    if (!Options::auth) return;
    #endif

    // test hand crafted tokens against the parser and validator

    // setup
    bool ok = false;
    int user_id = 0;
    time_t expiration_time = 0;
    char* hash = NULL;
    char* username = NULL;

    // Parsing tests:

    // Valid Token
    const char valid_token[] = "000000001|1347071435|7da756f7e8f76f4244439aefda651b15eb8d35776e02c26f622abc0533077fb2|getgetgetgetget";
    ok = parse_auth_token(valid_token, &user_id, &expiration_time, &hash, &username);
    GS_ASSERT(ok);
    if (ok)
    {
        GS_ASSERT(user_id == 1);
        GS_ASSERT(expiration_time == 1347071435);
        GS_ASSERT(strcmp(hash, "7da756f7e8f76f4244439aefda651b15eb8d35776e02c26f622abc0533077fb2") == 0);
        GS_ASSERT(strcmp(username, "getgetgetgetget") == 0);
        free(username);
        free(hash);
        #if DC_SERVER
        // verification will fail because this token has expired
        GS_ASSERT(!verify_token(valid_token));
        #endif
    }

    const char empty_token[] = "";
    ok = parse_auth_token(empty_token, &user_id, &expiration_time, &hash, &username);
    GS_ASSERT(!ok);
    #if DC_SERVER
    GS_ASSERT(!verify_token(valid_token));
    #endif

    const char all_delimiters[] = "|||";
    ok = parse_auth_token(all_delimiters, &user_id, &expiration_time, &hash, &username);
    GS_ASSERT(!ok);
    #if DC_SERVER
    GS_ASSERT(!verify_token(valid_token));
    #endif

    const char less_delimiters[] = "||";
    ok = parse_auth_token(less_delimiters, &user_id, &expiration_time, &hash, &username);
    GS_ASSERT(!ok);
    #if DC_SERVER
    GS_ASSERT(!verify_token(valid_token));
    #endif
    
    const char more_delimiters[] = "||||";
    ok = parse_auth_token(more_delimiters, &user_id, &expiration_time, &hash, &username);
    GS_ASSERT(!ok);
    #if DC_SERVER
    GS_ASSERT(!verify_token(valid_token));
    #endif

    // Negative user id
    const char negative_user_id[] = "-00000001|1347071435|7da756f7e8f76f4244439aefda651b15eb8d35776e02c26f622abc0533077fb2|get";
    ok = parse_auth_token(negative_user_id, &user_id, &expiration_time, &hash, &username);
    GS_ASSERT(!ok);
    #if DC_SERVER
    GS_ASSERT(!verify_token(valid_token));
    #endif
    
    const char malformed_user_id[] = "000a00001|1347071435|7da756f7e8f76f4244439aefda651b15eb8d35776e02c26f622abc0533077fb2|get";
    ok = parse_auth_token(malformed_user_id, &user_id, &expiration_time, &hash, &username);
    GS_ASSERT(!ok);
    #if DC_SERVER
    GS_ASSERT(!verify_token(valid_token));
    #endif

    const char small_user_id[] = "00000001|1347071435|7da756f7e8f76f4244439aefda651b15eb8d35776e02c26f622abc0533077fb2|get";
    ok = parse_auth_token(small_user_id, &user_id, &expiration_time, &hash, &username);
    GS_ASSERT(!ok);
    #if DC_SERVER
    GS_ASSERT(!verify_token(valid_token));
    #endif
    
    const char large_user_id[] = "0100000001|1347071435|7da756f7e8f76f4244439aefda651b15eb8d35776e02c26f622abc0533077fb2|get";
    ok = parse_auth_token(large_user_id, &user_id, &expiration_time, &hash, &username);
    GS_ASSERT(!ok);
    #if DC_SERVER
    GS_ASSERT(!verify_token(valid_token));
    #endif
    
    const char missing_user_id[] = "|1347071435|7da756f7e8f76f4244439aefda651b15eb8d35776e02c26f622abc0533077fb2|get";
    ok = parse_auth_token(missing_user_id, &user_id, &expiration_time, &hash, &username);
    GS_ASSERT(!ok);
    #if DC_SERVER
    GS_ASSERT(!verify_token(valid_token));
    #endif
    
    const char negative_timestamp[] = "000000001|-347071435|7da756f7e8f76f4244439aefda651b15eb8d35776e02c26f622abc0533077fb2|get";
    ok = parse_auth_token(negative_timestamp, &user_id, &expiration_time, &hash, &username);
    GS_ASSERT(!ok);
    #if DC_SERVER
    GS_ASSERT(!verify_token(valid_token));
    #endif
    
    const char malformed_timestamp[] = "000000001|13r7071435|7da756f7e8f76f4244439aefda651b15eb8d35776e02c26f622abc0533077fb2|get";
    ok = parse_auth_token(malformed_timestamp, &user_id, &expiration_time, &hash, &username);
    GS_ASSERT(!ok);
    #if DC_SERVER
    GS_ASSERT(!verify_token(valid_token));
    #endif
    
    const char small_timestamp[] = "000000001|347071435|7da756f7e8f76f4244439aefda651b15eb8d35776e02c26f622abc0533077fb2|get";
    ok = parse_auth_token(small_timestamp, &user_id, &expiration_time, &hash, &username);
    GS_ASSERT(!ok);
    #if DC_SERVER
    GS_ASSERT(!verify_token(valid_token));
    #endif
    
    const char large_timestamp[] = "000000001|11347071435|7da756f7e8f76f4244439aefda651b15eb8d35776e02c26f622abc0533077fb2|get";
    ok = parse_auth_token(large_timestamp, &user_id, &expiration_time, &hash, &username);
    GS_ASSERT(!ok);
    #if DC_SERVER
    GS_ASSERT(!verify_token(valid_token));
    #endif
    
    const char missing_timestamp[] = "000000001||7da756f7e8f76f4244439aefda651b15eb8d35776e02c26f622abc0533077fb2|get";
    ok = parse_auth_token(missing_timestamp, &user_id, &expiration_time, &hash, &username);
    GS_ASSERT(!ok);
    #if DC_SERVER
    GS_ASSERT(!verify_token(valid_token));
    #endif
    
    const char malformed_hash[] = "000000001|1347071435|7(a756f7e8f76f4244439aefda651b15eb8d35776e02c26f622abc0533077fb2|get";
    ok = parse_auth_token(malformed_hash, &user_id, &expiration_time, &hash, &username);
    GS_ASSERT(!ok);
    #if DC_SERVER
    GS_ASSERT(!verify_token(valid_token));
    #endif
    
    const char small_hash[] = "000000001|1347071435|7da7567e8f76f4244439aefda651b15eb8d35776e02c26f622abc0533077fb2|get";
    ok = parse_auth_token(small_hash, &user_id, &expiration_time, &hash, &username);
    GS_ASSERT(!ok);
    #if DC_SERVER
    GS_ASSERT(!verify_token(valid_token));
    #endif
    
    const char large_hash[] = "000000001|1347071435|7da7456f7e8f76f4244439aefda651b15eb8d35776e02c26f622abc0533077fb2|get";
    ok = parse_auth_token(large_hash, &user_id, &expiration_time, &hash, &username);
    GS_ASSERT(!ok);
    #if DC_SERVER
    GS_ASSERT(!verify_token(valid_token));
    #endif
    
    const char missing_hash[] = "000000001|1347071435||get";
    ok = parse_auth_token(missing_hash, &user_id, &expiration_time, &hash, &username);
    GS_ASSERT(!ok);
    #if DC_SERVER
    GS_ASSERT(!verify_token(valid_token));
    #endif
    
    const char malformed_username[] = "000000001|1347071435|7da756f7e8f76f4244439aefda651b15eb8d35776e02c26f622abc0533077fb2|g/et";
    ok = parse_auth_token(malformed_username, &user_id, &expiration_time, &hash, &username);
    GS_ASSERT(!ok);
    #if DC_SERVER
    GS_ASSERT(!verify_token(valid_token));
    #endif
    
    const char missing_username[] = "000000001|1347071435|7da756f7e8f76f4244439aefda651b15eb8d35776e02c26f622abc0533077fb2|";
    ok = parse_auth_token(missing_username, &user_id, &expiration_time, &hash, &username);
    GS_ASSERT(!ok);
    #if DC_SERVER
    GS_ASSERT(!verify_token(valid_token));
    #endif
    
    const char small_username[] = "000000001|1347071435|7da756f7e8f76f4244439aefda651b15eb8d35776e02c26f622abc0533077fb2|as";
    ok = parse_auth_token(small_username, &user_id, &expiration_time, &hash, &username);
    GS_ASSERT(!ok);
    #if DC_SERVER
    GS_ASSERT(!verify_token(valid_token));
    #endif
    
    const char large_username[] = "000000001|1347071435|7da756f7e8f76f4244439aefda651b15eb8d35776e02c26f622abc0533077fb2|a23456789qwertya";
    ok = parse_auth_token(large_username, &user_id, &expiration_time, &hash, &username);
    GS_ASSERT(ok);
    // username should be truncated. parses correctly, as the parser only read the token length
    if (ok)
    {
        GS_ASSERT(strcmp(username, "a23456789qwerty") == 0);
        GS_ASSERT(strcmp(hash, "7da756f7e8f76f4244439aefda651b15eb8d35776e02c26f622abc0533077fb2") == 0);
        free(username);
        free(hash);
        #if DC_SERVER
        GS_ASSERT(!verify_token(large_username));
        #endif
    }

    // large username, but another element is truncated
    const char large_username_in_bounds[] = "000000001|1347071435|7da756f7e8f76f4244439aefda651b15eb8d35776e02c26f622abc0533077fb|getgetgetgetgetg";
    ok = parse_auth_token(large_username_in_bounds, &user_id, &expiration_time, &hash, &username);
    GS_ASSERT(!ok);
    #if DC_SERVER
    GS_ASSERT(!verify_token(valid_token));
    #endif
    
    const char last_char_is_delim2[] = "000000001|1347071435|7da756f7e8f76f4244439aefda651b15eb8d35776e02c26f622abc0533077fb2|123456789qwert|";
    ok = parse_auth_token(last_char_is_delim2, &user_id, &expiration_time, &hash, &username);
    GS_ASSERT(!ok);
    #if DC_SERVER
    GS_ASSERT(!verify_token(valid_token));
    #endif

    // Username
    GS_ASSERT(!is_valid_username(""));
    GS_ASSERT(!is_valid_username("g"));
    GS_ASSERT(!is_valid_username("ge"));
    GS_ASSERT(is_valid_username("get"));
    GS_ASSERT(is_valid_username("get_"));
    GS_ASSERT(is_valid_username("get_423"));
    GS_ASSERT(is_valid_username("4get"));
    GS_ASSERT(!is_valid_username("4444"));
    GS_ASSERT(is_valid_username("getgetgetgetget"));
    GS_ASSERT(!is_valid_username("getgetgetgetgetg"));
    GS_ASSERT(!is_valid_username("___"));
    GS_ASSERT(!is_valid_username("_get_"));
    GS_ASSERT(is_valid_username("get_"));

    // timestamp
    GS_ASSERT(auth_token_expired(0, AUTH_TOKEN_LIFETIME));
    GS_ASSERT(auth_token_expired(utc_now() - 60, AUTH_TOKEN_LIFETIME));
    GS_ASSERT(auth_token_expired(AUTH_TOKEN_LIFETIME, AUTH_TOKEN_LIFETIME));
    GS_ASSERT(auth_token_expired(AUTH_TOKEN_LIFETIME + 1, AUTH_TOKEN_LIFETIME));
    GS_ASSERT(auth_token_expired(utc_now() - 60, AUTH_TOKEN_LIFETIME));
    GS_ASSERT(!auth_token_expired(utc_now() + 2, AUTH_TOKEN_LIFETIME));
    GS_ASSERT(!auth_token_expired(utc_now() + AUTH_TOKEN_LIFETIME, AUTH_TOKEN_LIFETIME));
}

void update()
{
    #if DC_SERVER
    server_update();
    #endif
    #if DC_CLIENT
    client_update();
    #endif
}

}   // Auth
