#include "client.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <SDL/awesomium/_interface.hpp>

namespace Auth
{

char* auth_token = NULL;
int auth_token_user_id = 0;
time_t auth_token_timestamp = 0;
char* auth_token_hash = NULL;
char* auth_token_username = NULL;
static int token_retries = 0;

bool authorized = false;
bool needs_login = false;
bool refreshing_token = false;
bool token_failure = false;

bool token_available = false;   // becomes true after first time token exists and appears valid

bool send_auth_token()
{
    return send_auth_token(auth_token);
}

bool send_auth_token(const char* token)
{
    GS_ASSERT(token != NULL);
    if (token == NULL) return false;
    auth_token_CtoS msg;
    strncpy(msg.token, token, AUTH_TOKEN_LENGTH);
    msg.token[AUTH_TOKEN_LENGTH] = '\0';
    msg.send();
    return true;
}

bool load_auth_token(const char* token)
{
    GS_ASSERT(token != NULL);
    if (token == NULL) return false;
    
    if (auth_token != NULL)
    {
        free(auth_token);
        auth_token = NULL;
    }

    bool ok = parse_auth_token(token, &auth_token_user_id, &auth_token_timestamp, &auth_token_hash, &auth_token_username);
    GS_ASSERT(ok); // should be valid if exists
    if (!ok) return false;

    size_t len = strlen(token);
    auth_token = (char*)malloc((len+1)*sizeof(char));
    strcpy(auth_token, token);
    refreshing_token = false;   // token is considered refreshed by this point

    return true;
}

bool auth_token_expired(const time_t timestamp)
{
    return auth_token_expired(timestamp, AUTH_TOKEN_LIFETIME);
}

bool auth_token_expiring(const time_t timestamp)
{
    return auth_token_expired(timestamp, AUTH_TOKEN_LIFETIME/2);
}

void check_expiring_token()
{
    static int expiration_tick = 0;
    static int expiration_attempts = 0;
    if (auth_token == NULL) return;
    if (!needs_login)
    {   // only attempt the token navigation if we dont think we need to login
        // otherwise it will reload the login page while they should be using it
        if (auth_token_expiring(auth_token_timestamp))
        {
            expiration_tick++;
            if (expiration_tick % AUTH_TOKEN_RETRY_WAIT == 0 && !refreshing_token)
            {
                Awesomium::open_token_page();
                refreshing_token = true;
                expiration_attempts++;
            }
        }
        else
        {
            expiration_attempts = 0;
            expiration_tick = 0;
        }
    }

    if (authorized)
        Hud::unset_error_status(GS_ERROR_AUTH_FAILED);
}

void begin_auth()
{
    GS_ASSERT(auth_token == NULL);

    // check if auth token already exists
    char* token = Awesomium::get_auth_token();
    if (token != NULL && load_auth_token(token) && !auth_token_expiring(auth_token_timestamp))
    {
        token_available = true;
        send_auth_token();  // we won't be connected yet, but the message will still queue
    }
    else // display awesomium (should already have the login page set)
        enable_awesomium();
    if (token != NULL)
        free(token);
}

AuthError update_token(const char* token)
{
    GS_ASSERT(token != NULL);
    if (token == NULL) return AUTH_ERROR_TOKEN_MISSING;

    if (!load_auth_token(token))
        return AUTH_ERROR_TOKEN_MALFORMED;

    if (auth_token_expired(auth_token_timestamp))
        return AUTH_ERROR_TOKEN_EXPIRED;

    Awesomium::set_game_token_cookie(token, auth_token_timestamp);

    send_auth_token(token);

    token_available = true;

    return AUTH_ERROR_NONE;
}

void token_was_accepted()
{
    token_retries = 0;   // reset retry counter
    authorized = true;
    needs_login = false;
    Hud::unset_error_status(GS_ERROR_NEEDS_LOGIN);
    Hud::unset_error_status(GS_ERROR_AUTH_FAILED);
    Hud::unset_error_status(GS_ERROR_REAUTHORIZING);
    disable_awesomium();
    printf("Token accepted\n");
}

void token_was_denied()
{   // request new token from auth server
    authorized = false;
    Hud::set_error_status(GS_ERROR_AUTH_FAILED);
    if (token_retries == 0) printf("Token denied\n");
    token_retries++;
    Awesomium::open_token_page();   // try once more, fresh from the auth server
}

void client_init()
{
}

void client_teardown()
{
    if (auth_token != NULL) free(auth_token);
    if (auth_token_hash != NULL) free(auth_token_hash);
    if (auth_token_username != NULL) free(auth_token_username);
}

}   // Auth