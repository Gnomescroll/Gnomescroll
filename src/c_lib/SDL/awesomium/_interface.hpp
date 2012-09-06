#pragma once

#include <Awesomium/awesomium_capi.h>

namespace Awesomium
{

const char AUTH_TOKEN_COOKIE_NAME[] = "gstoken";

void init();
void teardown();
void update();
void draw();

// F1 toggles awesomium window
void enable();
void disable();

void handle_mouse_event(int x, int y, int button, int event_type);
void handle_keyboard_event(union SDL_Event* keyEvent);

void SDL_keyboard_event(const SDL_Event* event);
void SDL_mouse_event(const SDL_Event* event);

void open_url(const char* url);

awe_string* get_awe_string(const char* _str);
char* get_str_from_awe(const awe_string* str);

extern class ChromeViewport* cv;
extern class ViewportManager* viewport_manager;

char* get_cookies();
char* get_auth_token();

}   // Awesomium
