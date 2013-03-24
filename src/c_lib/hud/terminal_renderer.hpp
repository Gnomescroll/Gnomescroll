#pragma once

#include <common/input_buffer.hpp>
#include <hud/font.hpp>
//#include <t_map/common/types.hpp>

namespace Hud
{

const int TERMINAL_MAX_CHARS_WIDE = 8;
const int TERMINAL_MAX_CHARS_HIGH = 8;
const int TERMINAL_BUFFER_SIZE = TERMINAL_MAX_CHARS_WIDE * TERMINAL_MAX_CHARS_HIGH;

class TerminalRenderer
{
    private:
        static const int CELL_SPAN = 20;
        static const int DIST_FROM_BOTT_EDGE = 257;
        int cursor_x;
        int cursor_y;        
        int curr_cursor_w;
        int curr_cursor_h;
        HudText::Text* grid;
        InputBuffer input_buffer;
        // blinking text
        int prev_blink;
        bool blink_status_visible;
    public:

    // construct
    TerminalRenderer() :
        input_buffer((size_t)TERMINAL_BUFFER_SIZE)
    {
        cursor_x = 0;
        cursor_y = TERMINAL_MAX_CHARS_HIGH - 1; 
        curr_cursor_w = 0;
        curr_cursor_h = 0;

        prev_blink =_GET_MS_TIME();
        blink_status_visible = true;
        grid = new HudText::Text[TERMINAL_BUFFER_SIZE];
        //IF_ASSERT(grid == NULL) return;           ???????maybe???????

        for (int i = 0; i < TERMINAL_BUFFER_SIZE; i++)
        {
            int x = i % TERMINAL_MAX_CHARS_WIDE;
            int y = i / TERMINAL_MAX_CHARS_WIDE;
            grid[i].set_text(" ");

            char c;
            switch(i)
            {
                case 0: c = 't'; break;
                case 1: c = 'o'; break;
                case 2: c = 'g'; break;
                case 3: c = 'g'; break;
                case 4: c = 'l'; break;
                case 5: c = 'e'; break;
                
                case 8: c = 'I'; break;
                case 9: c = 'N'; break;
                case 10: c = 'S'; break;
                case 11: c = 'E'; break;
                case 12: c = 'R'; break;
                case 13: c = 'T'; break;
                case 14: c = ' '; break;
                case 15: c = '='; break;
                
                case 24: c = 'T'; break;
                case 25: c = 'E'; break;
                case 26: c = 'R'; break;
                case 27: c = 'M'; break;
                case 28: c = 'I'; break;
                case 29: c = 'N'; break;
                case 30: c = 'A'; break;
                case 31: c = 'L'; break;
                default: c = ' '; break;
            }

            grid[i].text[0] = c;
            grid[i].set_scale(1.3f);

            int offs = CELL_SPAN - grid[i].get_width() / 2; // offset for centering text
            // get_width() seems to return zero... i guess maybe cuz not using set_text()?
            grid[i].set_position(x*CELL_SPAN + 4, DIST_FROM_BOTT_EDGE + y*CELL_SPAN);        
            //grid[i].shadowed = true;
        }
    }

    // destruct
    ~TerminalRenderer()
    {
        delete[] grid;
    }

    void add(char c)
    {
        input_buffer.add(c);
        int i = cursor_x + cursor_y*TERMINAL_MAX_CHARS_WIDE;
        grid[i].text[0] = c;
        right();
    }

    void set_cursor_x(int x)
    {
        cursor_x = x;
    }

    void left()
    { // don't just wrap, do like a text editor
        cursor_x--;

        if (cursor_x < 0)
        {
            cursor_x = TERMINAL_MAX_CHARS_WIDE - 1;
            cursor_y++;

            if (cursor_y >= TERMINAL_MAX_CHARS_WIDE)
                cursor_y = 0;
        }
    }

    void right()
    { // don't just wrap, do like a text editor
        cursor_x++;

        if (cursor_x >= TERMINAL_MAX_CHARS_WIDE)
        {
            cursor_x = 0;
            cursor_y--;

            if (cursor_y < 0)
                cursor_y = TERMINAL_MAX_CHARS_WIDE - 1;
        }
    }

    void up()
    {
        cursor_y++;
        if (cursor_y >= TERMINAL_MAX_CHARS_WIDE)
            cursor_y = 0;
    }

    void down()
    {
        cursor_y--;
        if (cursor_y < 0)
            cursor_y = TERMINAL_MAX_CHARS_WIDE - 1;
    }

    void reset_blink()
    {
        // the whole point of blinking is drawing your eye to the focal/selection point with some activity.
        // but while you're cursoring around, you have MOVEment activity, and want to make sure every position
        // visited, gets a period of visibility
        prev_blink = _GET_MS_TIME();
        blink_status_visible = true;
    }

    void draw_cursor()
    {
        curr_cursor_w++;
        if (curr_cursor_w > CELL_SPAN)
            curr_cursor_w = 0;
        curr_cursor_h++;
        if (curr_cursor_h > CELL_SPAN)
            curr_cursor_h = 0;

        // draw twice for an outline
        int mar = 1;  // margin
        draw_rect(
            COLOR_BLACK,
            cursor_x*CELL_SPAN + (CELL_SPAN - curr_cursor_w) / 2 - mar,
            cursor_y*CELL_SPAN + (CELL_SPAN - curr_cursor_h) / 2 - mar + DIST_FROM_BOTT_EDGE - CELL_SPAN,
            curr_cursor_w + mar * 2,
            curr_cursor_h + mar * 2);
        draw_rect(
            COLOR_WHITE,
            cursor_x*CELL_SPAN + (CELL_SPAN - curr_cursor_w) / 2,
            cursor_y*CELL_SPAN + (CELL_SPAN - curr_cursor_h) / 2 + DIST_FROM_BOTT_EDGE - CELL_SPAN,
            curr_cursor_w,
            curr_cursor_h);
    }

    void draw_background()
    {
        // render the grid
        for (int i = 0; i < TERMINAL_BUFFER_SIZE; i++)
        {
            int x = i % TERMINAL_MAX_CHARS_WIDE;
            int y = i / TERMINAL_MAX_CHARS_WIDE;
            draw_rect(Color(19,19,19), x*CELL_SPAN, DIST_FROM_BOTT_EDGE + y*CELL_SPAN - CELL_SPAN, CELL_SPAN, CELL_SPAN);
        }
    }

    void draw_text()
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
            int x = i % TERMINAL_MAX_CHARS_WIDE;
            int y = i / TERMINAL_MAX_CHARS_WIDE;

            //if (blink_status_visible && /* selected --> */ x == cursor_x && y == cursor_y)
            //    grid[i].set_color(COLOR_GREEN);
            //else
                grid[i].set_color(Color(128-x*12, 0, 128+y*12));

            grid[i].draw();
        }
    }
};


//if (drawn_width >= this->render_width) { y += line_height; x = 0; }

//<rdn> you might have to add a new draw method and fixed width to wrap on, if its not there
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