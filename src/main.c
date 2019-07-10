#include "sugi.h"
#include <stdio.h>


#pragma region GAME
static int x, w, y, h, xs, ys;
static int px, py, pw, ph;


void test_init(void)
{
  x  = 28;
  w  = 16;
  y  = 28;
  h  = 8;
  xs = 1;
  ys = 1;
  // player variables
  px = 32;
  py = 32;
  pw = 8;
  ph = 8;
}


void test_update(void)
{
  x += xs;
  y += ys;

  if (x + w > 64)
  {
    x = 64 - w;
    xs *= -1;
  }
  if (y + h > 64)
  {
    y = 64 - h;
    ys *= -1;
  }

  if (x < 0)
  {
    x = 0;
    xs *= -1;
  }
  if (y < 0)
  {
    y = 0;
    ys *= -1;
  }
}


void test_draw()
{
  sugi_gfx_pal_reset();
  sugi_gfx_palt_reset();
  sugi_gfx_clear(0);

  for (int i = 0; i < 8; i++)
    sugi_gfx_rect(8 * i, 0, 8 * (i + 1), 7, 1, i);

  for (int i = 0; i < 192; i+=2)
  {
    sugi_gfx_pset(i,   0, 7);
    sugi_gfx_pset(i,  63, 7);
    sugi_gfx_pset(i, 127, 7);
  }

  sugi_gfx_clip(16, 16, 64, 64);
  sugi_gfx_rect(x + 1, y + 1, x + w + 1, y + h + 1, 1, 7);
  sugi_gfx_clip_reset();

  sugi_gfx_line(64, 64, 128,  92, 7);
  sugi_gfx_rect(32, 16,  92,  48, 0, 1);
  sugi_gfx_rect(40, 24,  84,  40, 1, 2);
  sugi_gfx_circ(64, 64,   8,   1, 6);

  for (int i = 0; i < 8; i++)
    sugi_gfx_sset(i, i, 7);

  sugi_gfx_sset(0,  0, 1);
  sugi_gfx_sset(8,  0, 5);
  sugi_gfx_sset(16, 0, 4);
  sugi_gfx_sset(24, 0, 6);

  sugi_gfx_sset(2, 18, 7);
  sugi_gfx_sset(5, 18, 7);
  sugi_gfx_sset(2, 22, 7);
  sugi_gfx_sset(3, 22, 7);
  sugi_gfx_sset(4, 22, 7);
  sugi_gfx_sset(5, 22, 7);
  sugi_gfx_sset(1, 21, 7);
  sugi_gfx_sset(6, 21, 7);

  sugi_gfx_sset(2 + 8, 18, 9);
  sugi_gfx_sset(5 + 8, 18, 9);
  sugi_gfx_sset(2 + 8, 22, 9);
  sugi_gfx_sset(3 + 8, 22, 9);
  sugi_gfx_sset(4 + 8, 22, 9);
  sugi_gfx_sset(5 + 8, 22, 9);
  sugi_gfx_sset(1 + 8, 21, 9);
  sugi_gfx_sset(6 + 8, 21, 9);

  uint8_t c00;
  sugi_gfx_sget(0, 0, &c00);

  sugi_gfx_clear(c00);
  for (int s = 0; s < 256; s++)
  {
    if (s == 32)
    {
      sugi_gfx_palt(7,0);
      sugi_gfx_palt(0,1);
    }
    if (s == 33)
    {
      sugi_gfx_palt(9,0);
      sugi_gfx_palt(0,1);
      sugi_gfx_pal_no_mode(0,2);
    }

    sugi_gfx_spr(s, 16 + (s % 16) * 8, (s / 16) * 8);
    sugi_gfx_palt_reset();
    sugi_gfx_pal_reset();
  }
}
#pragma endregion GAME


int main (int argc, char *argv[])
{
  sugi_core_set_init(test_init);
  sugi_core_set_draw(test_draw);
  sugi_core_set_update(test_update);
  sugi_core_run();

  return 0;
}
