#include "sugi.h"
#include <string.h>


void sugi_mem_memcpy(void *dst, void *src, uint32_t size)
{
    memcpy(dst, src, size);
}


void sugi_mem_memcpy_addr(uint32_t dst_addr, uint32_t src_addr, uint32_t size)
{
    memcpy(sugi_memory + dst_addr, sugi_memory + src_addr, size);
}


void sugi_mem_screen_to_backbuff()
{
    memcpy(sugi_memory_screen_backbuffer, sugi_memory, sizeof(sugi_memory_screen_backbuffer));// SUGI_MEM_SCREEN_SIZE);
}


void sugi_mem_backbuff_to_screen()
{
    memcpy(sugi_memory, sugi_memory_screen_backbuffer, sizeof(sugi_memory_screen_backbuffer));
}


void sugi_mem_poke_addr(uint32_t addr, uint8_t value)
{
    *(sugi_memory + addr) = value;
}


uint8_t sugi_mem_peek_addr(uint32_t addr)
{
    return *(sugi_memory + addr);
}

