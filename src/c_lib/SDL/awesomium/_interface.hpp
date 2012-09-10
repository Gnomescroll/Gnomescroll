#pragma once

#include <Awesomium/awesomium_capi.h>

namespace Awesomium
{

#define _BASE_USER_AGENT "Mozilla/5.0 AppleWebKit/533.3 (KHTML, like Gecko) Chrome/12.0.702.0 Safari/533.3 Awesomium/1.6.5"
const char BASE_USER_AGENT[] = _BASE_USER_AGENT;
const char USER_AGENT[] = _BASE_USER_AGENT " Gnomescroll/" STR(GS_VERSION);

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
void open_file(const char* file);
void open_token_page();
void open_login_page();

bool url_is_whitelisted(const char* url);

awe_string* get_awe_string(const char* _str);
char* get_str_from_awe(const awe_string* str);

extern class ChromeViewport* cv;
extern class ViewportManager* viewport_manager;

char* get_cookies();
char* get_auth_token();

void delete_cookie();
void delete_auth_token_cookie();
void delete_all_cookies();

void check_for_token_cookie(const awe_string* _url);

}   // Awesomium
