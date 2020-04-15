#include <stdio.h>
#include <string.h>
#include "sugi.h"

// term character parameters
uint8_t term_char_x = 4;
uint8_t term_char_y = 6;
// terminal parameters
uint8_t term_line_y = 0;
uint8_t term_offset_y = 0;
uint8_t term_shift_y = 0;
// terminal cursor
uint8_t term_cursor_active = 0;
uint8_t term_cursor_blink_dur = 7;
uint8_t term_cursor_blink_t = 0;
// text buffer
uint8_t term_cmd_text[1024];


void sugi_term_init()
{
    sugi_core_set_update(sugi_term_update);
    sugi_text_set_target(&term_cmd_text);
    sugi_text_set_mode(SUGI_TEXTMODE_TEXT);
}

// 5351 4520 0023 4341 - Molosina 

void sugi_term_update()
{
    int32_t text_len   = strlen(term_cmd_text);
    int32_t cursor_pos = sugi_text_field_struct.cursor_pos;
    // text foreground
    sugi_gfx_rect(0,                            term_offset_y + term_char_y * term_line_y, 
                  term_char_x * (text_len + 4), term_offset_y + term_char_y * (term_line_y + 1), 1, 0);
    // print text
    sugi_gfx_print(">",                         0, term_line_y * term_char_y + term_offset_y, 7);
    sugi_gfx_print(term_cmd_text, term_char_x * 2, term_line_y * term_char_y + term_offset_y, 7);
    // blink cursor
    term_cursor_blink_t++;
    if (term_cursor_blink_t > term_cursor_blink_dur)
    {
        term_cursor_blink_t = 0;
        term_cursor_active = ~term_cursor_active;
    }

    // check for enter as a last character (TODO: Replace it, as it is not reliable!)
    if (term_cmd_text[text_len - 1] == '\n')
    {
        term_line_y++;
        term_cursor_active = 0;
        // Execute string in lua
        sugi_text_backspace_char();
        luaL_dostring(L, term_cmd_text);
        // Check for a clear log
        if (SUGI_LUA_LOG_CLEAR == sugi_lua_cmd_get_last_log())
        {
            sugi_gfx_camera(0, 0);
            term_offset_y = 0;
            term_shift_y = 0;
            term_line_y = 0;
        }
        // Scroll down
        if ((term_line_y + 1) * term_char_y > SUGI_RENDER_HEIGHT)
        {
            // Caclulating how many lines we need to shift up to get scrolling effect
            if (term_shift_y == 0) 
            {
                term_shift_y  = term_char_y - (term_line_y + 1) * term_char_y - SUGI_RENDER_HEIGHT - 1;
                term_offset_y = term_shift_y;
            } 
            else 
            {
                term_shift_y = term_char_y;
            }
            // defining shift parameters
            int offset = (SUGI_RENDER_WIDTH / 2) * term_shift_y;
            int size   = SUGI_MEM_SCREEN_SIZE - offset;
            // shifting screen upwards and filling a gap
            sugi_mem_memcpy_addr(0, offset, size);
            sugi_gfx_rect(0, SUGI_RENDER_HEIGHT - term_shift_y, SUGI_RENDER_WIDTH, SUGI_RENDER_HEIGHT, 1, 0);
            // subtracting line y, because technically not moving
            term_line_y--;
        }
        // Clear cmd string
        sugi_lua_cmd_add_log(SUGI_LUA_LOG_NOP);
        sugi_text_clear();
    }
    // cursor
    if (term_cursor_active)
    {
        sugi_gfx_rect((cursor_pos + 2) * term_char_x,     (term_line_y + 1) * term_char_y + term_offset_y - term_char_y, 
                      (cursor_pos + 3) * term_char_x - 1, (term_line_y + 1) * term_char_y + term_offset_y - 2, 1, 8);
    }
    // Displaying on a screen
    sugi_gfx_flip();
}

