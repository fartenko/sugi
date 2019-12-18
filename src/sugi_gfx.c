#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "sugi.h"
#include "sugi_font.h"


// Sets a current color to draw pixels with
void sugi_gfx_setcolor(uint8_t c_in)
{
  *(sugi_memory_ptr + SUGI_MEM_COLOR_PTR) = c_in;
}


// Gets a current color
uint8_t sugi_gfx_getcolor()
{
  return *(sugi_memory_ptr + SUGI_MEM_COLOR_PTR);
}


// Puts a pixel on a screen
int8_t sugi_gfx_pset(int32_t x, int32_t y, uint8_t c_in)
{
  // clipping color value between 0 and 15
  // and setting a current color
  c_in %= 16;
  sugi_gfx_setcolor(c_in);

  // getting a 32 bit camera position value from
  // 4 8bit values from a virtual RAM
  int32_t sugi_gfx_camera_x = *(sugi_memory_ptr + SUGI_MEM_CAMERA_X_PTR + 0)       |
                              *(sugi_memory_ptr + SUGI_MEM_CAMERA_X_PTR + 1) << 8  |
                              *(sugi_memory_ptr + SUGI_MEM_CAMERA_X_PTR + 2) << 16 |
                              *(sugi_memory_ptr + SUGI_MEM_CAMERA_X_PTR + 3) << 24;
  int32_t sugi_gfx_camera_y = *(sugi_memory_ptr + SUGI_MEM_CAMERA_Y_PTR + 0)       |
                              *(sugi_memory_ptr + SUGI_MEM_CAMERA_Y_PTR + 1) << 8  |
                              *(sugi_memory_ptr + SUGI_MEM_CAMERA_Y_PTR + 2) << 16 |
                              *(sugi_memory_ptr + SUGI_MEM_CAMERA_Y_PTR + 3) << 24;

  // getting a screen-space clipping region
  uint8_t sugi_gfx_clip_x1 = *(sugi_memory_ptr + SUGI_MEM_CLIP_PTR + 0);
  uint8_t sugi_gfx_clip_y1 = *(sugi_memory_ptr + SUGI_MEM_CLIP_PTR + 1);
  uint8_t sugi_gfx_clip_x2 = *(sugi_memory_ptr + SUGI_MEM_CLIP_PTR + 2);
  uint8_t sugi_gfx_clip_y2 = *(sugi_memory_ptr + SUGI_MEM_CLIP_PTR + 3);

  // converting a pixel world-space position to a screen-space position
  x -= sugi_gfx_camera_x;
  y -= sugi_gfx_camera_y;

  //if (x < 0 || x >= SUGI_RENDER_WIDTH ||
  //    y < 0 || y >= SUGI_RENDER_HEIGHT)
  if (x < sugi_gfx_clip_x1 || x >= sugi_gfx_clip_x2 || // if we're out of bound of
      y < sugi_gfx_clip_y1 || y >= sugi_gfx_clip_y2)   //   a clip area.
    return 0;

  // because we're dealing with 4 bit colors, we store two
  // colors in one 8 bit value
  uint8_t offset = (x % 2 == 0) ? 4 : 0;                     // byte offset
  c_in = *(sugi_memory_ptr + SUGI_MEM_PAL_DRAW_PTR + c_in);  // applying palette
  *(sugi_memory_ptr + x / 2 + y * SUGI_RENDER_WIDTH / 2) &= 0x0F << (4 - offset);
  *(sugi_memory_ptr + x / 2 + y * SUGI_RENDER_WIDTH / 2) |= c_in << offset;
  // *(__sugi_draw_buffer + _x + _y * sugi_render_width) = c;
  return 1;
}


// Puts a pixels but needs no color argument, instead
// it get a color stored in a virtual RAM
int8_t sugi_gfx_pset_no_col(int32_t x, int32_t y)
{
  uint8_t c_in = sugi_gfx_getcolor();
  sugi_gfx_pset(x, y, c_in);
}


// Gets a pixel color of a pixel
int8_t sugi_gfx_pget(int32_t x, int32_t y, uint8_t *c_out)
{
  // Getting a camera position
  int32_t sugi_gfx_camera_x = *(sugi_memory_ptr + SUGI_MEM_CAMERA_X_PTR + 0)       |
                              *(sugi_memory_ptr + SUGI_MEM_CAMERA_X_PTR + 1) << 8  |
                              *(sugi_memory_ptr + SUGI_MEM_CAMERA_X_PTR + 2) << 16 |
                              *(sugi_memory_ptr + SUGI_MEM_CAMERA_X_PTR + 3) << 24;
  int32_t sugi_gfx_camera_y = *(sugi_memory_ptr + SUGI_MEM_CAMERA_Y_PTR + 0)       |
                              *(sugi_memory_ptr + SUGI_MEM_CAMERA_Y_PTR + 1) << 8  |
                              *(sugi_memory_ptr + SUGI_MEM_CAMERA_Y_PTR + 2) << 16 |
                              *(sugi_memory_ptr + SUGI_MEM_CAMERA_Y_PTR + 3) << 24;

  // Converting position to screen-space position
  x -= sugi_gfx_camera_x;
  y -= sugi_gfx_camera_y;

  if (x < 0 || x >= SUGI_RENDER_WIDTH ||
      y < 0 || y >= SUGI_RENDER_HEIGHT)
    return 0;

  uint8_t offset = (x % 2 == 0) ? 4 : 0;
  *c_out = (*(sugi_memory_ptr + x / 2 + y * SUGI_RENDER_WIDTH / 2) >> offset) & 0xF;
  return 1;
}


// Sets a camera position,
// What it really does, is it just offsets a position, where pixels would be drawn
void sugi_gfx_camera(int32_t x, int32_t y)
{
  // sugi_gfx_camera_x = x;
  // sugi_gfx_camera_y = y;

  *(sugi_memory_ptr + SUGI_MEM_CAMERA_X_PTR + 0) =         x & 0xFF;
  *(sugi_memory_ptr + SUGI_MEM_CAMERA_X_PTR + 1) =  (x >> 8) & 0xFF;
  *(sugi_memory_ptr + SUGI_MEM_CAMERA_X_PTR + 2) = (x >> 16) & 0xFF;
  *(sugi_memory_ptr + SUGI_MEM_CAMERA_X_PTR + 3) = (x >> 24) & 0xFF;

  *(sugi_memory_ptr + SUGI_MEM_CAMERA_Y_PTR + 0) =         y & 0xFF;
  *(sugi_memory_ptr + SUGI_MEM_CAMERA_Y_PTR + 1) =  (y >> 8) & 0xFF;
  *(sugi_memory_ptr + SUGI_MEM_CAMERA_Y_PTR + 2) = (y >> 16) & 0xFF;
  *(sugi_memory_ptr + SUGI_MEM_CAMERA_Y_PTR + 3) = (y >> 24) & 0xFF;

}


// Clears a screen
void sugi_gfx_clear(uint8_t c)
{
  c %= 16;
  sugi_gfx_setcolor(c);
  memset(sugi_memory_ptr, c | c << 4, sugi_memory_screen_size);
  // memset(__sugi_draw_buffer, c, sugi_render_width * sugi_render_height);
}


// Clears a screen without a color argumeent needed
void sugi_gfx_clear_no_col()
{
  uint8_t c = sugi_gfx_getcolor();
  sugi_gfx_clear(c);
}


// Draws a horizontal line
void sugi_gfx_hline(int32_t x1, int32_t x2, int32_t y, uint8_t c_in)
{
  int32_t _x1 = (x1 < x2) ? x1 : x2;
  int32_t _x2 = (x1 < x2) ? x2 : x1;
  for(int32_t x = _x1; x <= _x2; x++)
  {
    sugi_gfx_pset(x, y, c_in);
  }
}


// Draws a vertical line
void sugi_gfx_vline(int32_t x, int32_t y1, int32_t y2, uint8_t c_in)
{
  int32_t _y1 = (y1 < y2) ? y1 : y2;
  int32_t _y2 = (y1 < y2) ? y2 : y1;
  for(int32_t y = _y1; y <= _y2; y++)
  {
    sugi_gfx_pset(x, y, c_in);
  }
}


// Draws a line
void sugi_gfx_line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint8_t c_in)
{
  // If both x coordinates are equal, we'll just draw a horizontal line
  if (x1 == x2)
  {
    sugi_gfx_vline(x1, y1, y2, c_in);
    return;
  }
  // If both y coordinates are equal, we'll just draw a vertical line
  if (y1 == y2)
  {
    sugi_gfx_hline(x1, x2, y1, c_in);
    return;
  }

  // Bresenham's line drawing algorithm
  int32_t dx   = abs(x2 - x1);
  int32_t sx   = (x1 < x2) ? 1 : -1;
  int32_t dy   = abs(y2 - y1);
  int32_t sy   = (y1 < y2) ? 1 : -1;
  int32_t err  = ((dx > dy) ? dx : dy) / 2;

  while (1)
  {
    sugi_gfx_pset(x1, y1, c_in);

    if (x1 == x2 && y1 == y2)
      break;

    int32_t err_tmp = err;

    if (err_tmp > -dx)
    {
      err -= dy;
      x1 += sx;
    }
    if (err_tmp < dy)
    {
      err += dx;
      y1 += sy;
    }
  }
}


// Draws a line but requires no color argument
void sugi_gfx_line_no_col(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
  uint8_t c_in = sugi_gfx_getcolor();
  sugi_gfx_line(x1, y1, x2, y2, c_in);
}


// Draws a rectangle
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


// Draws a rectabgle but requires no color argument
void sugi_gfx_rect_no_col(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int8_t fill)
{
  uint8_t c_in = sugi_gfx_getcolor();
  sugi_gfx_rect(x1, y1, x2, y2, fill, c_in);
}


// Draws a circle segment
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


// Draws a circle using a Bresenham's Circle drawing algorithm
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


// Draws a circle but requires no circle argument
void sugi_gfx_circ_no_col(int32_t xc, int32_t yc, int32_t r, int8_t fill)
{
  uint8_t c_in = sugi_gfx_getcolor();
  sugi_gfx_circ(xc, yc, r, fill, c_in);
}


// Internal clip function that sets screen-space draw boundaries or clipping region
void sugi_gfx_clip_internal(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
  if (x2 < x1)
  {
    uint8_t tmp = x1;
    x2 = x1;
    x1 = tmp;
  }

  if (y2 < y1)
  {
    uint8_t tmp = y1;
    y2 = y1;
    y1 = tmp;
  }

  // Writes data to a virtual RAM
  *(sugi_memory_ptr + SUGI_MEM_CLIP_PTR + 0) = x1;
  *(sugi_memory_ptr + SUGI_MEM_CLIP_PTR + 1) = y1;
  *(sugi_memory_ptr + SUGI_MEM_CLIP_PTR + 2) = x2;
  *(sugi_memory_ptr + SUGI_MEM_CLIP_PTR + 3) = y2;
}


// Resers a clip region to a screen size
void sugi_gfx_clip_reset()
{
  sugi_gfx_clip(0, 0, SUGI_RENDER_WIDTH, SUGI_RENDER_HEIGHT);
}


// Sets a screen-space draw bounaries or clipping region
void sugi_gfx_clip(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
  x1 = (x1 < 0) ? 0 : (x1 > SUGI_RENDER_WIDTH)  ? SUGI_RENDER_WIDTH  : x1;
  y1 = (y1 < 0) ? 0 : (y1 > SUGI_RENDER_HEIGHT) ? SUGI_RENDER_HEIGHT : y1;
  x2 = (x2 < 0) ? 0 : (x2 > SUGI_RENDER_WIDTH)  ? SUGI_RENDER_WIDTH  : x2;
  y2 = (y2 < 0) ? 0 : (y1 > SUGI_RENDER_HEIGHT) ? SUGI_RENDER_HEIGHT : y2;
  sugi_gfx_clip_internal((uint8_t)x1, (uint8_t)y1, (uint8_t)x2, (uint8_t)y2);
}


// Resets a palette swap values
void sugi_gfx_pal_reset()
{
  for (uint8_t i = 0; i < 16; i++)
  {
    *(sugi_memory_ptr + SUGI_MEM_PAL_DRAW_PTR   + i) = i;
    *(sugi_memory_ptr + SUGI_MEM_PAL_SCREEN_PTR + i) = i;
  }
}


// Replaces color c1 with color c2
void sugi_gfx_pal(uint8_t c1, uint8_t c2, uint8_t mode)
{
  // replaces c1 with c2
  // mode 0 - draw palette (applied at draw functions)
  // mode 1 - screen palette (applied at screen render)
  c1 %= 16;
  c2 %= 16;
  *(sugi_memory_ptr + ((mode == 0) ? SUGI_MEM_PAL_DRAW_PTR : SUGI_MEM_PAL_SCREEN_PTR) + c1) = c2;
}


// Replaces color c1 with color c2 but requires no mode argument
// mode is 0 (draw palette) by default
void sugi_gfx_pal_no_mode(uint8_t c1, uint8_t c2)
{
  sugi_gfx_pal(c1, c2, 0);
}


// PALT -> 0 - transparetn
// PALT -> 1 - opaque
void sugi_gfx_palt_reset()
{
  *(sugi_memory_ptr + SUGI_MEM_PALT_PTR + 0) = 0x00;
  *(sugi_memory_ptr + SUGI_MEM_PALT_PTR + 1) = 0x00;
  *(sugi_memory_ptr + SUGI_MEM_PALT_SET_PTR) = 0x00;
}


// palt affects sprite drawing only
void sugi_gfx_palt(uint8_t c, uint8_t t)
{
  // clipping c between 0 and 15
  // a memory offset, 1 or 0 to select a 8bit part of a 16bit value
  c %= 16;
  uint8_t mem_off = c / 8;
  c -= mem_off * 8;

  // getting a current value of palt register
  // creating a XOR mask to toggle one of the bits in 8bit part
  // e.g. first bit is 0 and we want to set it to 1,
  // .. then t=1, and 0^t=1
  // .. if we want to set from 0 to 1,
  // .. then t=0, and 1^~t=0
  // TODO: add palt logic to spr()
  if (c == 0)
  {
    // t = ~t & 0x1;
    *(sugi_memory_ptr + SUGI_MEM_PALT_SET_PTR) = t;
  }

  uint8_t val = *(sugi_memory_ptr + SUGI_MEM_PALT_PTR + mem_off);
  t = ((((val >> c) & 0x1 == 0) ? t : ~t) & 0x1) << c;
  val ^= t;
  *(sugi_memory_ptr + SUGI_MEM_PALT_PTR + mem_off) = val;
}



void sugi_gfx_spr_pset_internal(int32_t x, int32_t y, uint8_t c_in)
{
  uint8_t c_pal = *(sugi_memory_ptr + SUGI_MEM_PAL_DRAW_PTR + c_in);
  if (c_pal == 0 && *(sugi_memory_ptr + SUGI_MEM_PALT_SET_PTR) == 0)
    return;

  uint8_t palt_mem_off = c_pal / 8;
  if ((*(sugi_memory_ptr + SUGI_MEM_PALT_PTR + palt_mem_off) >> (c_pal - palt_mem_off * 8)) & 0x01 == 1)
     return;

  sugi_gfx_pset(x, y, c_in);
}


void sugi_gfx_spr(uint8_t s, int32_t x, int32_t y)
{
  // if (s < 0 || s >= 256)
  //   return;

  // getting a color, to set it back to original
  uint8_t _c  = sugi_gfx_getcolor();
  uint8_t sox = (s % 16) * 4;
  uint8_t soy = (s / 16) * 8;

  for (uint8_t sx = 0; sx < 8; sx += 2)
  {
    for (uint8_t sy = 0; sy < 8; sy++)
    {
      uint32_t off = sox + sx / 2 + soy * 64 + sy * 64;
      uint8_t sc1 = *(sugi_memory_ptr + SUGI_MEM_SPRSHEET_PTR + off) >> 4 & 0x0F;
      uint8_t sc2 = *(sugi_memory_ptr + SUGI_MEM_SPRSHEET_PTR + off) & 0x0F;
      sugi_gfx_spr_pset_internal(x + sx,     y + sy, sc1);
      sugi_gfx_spr_pset_internal(x + sx + 1, y + sy, sc2);
    }
  }

  // setting a color back
  sugi_gfx_setcolor(_c);
}


void sugi_gfx_sset(int32_t x, int32_t y, uint8_t c_in)
{
  c_in %= 16;
  // 128x128 spritesheet
  if (x < 0 || x >= 128 || y < 0 || y >= 128)
    return;

  uint8_t offset = (x % 2 == 0) ? 4 : 0;
  c_in = *(sugi_memory_ptr + SUGI_MEM_PAL_DRAW_PTR + c_in);
  *(sugi_memory_ptr + SUGI_MEM_SPRSHEET_PTR + x / 2 + y * 64) &= 0x0F << (4 - offset);
  *(sugi_memory_ptr + SUGI_MEM_SPRSHEET_PTR + x / 2 + y * 64) |= c_in << offset;
}


int8_t sugi_gfx_sget(int32_t x, int32_t y, uint8_t *c_out)
{
  if (x < 0 || x >= 128 || y < 0 || y >= 128)
  {
    *c_out = 0;
    return 0;
  }

  uint8_t offset = (x % 2 == 0) ? 4 : 0;
  *c_out = (*(sugi_memory_ptr + SUGI_MEM_SPRSHEET_PTR + x / 2 + y * 64) >> offset) & 0xF;
  return 1;
}


// sspr
// get all the pixels in a selected box
// then `stretch` or `squish` them depending on the box it is being drawn to

void sugi_gfx_sspr(int32_t sx, int32_t sy, int32_t sw, int32_t sh, int32_t x, int32_t y, int32_t w, int32_t h)
{
  // pixels = memalloc(); ??? not needed???
  // for x and y ...
  // ... get needed sx and sy pixel
  // ... then put the pixel
  
  for (int32_t _x = 0; _x < w; _x++)
  {
    for (int32_t _y = 0; _y < h; _y++)
    {
      uint8_t c = 0;
      int32_t _sx = floorf(_x * ((float)sw / (float)w));
      int32_t _sy = floorf(_y * ((float)sh / (float)h));
      sugi_gfx_sget(sx + _sx, sy + _sy, &c);
      // sugi_gfx_sset(x + _x, y + _y, c);
      if (c == 0) c = 3;
      sugi_gfx_spr_pset_internal(x + _x, y + _y, c);
    }
  }
}
 

void sugi_gfx_print(char *str, int32_t x, int32_t y, uint8_t c)
{
  sugi_gfx_setcolor(c);
  int32_t ox = x, oy = y;
  int32_t slen = strlen(str);
  int32_t orig_ox = ox;

  printf("-----\n\r");
  for (int i = 0; i < slen; i++)
  {
    int ascii = (int)str[i];
    
    // Tab case
    if (ascii == 9){
      ox += (SUGI_FONT_WIDTH + SUGI_FONT_LETER_SPACE) * SUGI_FONT_TAB_SIZE;
      continue;
    }
    // NL case
    if (ascii == 10){
      oy += SUGI_FONT_HEIGHT;
      continue;
    }
    // CR case
    if (ascii == 13){
      ox = orig_ox;
      continue;
    }

    ascii += SUGI_FONT_ASCII_OFFSET;

    for (int y = 0; y < SUGI_FONT_HEIGHT; y++)
    {
      char c = SUGI_FONT[ascii][y];
      
      for (int x = 0; x < SUGI_FONT_WIDTH; x++)
      {
        if (c & 0x01 != 0)
          // sugi_gfx_pset_no_col(ox - x + SUGI_FONT_WIDTH - 1, oy + y);
          sugi_gfx_pset_no_col(ox + y, oy + x + SUGI_FONT_HEIGHT_OFFSET);
        c = c >> 1;
      }
    }

    ox += SUGI_FONT_WIDTH + SUGI_FONT_LETER_SPACE;
  }
}


void sugi_gfx_flip() { sugi_renderer_draw_internal(); }


