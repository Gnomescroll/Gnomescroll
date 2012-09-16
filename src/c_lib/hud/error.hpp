#pragma once

#include <hud/text.hpp>
#include <auth/client.hpp>

typedef enum
{
    // UI will render a message based on most important error
    GS_ERROR_NONE = 0,

    GS_ERROR_NOT_CONNECTED,
    GS_ERROR_WAS_DISCONNECTED,

    GS_ERROR_REAUTHORIZING,     // not really an error
    GS_ERROR_AUTH_FAILED,
    GS_ERROR_AUTH_NOT_AVAILABLE,

    GS_ERROR_NEEDS_LOGIN,

    GS_ERROR_VERSION_MISMATCH,  // highest priority
} GSError;

namespace Hud
{

static const char not_connected_msg[] = "Server not connected";
static const char was_disconnected_msg[] = "Disconnected by server";
static const char reauth_msg[] = "Reauthorizing...";
static const char auth_failed_msg[] = "Authorization failed";
static const char auth_server_failed_msg[] = "Authentication server failure."; 
static const char auth_na_msg[] = "Login server not available";
static const char needs_login_msg[] = "Login required to continue";
static const char version_mismatch_msg[] = "Your game version is\nout of date.\nGet the new version from\nwww.gnomescroll.com";
char* disconnect_msg = NULL;

const unsigned int N_GSERRORS = 7;

bool errors[N_GSERRORS] = { false };

void init_errors()
{
    errors[GS_ERROR_NONE] = true;
    errors[GS_ERROR_NOT_CONNECTED] = true;
}

void set_error_status(GSError err)
{
    GS_ASSERT(err >= 0 && err < N_GSERRORS);
    if (err < 0 || err >= N_GSERRORS) return;
    
    errors[err] = true;
}

void unset_error_status(GSError err)
{
    GS_ASSERT(err >= 0 && err < N_GSERRORS);
    if (err < 0 || err >= N_GSERRORS) return;
    
    errors[err] = false;
}

void set_server_disconnect_message(const char* msg)
{
    printf("%s\n", msg);
    if (disconnect_msg != NULL) free(disconnect_msg);
    disconnect_msg = (char*)malloc((strlen(msg)+1)*sizeof(char));
    strcpy(disconnect_msg, msg);
}

GSError get_primary_error()
{
    for (unsigned int i=N_GSERRORS-1; i>0; i--)
        if (errors[i]) return (GSError)i;
    return GS_ERROR_NONE;
}

bool has_error()
{
    return (get_primary_error() != GS_ERROR_NONE);
}

bool update_error_text(class HudText::Text* t)
{
    GS_ASSERT(t != NULL);
    if (t == NULL) return false;
    
    GSError err = get_primary_error();
    if (err == GS_ERROR_NONE) return false;

    if (err == GS_ERROR_NOT_CONNECTED)
        t->set_text(not_connected_msg);
    else if (err == GS_ERROR_WAS_DISCONNECTED)
    {
        // if there was a known token failure that has not recovered,
        // and we are disconnected for an auth reason, say that the auth server failed
        if (Auth::token_failure &&
            (NetClient::Server.disconnect_code == DISCONNECT_AUTH_EXPIRED
        || NetClient::Server.disconnect_code == DISCONNECT_AUTH_LIMIT
        || NetClient::Server.disconnect_code == DISCONNECT_AUTH_TIMEOUT))
            t->set_text(auth_server_failed_msg);
        else
        {
            if (disconnect_msg != NULL)
                t->set_text(disconnect_msg);
            else
                t->set_text(was_disconnected_msg);
        }
    }
    else if (err == GS_ERROR_REAUTHORIZING)
        t->set_text(reauth_msg);
    else if (err == GS_ERROR_AUTH_FAILED)
        t->set_text(auth_failed_msg);
    else if (err == GS_ERROR_AUTH_NOT_AVAILABLE)
        t->set_text(auth_na_msg);
    else if (err == GS_ERROR_NEEDS_LOGIN)
        t->set_text(needs_login_msg);
    else if (err == GS_ERROR_VERSION_MISMATCH)
        t->set_text(version_mismatch_msg);
    else
    {
        GS_ASSERT(false);
    }
    return true;
}


void error_init()
{
}

void error_teardown()
{
    if (disconnect_msg != NULL) free(disconnect_msg);
}

};
