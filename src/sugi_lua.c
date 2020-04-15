#include "sugi.h"


void sugi_lua_cmd_add_log(uint16_t code)
{
    if (sugi_lua_cmd_log.lua_cmd_count >= SUGI_LUA_CMD_LOG_SIZE)
    {
        // TODO: clear log and dump to file
        sugi_lua_cmd_log.lua_cmd_count = 0;
    }
    sugi_lua_cmd_log.lua_cmd[sugi_lua_cmd_log.lua_cmd_count] = code;
    sugi_lua_cmd_log.lua_cmd_count++;
}


uint16_t sugi_lua_cmd_get_last_log(void)
{
    return sugi_lua_cmd_log.lua_cmd[sugi_lua_cmd_log.lua_cmd_count - 1];    
}


void sugi_lua_register(void)
{
    lua_register(L, "cls",    sugi_lua_fn_gfx_clear);
    lua_register(L, "print",  sugi_lua_fn_gfx_print);
    lua_register(L, "flip",   sugi_lua_fn_gfx_flip);
    lua_register(L, "pset",   sugi_lua_fn_gfx_pset);
    lua_register(L, "pget",   sugi_lua_fn_gfx_pget);
    lua_register(L, "camera", sugi_lua_fn_gfx_camera);
    lua_register(L, "line",   sugi_lua_fn_gfx_line);
    lua_register(L, "rect",   sugi_lua_fn_gfx_rect);
    lua_register(L, "circ",   sugi_lua_fn_gfx_circ);
}


void sugi_call_lua_prog_init(char *filepath)
{
    luaL_dofile(L, filepath);
    lua_getglobal(L, "__init__");
    lua_call(L, 0, 0);
    lua_pop(L, 0);
}

void sugi_call_lua_prog_update(void)
{
    lua_getglobal(L, "__tick__");
    lua_call(L, 0, 0);
    lua_pop(L, 0);
}


static int sugi_lua_fn_gfx_clear(lua_State *L)
{
    sugi_lua_cmd_add_log(SUGI_LUA_LOG_CLEAR);
    // process arguments
    if (!lua_isnumber(L, 1))
    {
        // sugi_gfx_clear_no_col();
        sugi_gfx_clear(0);
        return 1;
    }
    // call fn
    sugi_gfx_clear(lua_tointeger(L, 1));
    return 1;
}


static int sugi_lua_fn_gfx_print(lua_State *L)
{
    // define default values
    char *s = "";
    int32_t x = 0;
    int32_t y = 0;
    uint8_t c = sugi_gfx_getcolor(); 

    // process arguments
    if (lua_isstring(L, 1)) 
    {
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


static int sugi_lua_fn_gfx_flip(lua_State *L)
{
    sugi_gfx_flip();
    return 1;
}


static int sugi_lua_fn_gfx_pset(lua_State *L)
{
    int32_t x = 0;
    int32_t y = 0;
    uint8_t c = sugi_gfx_getcolor();

    if (lua_isnumber(L, 1))
        x = lua_tointeger(L, 1);
    if (lua_isnumber(L, 2))
        y = lua_tointeger(L, 2);
    if (lua_isnumber(L, 3))
        c = lua_tointeger(L, 3);
    
    sugi_gfx_pset(x, y, c);
    return 1;
}


static int sugi_lua_fn_gfx_pget(lua_State *L)
{
    uint8_t c_out;
    int32_t x = 0;
    int32_t y = 0;

    if (lua_isnumber(L, 1));
        x = lua_tointeger(L, 1);
    if (lua_isnumber(L, 2));
        y = lua_tointeger(L, 2);

    sugi_gfx_pget(x, y, &c_out);
    lua_pushinteger(L, c_out);
    return 1;
}


static int sugi_lua_fn_gfx_camera(lua_State *L)
{
    int32_t x = 0;
    int32_t y = 0;

    if (lua_isnumber(L, 1));
        x = lua_tointeger(L, 1);
    if (lua_isnumber(L, 2));
        y = lua_tointeger(L, 2);
    
    sugi_gfx_camera(x, y);
    return 1;    
}


static int sugi_lua_fn_gfx_line(lua_State *L)
{
    int32_t x1 = 0;
    int32_t y1 = 0;
    int32_t x2 = 0;
    int32_t y2 = 0;
    uint8_t c = sugi_gfx_getcolor();

    if (lua_isnumber(L, 1));
        x1 = lua_tointeger(L, 1);
    if (lua_isnumber(L, 2));
        y1 = lua_tointeger(L, 2);
    if (lua_isnumber(L, 3));
        x2 = lua_tointeger(L, 3);
    if (lua_isnumber(L, 4));
        y2 = lua_tointeger(L, 4);
    if (lua_isnumber(L, 5));
        c = lua_tointeger(L, 5);

    sugi_gfx_line(x1, y1, x2, y2, c);
    return 1;
}


static int sugi_lua_fn_gfx_rect(lua_State *L)
{
    int32_t x1 = 0;
    int32_t y1 = 0;
    int32_t x2 = 0;
    int32_t y2 = 0;
    int8_t f   = 0;
    uint8_t c  = sugi_gfx_getcolor();

    if (lua_isnumber(L, 1));
        x1 = lua_tointeger(L, 1);
    if (lua_isnumber(L, 2));
        y1 = lua_tointeger(L, 2);
    if (lua_isnumber(L, 3));
        x2 = lua_tointeger(L, 3);
    if (lua_isnumber(L, 4));
        y2 = lua_tointeger(L, 4);
    if (lua_isnumber(L, 5));
        f = lua_tointeger(L, 5);
    if (lua_isnumber(L, 6));
        c = lua_tointeger(L, 6);

    sugi_gfx_rect(x1, y1, x2, y2, f, c);
    return 1;
}


static int sugi_lua_fn_gfx_circ(lua_State *L)
{
    int32_t x = 0;
    int32_t y = 0;
    int32_t r = 0;
    int8_t f  = 0;
    uint8_t c = sugi_gfx_getcolor();

    if (lua_isnumber(L, 1));
        x = lua_tointeger(L, 1);
    if (lua_isnumber(L, 2));
        y = lua_tointeger(L, 2);
    if (lua_isnumber(L, 3));
        r = lua_tointeger(L, 3);
    if (lua_isnumber(L, 4));
        f = lua_tointeger(L, 4);
    if (lua_isnumber(L, 5));
        c = lua_tointeger(L, 5);

    sugi_gfx_circ(x, y, r, f, c);
    return 1;
}


