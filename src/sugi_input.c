#include "sugi.h"
#include <stdio.h>

uint8_t sugi_text_input_mode = 0;

uint8_t sugi_kb_map[8] = {
    SUGI_KB_BTN_UP,
    SUGI_KB_BTN_DOWN,
    SUGI_KB_BTN_LEFT,
    SUGI_KB_BTN_RIGHT,
    SUGI_KB_BTN_A,
    SUGI_KB_BTN_B,
    SUGI_KB_BTN_X,
    SUGI_KB_BTN_Y,
};

uint8_t sugi_controller_map[8] = {
    SUGI_CONTROLLER_UP,
    SUGI_CONTROLLER_DOWN,
    SUGI_CONTROLLER_LEFT,
    SUGI_CONTROLLER_RIGHT,
    SUGI_CONTROLLER_A,
    SUGI_CONTROLLER_B,
    SUGI_CONTROLLER_X,
    SUGI_CONTROLLER_Y,
};

void sugi_input_process_kb_press_state(const uint8_t * state)
{
    for (int i = 0; i < 8; i++)
    {
        if (state[sugi_kb_map[i]])
        {
            *(sugi_memory + SUGI_MEM_BTNP_PTR + 0) |= 0x1 << i;
            *(sugi_memory + SUGI_MEM_BTN_PTR + 0)  |= 0x1 << i;
        }
    }
}


void sugi_input_process_kb_release_state(const uint8_t * state)
{
    for (int i = 0; i < 8; i++)
    {
        if (state[sugi_kb_map[i]])
            *(sugi_memory + SUGI_MEM_BTN_PTR + 0) |= 0x1 << i;
        else 
            *(sugi_memory + SUGI_MEM_BTN_PTR + 0) &= 0xFF ^ (0x1 << i);
    }
}


void sugi_input_process_controller_press_button(uint8_t button, uint8_t player)
{
    for (int i = 0; i < 8; i++)
    {
        if (sugi_controller_map[i] == button)
        {
            *(sugi_memory + SUGI_MEM_BTNP_PTR + player) |= 0x1 << i;
            *(sugi_memory + SUGI_MEM_BTN_PTR + player)  |= 0x1 << i;
        }
    }
}


void sugi_input_process_controller_release_button(uint8_t button, uint8_t player)
{
    for (int i = 0; i < 8; i++)
    {
        if (sugi_controller_map[i] == button)
            *(sugi_memory + SUGI_MEM_BTN_PTR + player) &= 0xFF ^ (0x1 << i);
    }
}


void sugi_input_clear_btnp_internal() 
{
    for (int i = 0; i < 4; i++)
        *(sugi_memory + SUGI_MEM_BTNP_PTR + i) = 0;
}


uint8_t sugi_input_btn(uint8_t b, uint8_t p) 
{
    return (*(sugi_memory + SUGI_MEM_BTN_PTR + p) >> b) & 0x1;
}


uint8_t sugi_input_btnp(uint8_t b, uint8_t p)
{
    return (*(sugi_memory + SUGI_MEM_BTNP_PTR + p) >> b) & 0x1;
} 


void sugi_text_set_target(char *target)
{
    sugi_text_field_struct.text_ptr = target;
}


void sugi_text_get_target(char *out)
{
    out = sugi_text_field_struct.text_ptr;
}


void sugi_text_unset_target(void)
{
    sugi_text_field_struct.text_ptr = NULL;
    sugi_text_field_struct.active = SUGI_TEXTMODE_OFF;
}


void sugi_text_set_mode(uint8_t mode)
{
    sugi_text_field_struct.active = mode;
}

void sugi_text_move_cursor(int8_t dir)
{
    if (sugi_text_field_struct.active == SUGI_TEXTMODE_OFF)
        return;

    uint32_t cur_pos = sugi_text_field_struct.cursor_pos;
    if (dir < 0)
    {
        // left
        if (cur_pos > 0)
            sugi_text_field_struct.cursor_pos = cur_pos - 1; 
    }
    else
    {
        // right
        if (cur_pos < strlen(sugi_text_field_struct.text_ptr))
            sugi_text_field_struct.cursor_pos = cur_pos + 1;
    }
}


void sugi_text_insert_str(char *s)
{
    if (sugi_text_field_struct.active == SUGI_TEXTMODE_OFF)
        return;

    uint32_t c_pos = sugi_text_field_struct.cursor_pos;
    uint32_t t_len = strlen(sugi_text_field_struct.text_ptr);
    uint32_t input_len = strlen(s);
    char *ptr = sugi_text_field_struct.text_ptr;

    // If cursor at the end of the string, just concatenate
    if (c_pos == t_len)
    {
        strcat(ptr, s);        
    }
    else 
    {
        memcpy(ptr + c_pos + input_len, ptr + c_pos, t_len - c_pos);
        memcpy(ptr + c_pos, s, input_len);
        ptr[t_len + input_len + 1] = '\0';
    }

    sugi_text_field_struct.cursor_pos += input_len;
}


void sugi_text_backspace_char(void)
{
    if (sugi_text_field_struct.active == SUGI_TEXTMODE_OFF)
        return;

    // if cursor is at the begining, no point of backspacing..
    if (sugi_text_field_struct.cursor_pos == 0)
        return;

    uint32_t c_pos = sugi_text_field_struct.cursor_pos;
    uint32_t t_len = strlen(sugi_text_field_struct.text_ptr);
    char *ptr = sugi_text_field_struct.text_ptr;

    // if we are at the end of a string, just erase last char
    if (c_pos == t_len)
    {
        ptr[c_pos - 1] = '\0';
    }
    else
    {
        memcpy(ptr + c_pos - 1, ptr + c_pos, t_len - c_pos);
        ptr[t_len - 1] = '\0';
    }

    // move cursor back
    sugi_text_field_struct.cursor_pos--;
}



void sugi_text_delete_char(void)
{
    if (sugi_text_field_struct.active == SUGI_TEXTMODE_OFF)
        return;

    uint32_t c_pos = sugi_text_field_struct.cursor_pos;
    uint32_t t_len = strlen(sugi_text_field_struct.text_ptr);
    char *ptr = sugi_text_field_struct.text_ptr;

    if (c_pos != t_len)
    {
        memcpy(ptr + c_pos, ptr + c_pos + 1, t_len - c_pos);
        ptr[t_len - 1] = '\0';
    }
}


void sugi_text_clear(void)
{
    sugi_text_field_struct.text_ptr[0] = '\0';
    sugi_text_field_struct.cursor_pos = 0;
}
