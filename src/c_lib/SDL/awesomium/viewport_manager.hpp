#pragma once

#include <SDL/awesomium/viewport_class.hpp>

namespace Awesomium
{

	const int MAX_VIEWPORTS = 8;;

	class ViewportManager
	{
		public:



		bool in_focus;

		class viewport_class* vpa[MAX_VIEWPORTS]; //viewport class array
		int vpai; //viewport class array index

		ViewportManager()
		{
			in_focus = false;
			for(int i=0; i<MAX_VIEWPORTS; i++) vpa[i] = NULL;
		}


		void add_viewport(class viewport_class* viewport)
		{
			for(int i=0; i < MAX_VIEWPORTS; i++)
			{
				if(vpa[i] == NULL)
				{
					vpa[i] = viewport;
					printf("Attached viewport to slot %i \n", i);
					return;
				}
			}

			printf("Error: viewport_manager, add_viewport; max viewports reached \n");
		}


		void handle_click(int x, int y, int button, int event_type) //up/down
		{
			if(event_type == 0)
				printf("Mouse %i down at: %i %i \n", button,x,y);
			if(event_type == 1)
				printf("Mouse %i down at: %i %i \n", button,x,y);

			for(int i=0; i<MAX_VIEWPORTS; i++)
			{
				if(vpa[i] == NULL) continue;



			}
		}

		void handle_key()
		{

		}

	};


// Awesomium::LEFT_MOUSE_BTN
// Awesomium::MIDDLE_MOUSE_BTN
// Awesomium::RIGHT_MOUSE_BTN

// mouse down
// myWebView->injectMouseDown(Awesomium::LEFT_MOUSE_BTN);

// mouse up
// myWebView->injectMouseUp(Awesomium::LEFT_MOUSE_BTN);

// mouse scroll
// myWebView->injectMouseWheel(100);
}