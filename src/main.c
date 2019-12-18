#include "sugi.h"
#include <stdio.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

lua_State *L;


#pragma region GAME
void test_draw();
void test_init();
void test_update();

static int x, w, y, h, xs, ys;
static int px, py, pw, ph;
double t = 0;


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

  t += 1.0 / 60.0;
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
  
  for (int i = -256; i < 320; i+=8)
  {
    sugi_gfx_line(i, 0, i+256, 256, 0);
  }

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

  sugi_gfx_sspr(0, 16, 8, 8,
                16, 24, 16 + cos(t * 4) * 8, 16 + sin(t * 4) * 8);

  luaL_dostring(L, "lua_print('Hello from Lua!')");

  // sugi_gfx_print("test\n\r\thello", 0, 0, 7);
}
#pragma endregion GAME


static int lua_print(lua_State *L)
{
  int n = lua_gettop(L);
  
  // for (int i = 1; i <= n; i++)
  // {
    
  // }
  
  // char **str;

  if (!lua_isstring(L, 1)){
    lua_pushstring(L, "First argument must be a string!");
    lua_error(L);
  }

  // lua_pushinteger()
  sugi_gfx_print(lua_tostring(L, 1), 0, 0, 7);

  return 1;
}


int main(int argc, char *argv[])
{
  L = luaL_newstate();

  luaL_openlibs(L);
  luaopen_base(L);
  luaopen_table(L);
  luaopen_io(L);
  luaopen_string(L);
  luaopen_math(L);

  lua_register(L, "lua_print", lua_print);

  sugi_core_set_init(test_init);
  sugi_core_set_draw(test_draw);
  sugi_core_set_update(test_update);
  sugi_core_run();

  lua_close(L);

  return 0;
}
