#pragma once

int FRAME_RATE_THOTTLE_ENABLED = 1;
int FRAME_RATE_THOTTLE_TARGET = 15; //ms for 60, 33 for 30

#include <common/time/physics_timer.hpp>
#include <input/input.hpp>

namespace Profiling
{

const int STAGES = 8; //includes start and end

/*
    Stage description

    0: physics tick
    1: misc stuff and network
    2: call draw functions
    3: map and voxel updates
    4: swap buffers
    5: wait stage
    6: final
*/


class FrameGraph
{
    public:

        class Texture_surface* ts;

        unsigned int index;
        unsigned int pixel_column;

        long timer[128][STAGES];

        unsigned char stage_color[STAGES][4];

    FrameGraph()
    {
        index = 0;
        pixel_column = 0;

        for (int i=0; i<128; i++)
        for (int j=0; j<STAGES; j++)
        {
            timer[i][j] = 0;
        }

        for (int i=0; i<STAGES; i++)
        {
            set_stage_color(i, 255,255,255,255);
        }
        /*
        set_stage_color(0, 255, 0,0, 70);
        set_stage_color(1, 255, 0,0, 150);
        set_stage_color(2, 255, 0,0, 200);
        set_stage_color(3, 255, 0,255, 100);
        set_stage_color(4, 0, 255,0, 255);
        set_stage_color(5, 0, 0,255, 50);
        */
        set_stage_color(0, 70, 0,0, 255);   //physics tick
        set_stage_color(1, 150, 0,0, 255);
        set_stage_color(2, 200, 0,0, 255);
        set_stage_color(3, 255, 0,255, 100);    //map a updates

        //set_stage_color(4, 255, 255,100, 100);      //voxel updates
        set_stage_color(4, 255, 0,255, 100);      //voxel updates

        set_stage_color(5, 0, 255,0, 50);       //swap buffers
        set_stage_color(6, 0, 0, 150, 75);    //wait stage

        ts = new Texture_surface(128, 64);
    }

    ~FrameGraph()
    {
        delete ts;
    }

    void set_stage_color(int n, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    {
        if (n >= STAGES)
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

        #if 0
        if (FRAME_RATE_THOTTLE_ENABLED)
        {
            int wait = FRAME_RATE_THOTTLE_TARGET - (_time - timer[index][0]);
            if (wait <= 0) return;
            usleep(1000*wait);
        }
        #endif
        if (FRAME_RATE_THOTTLE_ENABLED)
        {
            //int t_start = _time;

            while ((FRAME_RATE_THOTTLE_TARGET - (_GET_MS_TIME() - timer[index][0])) > 0)
            {
                poll_mouse();
            #ifdef __GNUC
                usleep(1000); //one millisecond
            #endif

            #ifdef __MSVC__
              Sleep(3); //ms
            #endif
            }
        }
    }

    void frame_end(int n)
    {
        if (n != STAGES-1) printf("Warning: frame_graph, frame_end() \n");
        timer[index][STAGES-1] = _GET_MS_TIME();
        blit_column();
    }

    void blit_column()
    {

        int t[STAGES-1];

        for (int i=0; i< STAGES-1; i++)
        {
            t[i] = (int) (timer[index][i+1] - timer[index][i]);
            if (t[i] < 0) printf("frame_graph timer: WTF temporal error on index %i \n", i);
            //if (t[i] == 1) t[i] = 0;
            if (t[i] == 0) t[i] = 1;
        }

    /*
        for (int i=0; i< STAGES-1; i++)
        {
            if (t[i] > 5) printf("warning: stage %i took %i \n", i, t[i]);
        }
    */

    /*
        R> loop up to flip
        G> flip time
        B> time waiting

    */

        int i = 0;

        for (int k=0; k< STAGES-1; k++)
        {
            int j=0;
            while (j < t[k] && i<64)
            {
                ts->set_pixel(index, i, stage_color[k][0], stage_color[k][1],stage_color[k][2],stage_color[k][3]);
                j++;
                i++;
            }
        }

        while (i < 64)
        {
            ts->set_pixel(index, i, 0, 0,0, 0);
            i++;
        }

    }

    void draw(int x, int y)
    {
        ts->draw(x,y);
    }

};

extern FrameGraph* frame_graph;
void init_frame_graph();
void teardown_frame_graph();

} // Profiling
