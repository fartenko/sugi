#include "sugi.h"
#include <stdio.h>


static int x, w, y, h, xs, ys;


void test_init(void) {
  x  = 28;// 76;
  w  = 8;
  y  = 28;// 60;
  h  = 8;
  xs = 1;
  ys = 1;

  printf("Sugi Init!\n\r");
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
  sugi_gfx_clear(9);

  for (int i = 0; i < 192; i+=2)
  {
    sugi_gfx_pset(i, 0,   7);
    sugi_gfx_pset(i, 63,  7);
    sugi_gfx_pset(i, 127, 7);
    sugi_gfx_pset(i, 159, 7);
  }

  for (int _x = x; _x < x + w; _x++)
    for (int _y = y; _y < y + h; _y++)
      sugi_gfx_pset(_x + 2, _y + 2, 15);
  for (int _x = x; _x < x + w; _x++)
    for (int _y = y; _y < y + h; _y++)
      sugi_gfx_pset(_x, _y, 7);
}


int main (int argc, char *argv[])
{

  sugi_core_init();
  sugi_set_init(test_init);
  sugi_set_draw(test_draw);
  sugi_set_update(test_update);
  sugi_core_run();

  return 0;
}



