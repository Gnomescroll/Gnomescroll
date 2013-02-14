#include "client.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <SDL/awesomium/_interface.hpp>

namespace Auth
{

char* auth_token = NULL;
UserID auth_token_user_id = NULL_USER_ID;
time_t auth_token_timestamp = 0;
char* auth_token_hash = NULL;
char* auth_token_username = NULL;
static int token_retries = 0;

bool authorized = false;
bool has_authorized_once = false;
bool needs_login = false;
bool refreshing_token = false;
bool token_failure = false;

bool token_available = false;   // becomes true after first time token exists and appears valid

double offset_time = 0;

static bool should_request_token = false;

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
    if (!NetClient::Server.connected) return;
    if (!needs_login)
    {   // only attempt the token navigation if we dont think we need to login
        // otherwise it will reload the login page while they should be using it
        if (auth_token_expiring(auth_token_timestamp))
        {
            expiration_tick++;
            if (expiration_tick % AUTH_TOKEN_RETRY_WAIT == 0 && !refreshing_token)
            {
                printf("Refreshing token\n");
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
    enable_awesomium();
}

AuthError update_token(const char* token)
{
    GS_ASSERT(token != NULL);
    if (token == NULL) return AUTH_ERROR_TOKEN_MISSING;

    if (!load_auth_token(token))
        return AUTH_ERROR_TOKEN_MALFORMED;
    // dont check timestamp expiration, because client's clock may be wrong
    Awesomium::set_game_token_cookie(token, auth_token_timestamp);

    send_auth_token(token);

    token_available = true;

    return AUTH_ERROR_NONE;
}

void token_was_accepted()
{
    token_retries = 0;   // reset retry counter
    authorized = true;
    has_authorized_once = true;
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
    if (token_retries == 0)
    {
        printf("Token denied\n");
        should_request_token = true;
    }
    token_retries++;
}

void check_should_load_token()
{
    if (should_request_token && Awesomium::login_page_loaded())
    {
        Awesomium::open_token_page();
        should_request_token = false;
    }
}

void client_update()
{
    check_should_load_token();
    check_expiring_token();
}

void client_init()
{
}

void client_teardown()
{
    free(auth_token);
    free(auth_token_hash);
    free(auth_token_username);
}

void set_time_offset(time_t server_time)
{
    time_t now = utc_now();
    offset_time = difftime(server_time, now);

    #ifdef _WIN32
    printf("Server-client time discrepancy: %I64d\n", (long long)offset_time);
    #else
    printf("Server-client time discrepancy: %lld\n", (long long)offset_time);
    #endif
}

}   // Auth
