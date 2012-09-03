#pragma once

namespace Awesomium
{

void init();
void teardown();
void update();
void draw();

void handle_mouse_event(int x, int y, int button, int event_type);
void handle_keyboard_event(union SDL_Event* keyEvent);

awe_string* get_awe_string(const char* _str);

extern class ChromeViewport* cv;
extern class ViewportManager* viewport_manager;

}   // Awesomium
