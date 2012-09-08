#pragma once

#include <hud/text.hpp>

namespace Hud
{

static const char not_connected_msg[] = "Server not connected";
static const char was_disconnected_msg[] = "Disconnected by server";
static const char reauth_msg[] = "Reauthorizing...";
static const char auth_failed_msg[] = "Authorization failed";
static const char auth_na_msg[] = "Login server not available";
static const char version_mismatch_msg[] = "Your game version is\nout of date.\nGet the new version from\nwww.gnomescroll.com";

typedef enum
{
    // TODO -- error code, rank by priority
    // UI will render a message based on most important error
    GS_ERROR_NONE = 0,

    GS_ERROR_NOT_CONNECTED,
    GS_ERROR_WAS_DISCONNECTED,

    GS_ERROR_REAUTHORIZING,     // not really an error
    GS_ERROR_AUTH_FAILED,
    GS_ERROR_AUTH_NOT_AVAILABLE,

    GS_ERROR_VERSION_MISMATCH,  // highest priority
} GSError;

const unsigned int N_GSERRORS = 7;

bool errors[N_GSERRORS] = { false };

void init_errors()
{
    errors[GS_ERROR_NONE] = true;
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

void update_error_text(class HudText::Text* t)
{
    GS_ASSERT(t != NULL);
    if (t == NULL) return;
    
    GSError err = get_primary_error();
    if (err == GS_ERROR_NONE) return;

    if (err == GS_ERROR_NOT_CONNECTED)
        t->set_text(not_connected_msg);
    else if (err == GS_ERROR_WAS_DISCONNECTED)
        t->set_text(was_disconnected_msg);
    else if (err == GS_ERROR_REAUTHORIZING)
        t->set_text(reauth_msg);
    else if (err == GS_ERROR_AUTH_FAILED)
        t->set_text(auth_failed_msg);
    else if (err == GS_ERROR_AUTH_NOT_AVAILABLE)
        t->set_text(auth_na_msg);
    else if (err == GS_ERROR_VERSION_MISMATCH)
        t->set_text(version_mismatch_msg);
    else
    {
        GS_ASSERT(false);
    }
}

};
