#include <string.h>
#include "sugi.h"


static int32_t sugi_gfx_camera_x = 0;
static int32_t sugi_gfx_camera_y = 0;


int8_t sugi_gfx_pset(int32_t x, int32_t y, uint8_t c_in)
{
  x -= sugi_gfx_camera_x;
  y -= sugi_gfx_camera_y;

  if (x < 0 || x >= SUGI_RENDER_WIDTH ||
      y < 0 || y >= SUGI_RENDER_HEIGHT)
    return 0;
  
  uint8_t offset = (x % 2 == 0) ? 4 : 0;
  *(sugi_memory_ptr + x / 2 + y * SUGI_RENDER_WIDTH / 2) &= 0xf << (4 - offset);
  *(sugi_memory_ptr + x / 2 + y * SUGI_RENDER_WIDTH / 2) |= (c_in % 16) << offset; 
  // *(__sugi_draw_buffer + _x + _y * sugi_render_width) = c % 16; 
  return 1;
}


int8_t sugi_gfx_pget(int32_t x, int32_t y, uint8_t *c_out)
{
  x -= sugi_gfx_camera_x;
  y -= sugi_gfx_camera_y;

  if (x < 0 || x >= SUGI_RENDER_WIDTH ||
      y < 0 || y >= SUGI_RENDER_HEIGHT)
    return 0;

  uint8_t offset = (x % 2 == 0) ? 4 : 0;
  *c_out = (*(sugi_memory_ptr + x / 2 + y * SUGI_RENDER_WIDTH / 2) >> offset) & 0xf;
  return 1;
}





void sugi_gfx_camera(int32_t x, int32_t y)
{
  sugi_gfx_camera_x = x;
  sugi_gfx_camera_y = y;
}


void sugi_gfx_clear(uint8_t c)
{
  c %= 16;
  memset(sugi_memory_ptr, c | c << 4, sugi_memory_screen_size);
  // memset(__sugi_draw_buffer, c, sugi_render_width * sugi_render_height); 
}

