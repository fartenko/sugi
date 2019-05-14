#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "sugi.h"
#include "bool.h"


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
  // TODO: write data to memory
  sugi_gfx_camera_x = x;
  sugi_gfx_camera_y = y;
}


void sugi_gfx_clear(uint8_t c)
{
  c %= 16;
  memset(sugi_memory_ptr, c | c << 4, sugi_memory_screen_size);
  // memset(__sugi_draw_buffer, c, sugi_render_width * sugi_render_height); 
}


void sugi_gfx_hline(int32_t x1, int32_t x2, int32_t y, uint8_t c_in)
{
  int32_t _x1 = (x1 < x2) ? x1 : x2;
  int32_t _x2 = (x1 < x2) ? x2 : x1;
  for(int32_t x = _x1; x <= _x2; x++)
  {
    sugi_gfx_pset(x, y, c_in);
  }
}

void sugi_gfx_vline(int32_t x, int32_t y1, int32_t y2, uint8_t c_in)
{
  int32_t _y1 = (y1 < y2) ? y1 : y2;
  int32_t _y2 = (y1 < y2) ? y2 : y1;
  for(int32_t y = _y1; y <= _y2; y++)
  {
    sugi_gfx_pset(x, y, c_in);
  }
}


void sugi_gfx_line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint8_t c_in)
{
  if (x1 == x2)
  {
    sugi_gfx_vline(x1, y1, y2, c_in);
    return;
  }

  if (y1 == y2)
  {
    sugi_gfx_hline(x1, x2, y1, c_in);
    return;
  }

  int32_t dx   = abs(x2 - x1);
  int32_t sx   = (x1 < x2) ? 1 : -1;
  int32_t dy   = abs(y2 - y1);
  int32_t sy   = (y1 < y2) ? 1 : -1;
  int32_t err  = ((dx > dy) ? dx : dy) / 2; 
  int32_t err2 = 0;

  while (true)
  {
    sugi_gfx_pset(x1, y1, c_in);

    if (x1 == x2 && y1 == y2)
      break;

    err2 = err;

    if (err2 > -dx)
    {
      err -= dy;
      x1 += sx;
    }
    if (err2 < dy)
    {
      err += dx;
      y1 += sy;
    }
  }
}


void sugi_gfx_rect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int8_t fill, uint8_t c_in)
{
  if (fill == 1)
  {
    int32_t _y1 = (y1 < y2) ? y1 : y2;
    int32_t _y2 = (y1 < y2) ? y2 : y1;
    
    for (int32_t y = _y1; y <= _y2; y++)
    {
      sugi_gfx_line(x1, y, x2, y, c_in);
    }
  }
  else
  {
    sugi_gfx_line(x1, y1, x2, y1, c_in);
    sugi_gfx_line(x1, y2, x2, y2, c_in);
    sugi_gfx_line(x1, y1, x1, y2, c_in);
    sugi_gfx_line(x2, y1, x2, y2, c_in);
  }
}

#define PI 3.14159265
float sugi_gfx_circ_turnatan2_internal(float y, float x)
{
  float rad = atan2f(y, x); 
  float ang = rad * (180.0 / PI);
  float res = (ang < 0) ? 360.0 + ang : ang;
  return res / 360.0;
}


void sugi_gfx_circ_segment_internal(int32_t x0, int32_t y0, int32_t x, int32_t y, int8_t fill, uint8_t c_in)
{
  if (fill == 1)
  {
    sugi_gfx_line(x0 - y, y0 + x, x0 + y, y0 + x, c_in);
    sugi_gfx_line(x0 - x, y0 + y, x0 + x, y0 + y, c_in);
    sugi_gfx_line(x0 - x, y0 - y, x0 + x, y0 - y, c_in);
    sugi_gfx_line(x0 - y, y0 - x, x0 + y, y0 - x, c_in);
  }
  else 
  {
    sugi_gfx_pset(x0 + x, y0 + y, c_in);
    sugi_gfx_pset(x0 - x, y0 + y, c_in);
    sugi_gfx_pset(x0 + x, y0 - y, c_in);
    sugi_gfx_pset(x0 - x, y0 - y, c_in);
    sugi_gfx_pset(x0 + y, y0 + x, c_in);
    sugi_gfx_pset(x0 - y, y0 + x, c_in);
    sugi_gfx_pset(x0 + y, y0 - x, c_in);
    sugi_gfx_pset(x0 - y, y0 - x, c_in);
  }
}


void sugi_gfx_circ(int32_t xc, int32_t yc, int32_t r, int8_t fill, uint8_t c_in)
{
  int32_t x = 0;
  int32_t y = r;
  int32_t d = 3 - 2 * r;

  while (y >= x)
  {
    sugi_gfx_circ_segment_internal(xc, yc, x, y, fill, c_in);
    x += 1;

    if (d > 0)
    {
      y -= 1;
      d += 4 * (x - y) + 2;
    }
    else 
    {
      d += 4 * x + 6;
    }
  }
}

