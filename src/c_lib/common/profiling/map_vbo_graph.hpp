#pragma once

namespace Profiling
{

class MapGraph
{
	public:

	class Texture_surface* ts;

	unsigned int index;
	unsigned int pixel_column;

	long _frame_start[128];
	long _frame_flip_start[128];
	long _frame_wait_start[128];
	long _frame_end[128];

	MapGraph()
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

	~MapGraph()
	{
		delete ts;
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

		if(FRAME_RATE_THOTTLE_ENABLED)
		{
			int wait = (int) (FRAME_RATE_THOTTLE_TARGET - (_frame_wait_start[index] - _frame_start[index]));
			if( wait <= 0) return;


            #ifdef __GNUC__
			  usleep(1000*wait);
            #endif

            #ifdef __MSVC__
              Sleep(wait); //ms
            #endif
        }

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

		//int total = _frame_end[index] - _frame_start[index];

		int t1 = (int) (_frame_flip_start[index] - _frame_start[index]);
		int t2 = (int) (_frame_wait_start[index] - _frame_flip_start[index]);
		int t3 = (int) (_frame_end[index] - _frame_wait_start[index]);

		if(t2 <= 1 ) t2 = 0;

		//printf("%i: %i %i %i \n", total, t1,t2,t3);

		if(t1 < 0)
		{
			printf("error1\n");
			return;
		}

		if(t2 < 0)
		{
			printf("error2\n");
			return;
		}

		if(t3 < 0)
		{
			printf("error3\n");
			return;
		}

		if(t1+t2+t3 >= 64)
		{
			printf("frame_graph: error frame took more than 64 ms\n");
			//return;
		}

		for(int i=0; i< 64; i++) ts->set_pixel(index, i, 0, 0,0, 0);


	/*
		R> loop up to flip
		G> flip time
		B> time waiting

	*/

		int i = 0;

		int j;

		j=0;
		while(j < t1 && i<64)
		{
			ts->set_pixel(index, i, 255, 0,0, 100);
			j++;
			i++;
		}

		j=0;
		while(j < t2 && i<64)
		{
			ts->set_pixel(index, i, 0, 255,0, 255);
			j++;
			i++;
		}

		j=0;
		while(j < t3 && i<64)
		{
			ts->set_pixel(index, i, 0, 0,255, 50);
			j++;
			i++;
		}
		while(i < 64 && i<64)
		{
			ts->set_pixel(index, i, 0, 0,0, 0);
			i++;
		}
		//while(++i < t1) ts->set_pixel(index, i, 255, 0,0, 255);
		//while(++i < t1+t2) ts->set_pixel(index, i, 0, 255,0, 255);
		//while(++i < t1+t2+t3) ts->set_pixel(index, i, 0, 0,255, 255);
	}

	void draw(int x, int y)
	{
		ts->draw(x,y);
	}
};

}
