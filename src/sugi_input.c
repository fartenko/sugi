#include "sugi.h"
#include <stdio.h>


uint8_t sugi_kb_codes[8] = {
    SUGI_KB_BTN_UP,
    SUGI_KB_BTN_DOWN,
    SUGI_KB_BTN_LEFT,
    SUGI_KB_BTN_RIGHT,
    SUGI_KB_BTN_X,
    SUGI_KB_BTN_Y,
    SUGI_KB_BTN_A,
    SUGI_KB_BTN_B,
};


void sugi_input_process_kb_press_state(const uint8_t * state)
{
    for (int i = 0; i < 8; i++)
    {
        if (state[sugi_kb_codes[i]])
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
        if (state[sugi_kb_codes[i]])
            *(sugi_memory + SUGI_MEM_BTN_PTR + 0) |= 0x1 << i;
        else 
            *(sugi_memory + SUGI_MEM_BTN_PTR + 0) &= 0xFF ^ (0x1 << i);
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