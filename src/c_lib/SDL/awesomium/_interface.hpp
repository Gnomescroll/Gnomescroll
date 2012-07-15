#pragma once


namespace Awesomium
{

	void init();
	void teardown();
	void update();

    void _draw();


	void handle_mouse_event(int x, int y, int button, int event_type);

	void handle_keyboard_event(union SDL_Event* keyEvent);

}