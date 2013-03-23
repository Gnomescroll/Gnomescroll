#pragma once

#include <common/input_buffer.hpp>
#include <hud/font.hpp>
//#include <t_map/common/types.hpp>

namespace Hud
{

const size_t TERMINAL_CHAR_WIDTH = 8;
const size_t TERMINAL_CHAR_HEIGHT = 8;
const size_t TERMINAL_BUFFER_SIZE = TERMINAL_CHAR_WIDTH * TERMINAL_CHAR_HEIGHT;

class TerminalRenderer
{
    public:
        int cursor_x;
        int cursor_y;        
        int prev_blink;
        bool blink_status_visible;
        HudText::Text* grid;
        HudText::Text input_text;
        InputBuffer input_buffer;

    TerminalRenderer() :
        input_buffer(TERMINAL_BUFFER_SIZE)
    {
        cursor_x = 0;
        cursor_y = 0;        
        prev_blink =_GET_MS_TIME();
        blink_status_visible = true;

        grid = new HudText::Text[TERMINAL_BUFFER_SIZE];
        //grid = NULL;
        for (int i = 0; i < TERMINAL_BUFFER_SIZE; i++) 
        {
            int x = i % TERMINAL_CHAR_WIDTH;
            int y = i / TERMINAL_CHAR_WIDTH;
            int span = 20;
                        
            grid[i].set_text("#");
            //grid[i].set_scale(0.7f);
            grid[i].set_position(255 + x*span, 255 + y*span);        
            grid[i].shadowed = true;        
            //grid[i].center();
        }
    }

    ~TerminalRenderer()
    {
        delete[] grid;
    }

    void left()
    { // don't just wrap, do like a text editor
        cursor_x -= 1;
        if (cursor_x < 0)
        {
            cursor_x = TERMINAL_CHAR_WIDTH - 1;
            cursor_y++;
            cursor_y = GS_MIN(cursor_y, TERMINAL_CHAR_HEIGHT - 1);
        }
        else
        {
            cursor_x = GS_MAX(cursor_x, 0);
        }
    }

    void right()
    { // don't just wrap, do like a text editor
        cursor_x += 1;
        if (cursor_x >= TERMINAL_CHAR_WIDTH)
        {
            cursor_x = 0;
            cursor_y--;
            cursor_y = GS_MAX(cursor_y, 0);
        }
        else
        {
            cursor_x = GS_MIN(cursor_x, TERMINAL_CHAR_WIDTH - 1);
        }
    }

    void up()
    {
        cursor_y += 1;
        cursor_y = GS_MIN(cursor_y, TERMINAL_CHAR_HEIGHT - 1);
    }

    void down()
    {
        cursor_y -= 1;
        cursor_y = GS_MAX(cursor_y, 0);
    }

    void reset_blink()
    {
        // the whole point of blinking is drawing your eye to the focal/selection point with some activity.
        // but while you're cursoring around, you have MOVEment activity, and want to make sure every position
        // visited, gets a period of visibility
        prev_blink = _GET_MS_TIME();
        blink_status_visible = true;
    }

    void draw()
    {
        // update blinking
        if (125 < _GET_MS_TIME() - prev_blink)
        {
            prev_blink = _GET_MS_TIME();
            blink_status_visible = !blink_status_visible;
        }
    
        // render the grid
        for (int i = 0; i < TERMINAL_BUFFER_SIZE; i++) 
        {
            int x = i % TERMINAL_CHAR_WIDTH;
            int y = i / TERMINAL_CHAR_WIDTH;

            if (blink_status_visible && /* selected */ x == cursor_x && y == cursor_y)
                grid[i].set_color(COLOR_GREEN);
            else
                grid[i].set_color(Color(128-x*12, 0, 128+y*12));

            grid[i].draw();
        }

        // show whatever text thats been input
        input_text.set_position(600, 600);
        input_text.set_text(input_buffer.buffer);
        input_text.draw();
        //if (drawn_width >= this->render_width) y += line_height;
        //if (drawn_width >= this->render_width) { y += line_height; x = 0; }
    }
};

//<rdn> know that you Text will handle almost everything. you only might have to add a new draw method and fixed width to wrap on, if its not there
//<rdn> the other thing you have to know is that input needs to be routed appropriately
//<rdn> input/handlers.cpp
//<rdn> you can model the input routing off of cube selector
//<rdn> you can model the input routing off of how cube selector is routed to
//<rdn> there are a few global input states
//<rdn> agent, camera, awesomium, chat, at least
//<rdn> dont add a new one, this will be in agent mode only
//<rdn> there is a struct InputState which holds a bunch of subvariables, you should see something for cube_selector there

//<rdn> and if they are pointing at it and are in range, display the screen, activate the cursor and set input_state.terminal = true;
//<rdn> when they are not looking, dont display anything, close any active cursors, and set input_state.terminal = false;
//<rdn> in agent_key_up_handler() { if (input_state.terminal) { Hud::terminal.handle_key_press(key); } }
//<rdn> in agent_key_up_handler() { if (input_state.terminal) { Hud::terminal.handle_key_press(key); return; } }
//<rdn> actually this doesnt work
//<rdn> it will require activation and deactivation

}   // Hud
