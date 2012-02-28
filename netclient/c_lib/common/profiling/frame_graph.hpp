#pragma once

namespace Profiling
{

#include <c_lib/time/physics_timer.h>

class FrameGraph
{
	public:

	class Texture_surface* ts;

	unsigned int index;
	unsigned int pixel_column;

	long _frame_start[128];
	long _frame_flip_start[128];
	long _frame_wait_start[128];
	long _frame_end[128];

	FrameGraph()
	{
		index = 128;
		pixel_column = 0;

		for(int i=0; i<128; i++) 
		{
			_frame_start[i] = 0;
			_frame_wait_start[i] = 0;
			_frame_flip_start[i] = 0;
			_frame_end[i] = 0;
		}

		ts = new Texture_surface(128, 64);
	}

	void frame_start()
	{
		int t = _GET_MS_TIME();

		_frame_end[index] = t;
		index = (index + 1) % 128;

		_frame_start[index] = t;

	}

	void frame_flip_start()
	{
		_frame_flip_start[index] =  _GET_MS_TIME();
	}

	void frame_wait_start()
	{
		_frame_wait_start[index] = _GET_MS_TIME();
	}

	void frame_end()
	{
		_frame_end[index] = _GET_MS_TIME();
		blit_column();
	}

	void blit_column()
	{
		//CPU
		//flip length
		//wait
		int t1 = _frame_flip_start[index] - _frame_start[index];
		int t2 = _frame_wait_start[index] - _frame_flip_start[index];
		int t3 = _frame_end[index] - _frame_wait_start[index];

		if(t1 < 0 || t1 > 64)
		{
			printf("error1\n");
			return;
		}

		if(t2 < 0 || t2 > 64)
		{
			printf("error2\n");
			return;
		}

		if(t2 < 0 || t2 > 64)
		{
			printf("error3\n");
			return;
		}

		if(t1+t2+t3 >= 64)
		{
			printf("error4\n");
			return;
		}

		int i = 0;

		while(++i < t1) ts->set_pixel(index, i, 255, 0,0, 255);
		while(++i < t2) ts->set_pixel(index, i, 0, 255,0, 255);
		while(++i < t1) ts->set_pixel(index, i, 0, 0,255, 255);
	}

	void draw()
	{
		ts->draw();
	}
};

}