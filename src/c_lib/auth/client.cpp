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
static int token_retries = 0;

bool authorized = false;

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

bool load_auth_token(char* token)
{
    GS_ASSERT(token != NULL);
    if (token == NULL) return false;
    
    if (auth_token != NULL)
    {
        free(auth_token);
        auth_token = NULL;
    }
    
    bool ok = parse_auth_token(token, &auth_token_user_id, &auth_token_timestamp, &auth_token_hash);
    GS_ASSERT(ok); // should be valid if exists
    if (!ok) return false;

    auth_token = token;

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

void begin_auth()
{
    GS_ASSERT(auth_token == NULL);

    // check if auth token already exists
    char* token = Awesomium::get_auth_token();
    if (token != NULL && load_auth_token(token) && !auth_token_expiring(auth_token_timestamp))
    {
        send_auth_token(token);
        return;
    }
    
    // navigate to token url
    // it will lead the user to login if need be; then redirect to token again
    // an awesomium callback will catch the cookie
    Awesomium::open_url(GNOMESCROLL_URL GNOMESCROLL_TOKEN_PATH);
}

AuthError update_token(char* token)
{
    GS_ASSERT(token != NULL);
    if (token == NULL) return AUTH_ERROR_TOKEN_MISSING;

    bool same_token = (auth_token != NULL && strcmp(token, auth_token) == 0);

    if (!load_auth_token(token))
        return AUTH_ERROR_TOKEN_MALFORMED;

    if (auth_token_expired(auth_token_timestamp))
        return AUTH_ERROR_TOKEN_EXPIRED;

    if (!same_token)
        send_auth_token(token);

    return AUTH_ERROR_NONE;
}

void token_was_accepted()
{
    disable_awesomium(); // hide awesomium
    token_retries = 0;   // reset retry counter
    authorized = true;
}

void token_was_denied()
{   // request new token from auth server
    if (token_retries >= MAX_TOKEN_RETRIES)
    {
        disable_awesomium();
        return;
    }
    Awesomium::open_url(GNOMESCROLL_URL GNOMESCROLL_TOKEN_PATH);
    authorized = false;
    token_retries++;
}

void client_init()
{
}

void client_teardown()
{
    if (auth_token != NULL) free(auth_token);
    if (auth_token_hash != NULL) free(auth_token_hash);
}

}   // Auth
