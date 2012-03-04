#pragma once

int FRAME_RATE_THOTTLE_ENABLED = 1;
int FRAME_RATE_THOTTLE_TARGET = 15; //ms for 60, 33 for 30

namespace Profiling
{

#include <c_lib/time/physics_timer.h>

const int STAGES = 4; //includes start and end
/*
    Stage description

    void frame_start()
    void frame_flip_start()
    void frame_map_update_start()
    void frame_voxel_update_start()
    void frame_wait_start()
    void frame_end()
*/


class FrameGraph
{
    public:

    class Texture_surface* ts;

    unsigned int index;
    unsigned int pixel_column;

    long timer[128][STAGES];

    unsigned char stage_color[STAGES][4];

/*
    long _frame_start[128];


    long _frame_map_update_start[128];
    long _frame_voxel_update_start[128];

    long _frame_wait_start[128];
    long _frame_flip_start[128];

    long _frame_end[128];
*/
    FrameGraph()
    {
        index = 0;
        pixel_column = 0;

        for(int i=0; i<128; i++) 
        for(int j=0; j<STAGES; j++)
        {
            timer[i][j] = 0;
        }

        for(int i=0; i<STAGES; i++)
        {
            set_stage_color(i, 255,255,255,255);
        }

        set_stage_color(0, 255, 0,0, 100);
        set_stage_color(1, 0, 255,0, 255);
        set_stage_color(2, 0, 0,255, 50);

    /*
        ts->set_pixel(index, i, 255, 0,0, 100);
        ts->set_pixel(index, i, 0, 255,0, 255);
        ts->set_pixel(index, i, 0, 0,255, 50);
    */

    /*
        for(int i=0; i<128; i++) 
        {
            _frame_start[i] = 0;
            
            _frame_map_update_start[i] = 0;
            _frame_voxel_update_start[i] = 0;

            _frame_wait_start[i] = 0;
            _frame_flip_start[i] = 0;

            _frame_end[i] = 0;
        }
    */
        ts = new Texture_surface(128, 64);
    }

    ~FrameGraph()
    {
        delete ts;
    }

    void set_stage_color(int n, unsigned char r, unsigned char g, unsigned char b, unsigned char a )
    {
        if(n >= STAGES)
        {
            printf("set_stage_color: index error!\n");
            return;
        }
        stage_color[n][0] = r;
        stage_color[n][1] = g;
        stage_color[n][2] = b;
        stage_color[n][3] = a;
    }

    void frame_start()
    {


        int t = _GET_MS_TIME();

        timer[index][STAGES-1] = t;
        index = (index + 1) % 128;
        timer[index][0] = t;

    }

    void frame_stage(int n)
    {
        timer[index][n] = _GET_MS_TIME();
    }

    void frame_wait_stage(int n)
    {
        long _time = _GET_MS_TIME();

        timer[index][n] = _time;
        
        //Sample input every 1ms during wait!!
        if(FRAME_RATE_THOTTLE_ENABLED)
        {
            int wait = FRAME_RATE_THOTTLE_TARGET - (_time - timer[index][0]);
            if( wait <= 0) return;
            usleep(1000*wait);
        }
    }

    void frame_end()
    {
        timer[index][STAGES-1] = _GET_MS_TIME();
        blit_column();
    }

    void blit_column()
    {

        int t[STAGES-1];

        for(int i=0; i< STAGES-1; i++)
        {
            t[i] = timer[index][i+1] - timer[index][i]; 
            if(t[i] < 0) printf("frame_graph timer: WTF temporal error on index %i \n", i);
            if(t[i] == 1) t[i] = 0;
        }

        //for(int i=0; i< 64; i++) ts->set_pixel(index, i, 0, 0,0, 0);


    /*
        R> loop up to flip
        G> flip time
        B> time waiting

    */

        int i = 0;

        for(int k=0; k< STAGES-1; k++)
        {
            int j=0;
            while(j < t[k] && i<64)
            {
                ts->set_pixel(index, i, stage_color[k][0], stage_color[k][1],stage_color[k][2],stage_color[k][3]);
                j++;
                i++;
            }
        }

        while(i < 64 && i<64)
        {
            ts->set_pixel(index, i, 0, 0,0, 0);
            i++;
        }

    }

/*
    void frame_start()
    {

        int t = _GET_MS_TIME();

        _frame_end[index] = t;
        index = (index + 1) % 128;

        _frame_start[index] = t;
    }
*/


/*
    void frame_flip_start()
    {
        _frame_flip_start[index] =  _GET_MS_TIME();
    }

    void frame_map_update_start()
    {
        _frame_map_update_start[index] =  _GET_MS_TIME();

    }

    void frame_voxel_update_start()
    {
        _frame_voxel_update_start[index] =  _GET_MS_TIME();
    }

    void frame_wait_start()
    {
        _frame_wait_start[index] = _GET_MS_TIME();

        
            //Sample input every 1ms during wait!!
        

        if(FRAME_RATE_THOTTLE_ENABLED)
        {
            int wait = FRAME_RATE_THOTTLE_TARGET - (_frame_wait_start[index] - _frame_start[index]);
            if( wait <= 0) return;

            usleep(1000*wait);
        }

    }

    void frame_end()
    {
        _frame_end[index] = _GET_MS_TIME();
        blit_column();
    }
*/

#if 0
    void blit_column()
    {
        //CPU
        //flip length
        //wait

        //int total = _frame_end[index] - _frame_start[index];

        int t1 = _frame_flip_start[index] - _frame_start[index];
        int t2 = _frame_wait_start[index] - _frame_flip_start[index];

        int t3 = _frame_end[index] - _frame_wait_start[index];

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

    /*
        if(t1+t2+t3 >= 64)
        {
            printf("frame_graph: error frame took more than 64 ms\n");
            return;
        }
    */
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

#endif

    void draw(int x, int y)
    {
        ts->draw(x,y);
    }

};

extern FrameGraph* frame_graph;
void init_frame_graph();
void teardown_frame_graph();

}
