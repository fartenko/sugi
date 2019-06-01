#include "sugi.h"
#include <stdio.h>


static int x, w, y, h, xs, ys;

void test_init(void) {
  x  = 28; 
  w  = 8;
  y  = 28;
  h  = 8;
  xs = 1;
  ys = 1;
}


void test_update(void) {
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


void test_draw() {
  sugi_gfx_pal_reset();
  sugi_gfx_clear(0);


  for (int i = 0; i < 8; i++)
  {
    sugi_gfx_rect(8 * i, 0, 8 * (i + 1), 7, 1, i);
  }

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
  sugi_gfx_circ(64, 64,  8,    1, 6);

  /*
  if (xs > 0)
  {
    for (uint8_t i = 0; i < 16; i++)
      sugi_gfx_pal(i, i - 8, 1);
  }
  */
}


int main (int argc, char *argv[])
{

  sugi_core_init();
  sugi_core_mem_init();
  sugi_set_init(test_init);
  sugi_set_draw(test_draw);
  sugi_set_update(test_update);
  sugi_core_run();

  return 0;
}



