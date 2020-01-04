#include "sugi.h"




void call_lua_tick();
void call_lua_init();

void test_draw();

float t = 0;
float fps = 0.0;

void call_lua_init(void)
{
  //luaL_dofile(L, "lua/main.lua");
  //lua_getglobal(L, "__init__");
  //lua_call(L, 0, 0);
  //lua_pop(L, 0);
}


void call_lua_tick(void)
{
  //lua_getglobal(L, "__tick__");
  //lua_call(L, 0, 0);
  //lua_pop(L, 0);
}


// Test draw function
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

  //sugi_gfx_clip(16, 16, 64, 64);
  //sugi_gfx_rect(x + 1, y + 1, x + w + 1, y + h + 1, 1, 7);
  //sugi_gfx_clip_reset();

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
  luaL_dofile(L, "lua/print.lua");

  sugi_gfx_print("test\n\r\thello", 0, 0, 7);


  for (int i = 0; i < 8; i++)
  {
    if (sugi_input_btnp(i, 0))
      sugi_gfx_circ(16 + i * 8, 64, 4, 1, 7);
    else 
      sugi_gfx_circ(16 + i * 8, 64, 4, 0, 7);

    if (sugi_input_btn(i, 0))
      sugi_gfx_circ(16 + i * 8, 72, 4, 1, 7);
    else 
      sugi_gfx_circ(16 + i * 8, 72, 4, 0, 7);
  }
  for (int i = 0; i < 8; i++)
  {
    if (sugi_input_btnp(i, 1))
      sugi_gfx_circ(64 + 16 + i * 8, 64, 4, 1, 6);
    else 
      sugi_gfx_circ(64 + 16 + i * 8, 64, 4, 0, 6);

    if (sugi_input_btn(i, 1))
      sugi_gfx_circ(64 + 16 + i * 8, 72, 4, 1, 6);
    else 
      sugi_gfx_circ(64 + 16 + i * 8, 72, 4, 0, 6);
  }

  t+=1.0/60.0;
}



static int lua_print(lua_State *L)
{
  // define default values
  char * s = "";
  int32_t x = 0;
  int32_t y = 0;
  uint8_t c = sugi_gfx_getcolor(); 

  // process arguments
  if (lua_isstring(L, 1)) {
    s = lua_tostring(L, 1);
  }
  else
  {
    if (lua_isnil(L, 1))
      s = "nil";
    if (lua_isboolean(L, 1))
      s = lua_toboolean(L, 1) ? "true" : "false";
    if (lua_istable(L, 1))
      s = "table";
  }

  if (lua_isnumber(L, 2))
    x = lua_tointeger(L, 2);
  if (lua_isnumber(L, 3))
    y = lua_tointeger(L, 3);
  if (lua_isnumber(L, 4))
    c = lua_tointeger(L, 4);
  
  // call sugi print function
  sugi_gfx_print(s, x, y, c);
  return 1;
}


static int lua_cls(lua_State *L)
{
  // process arguments
  if (!lua_isnumber(L, 1))
    sugi_gfx_clear_no_col();

  // call sugi clear function
  sugi_gfx_clear(lua_tointeger(L, 1));
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

  lua_register(L, "print", lua_print);
  lua_register(L, "cls", lua_cls);

  sugi_core_set_init(call_lua_init);
  sugi_core_set_update(call_lua_tick);
  sugi_core_set_draw(test_draw);
  sugi_core_run();

  lua_close(L);

  return 0;
}
