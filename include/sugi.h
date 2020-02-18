#ifndef SUGI_H
#define SUGI_H

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "GL/glew.h"
#include "GL/gl.h"
#include "GL/glu.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"


#pragma region CONFIG
// TODO: move all of this to a config file, to prevent 
// from recompiling after a change
enum SUGI_CONFIG {
    SUGI_SCREEN_WIDTH  = 640 + 16, // 320 + 16,
    SUGI_SCREEN_HEIGHT = 512 + 16, // 256 + 16,
    SUGI_RENDER_WIDTH  = 320,      // 160, 
    SUGI_RENDER_HEIGHT = 256,      // 128,
    SUGI_USE_VSYNC     = 1,
    SUGI_RESIZABLE     = 0,
    SUGI_MAX_JOYSTICKS = 4,
};
enum SUGI_SDL_KB_MAP {
    SUGI_KB_BTN_UP    = SDL_SCANCODE_UP,
    SUGI_KB_BTN_DOWN  = SDL_SCANCODE_DOWN,
    SUGI_KB_BTN_LEFT  = SDL_SCANCODE_LEFT,
    SUGI_KB_BTN_RIGHT = SDL_SCANCODE_RIGHT,
    SUGI_KB_BTN_A     = SDL_SCANCODE_Z,
    SUGI_KB_BTN_B     = SDL_SCANCODE_X,
    SUGI_KB_BTN_X     = SDL_SCANCODE_A,
    SUGI_KB_BTN_Y     = SDL_SCANCODE_S,
};
enum SUGI_SDL_CONTROLLER_MAP {
    SUGI_CONTROLLER_UP    = SDL_CONTROLLER_BUTTON_DPAD_UP,
    SUGI_CONTROLLER_DOWN  = SDL_CONTROLLER_BUTTON_DPAD_DOWN,
    SUGI_CONTROLLER_LEFT  = SDL_CONTROLLER_BUTTON_DPAD_LEFT,
    SUGI_CONTROLLER_RIGHT = SDL_CONTROLLER_BUTTON_DPAD_RIGHT,
    SUGI_CONTROLLER_A     = SDL_CONTROLLER_BUTTON_A,
    SUGI_CONTROLLER_B     = SDL_CONTROLLER_BUTTON_B,
    SUGI_CONTROLLER_X     = SDL_CONTROLLER_BUTTON_X,
    SUGI_CONTROLLER_Y     = SDL_CONTROLLER_BUTTON_Y,
};
#pragma endregion CONFIG


#pragma region CORE_VARIABLES
/* SDL & GL variables */
SDL_Window    *sugi_main_window;
SDL_GLContext  sugi_main_gl_context;
SDL_Event      sugi_sdl_event;
GLuint         sugi_gl_program;
/* Sugi render buffer */
uint8_t  sugi_draw_buffer[SUGI_RENDER_WIDTH * SUGI_RENDER_HEIGHT];
uint8_t *sugi_draw_buffer_ptr;
/* Sugi keyboard codes */
uint8_t sugi_kb_map[8];
uint8_t sugi_controller_map[8];
/* Sugi font */
TTF_Font *sugi_font;
/* Joysticks */
SDL_Joystick       *sugi_joysticks[SUGI_MAX_JOYSTICKS];
SDL_GameController *sugi_gamecontrollers[SUGI_MAX_JOYSTICKS];
SDL_JoystickID      sugi_joysticks_id[SUGI_MAX_JOYSTICKS];
uint8_t             sugi_joysticks_opened[SUGI_MAX_JOYSTICKS];
/* Functions pointers */
void (*sugi_init_func)(void);
void (*sugi_update_func)(void);
void (*sugi_draw_func)(void);
/* Shaders */
const char *vert_shader_src[];
const char *frag_shader_src[];
/* Lua interpreter state pointer */
lua_State *L;
#pragma endregion CORE_VARIABLES


#pragma region TEXTINPUT
uint8_t *sugi_text_target;
uint8_t sugi_text_input_mode;
#pragma endregion TEXTINPUT


#pragma region WINDOWDATA
uint8_t  sugi_window_zoom;
uint16_t sugi_window_offset_x;
uint16_t sugi_window_offset_y;
// TODO: Move this to memory
uint16_t sugi_mouse_x;
uint16_t sugi_mouse_y;
#pragma endregion WINDOWDATA


#pragma region RAM
/* Sugi RAM */
// 0x0000 - 0x27FF screen data (4bit color)
// 0x2800 - 0x2FFF some other variables
//        |
//        |-0x2800 current color & something else
//        |-0x2801 display mode
//        |-........
// 0x3000 - 0x4FFF spritesheet 0-255
// 0x5000 - 0x9FFF map 160x128
// 0xA000 - ......
#define MEMOFFSET 0x100000
enum SUGI_MEMORY_TABLE {
    // VRAM
    SUGI_MEM_SCREEN_PTR      = 0x0000 + MEMOFFSET,
    // DRAW STATE
    SUGI_MEM_COLOR_PTR       = 0x2800 + MEMOFFSET, // size: 1 byte
    SUGI_MEM_DISP_MODE_PTR   = 0x2801 + MEMOFFSET, // size: 1 byte
    SUGI_MEM_CAMERA_X_PTR    = 0x2802 + MEMOFFSET, // size: 4 bytes
    SUGI_MEM_CAMERA_Y_PTR    = 0x2806 + MEMOFFSET, // size: 4 bytes
    SUGI_MEM_CLIP_LOW_PTR    = 0x280A + MEMOFFSET, // size: 4 bytes
    SUGI_MEM_CLIP_HIGH_PTR   = 0x280E + MEMOFFSET, // size: 4 bytes
    SUGI_MEM_PAL_DRAW_PTR    = 0x2812 + MEMOFFSET, // size: 16 bytes
    SUGI_MEM_PAL_SCREEN_PTR  = 0x2822 + MEMOFFSET, // size: 16 bytes
    SUGI_MEM_PALT_PTR        = 0x2832 + MEMOFFSET, // size: 2 bytes
    SUGI_MEM_PALT_SET_PTR    = 0x2834 + MEMOFFSET, // size: 1 byte
    SUGI_MEM_FILLP_PTR       = 0x2835 + MEMOFFSET, // size: 1 byte
    SUGI_MEM_CURSOR_LOW_PTR  = 0x2836 + MEMOFFSET, // size: 2 bytes
    SUGI_MEM_CURSOR_HITH_PTR = 0x2838 + MEMOFFSET, // size: 2 bytes
    SUGI_MEM_BTNP_PTR        = 0x283A + MEMOFFSET, // size: 4 bytes (for each player)
    SUGI_MEM_BTN_PTR         = 0x283E + MEMOFFSET, // size: 4 bytes (for each player)
    SUGI_MEM_SPRSHEET_PTR    = 0x3000 + MEMOFFSET, // size: 0x2000
    SUGI_MEM_MAPSHEET_PTR    = 0x5000 + MEMOFFSET, // size: 0x5000
};

// Regarding map:
// sprite 0 is a transparent sprite in map
// optimal mem_set for screen to spritescheet is memset(0x7800,0x0000,0x2800)

// uint8_t sugi_display_mode;
static const uint32_t sugi_memory_screen_size =
                                            (SUGI_RENDER_WIDTH / 2) * SUGI_RENDER_HEIGHT;

//                   0x10000   // 0x0000 -> 0xFFFF   64kB
//                   0x18000   // 0x0000 -> 0x17FFF  96kB
uint8_t  sugi_memory[0x1000000]; //0x20000]; // 0x0000 -> 0x1FFFF 128kB
uint8_t *sugi_memory_ptr;
uint8_t *sugi_memory_screen_ptr;
#pragma endregion RAM


#pragma region CORE_FUNCTIONS
void sugi_core_run(void);                   /* Runs an engine */
void sugi_core_set_init(void (*f)(void));   /* Sets custom Init func */
void sugi_core_set_update(void (*f)(void)); /* Sets custom Update func */
void sugi_core_set_draw(void (*f)(void));   /* Sets custom Draw func */
/* Internal SUGI Functions */
void sugi_call_init_internal(void);         /* Calls Init Func */
void sugi_call_update_internal(void);       /* Calls Update Func */
void sugi_call_draw_internal(void);         /* Calls Draw Func */
#pragma endregion CORE_FUNCTIONS


#pragma region RENDERER_FUNCTIONS
void   sugi_renderer_gl_set_viewport_internal();   /* OpenGL Viewport Setup */
GLuint sugi_renderer_gl_compile_shader_internal(); /* Shader Compilation */
void   sugi_renderer_draw_internal();              /* Render to Screen */
/* Display modes */
void sugi_render_mode_default_internal(uint32_t rw, uint32_t rh);      /* Default display mode */
void sugi_render_mode_stretched_internal(uint32_t rw, uint32_t rh);    /* Horizontally stretched mode */
void sugi_render_mode_zoomed_internal(uint32_t rw, uint32_t rh);       /* 2x zoom */
void sugi_render_mode_square_internal(uint32_t rw, uint32_t rh);       /* Square display mode */
void sugi_render_mode_square_small_internal(uint32_t rw, uint32_t rh); /* 64x64 display mode */
void sugi_render_mode_square_pico_internal(uint32_t rw, uint32_t rh);  /* 128x128 pico-8 display mode */
#pragma endregion RENDERER_FUNCTIONS


#pragma region GFX_FUNCTIONS
// calls a draw function
void    sugi_gfx_flip();
// color manipulation
void    sugi_gfx_setcolor(uint8_t c_in);
uint8_t sugi_gfx_getcolor(void);
// pixel get and pixel set
int8_t  sugi_gfx_pset(int32_t x, int32_t y, uint8_t c_in);
int8_t  sugi_gfx_pset_no_col(int32_t x, int32_t y);
int8_t  sugi_gfx_pget(int32_t x, int32_t y, uint8_t *c_out);
// camera position
void    sugi_gfx_camera(int32_t x, int32_t y);
// clear screen
void    sugi_gfx_clear(uint8_t c);
void    sugi_gfx_clear_no_col(void);
// draw shapes
void    sugi_gfx_line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint8_t c_in);
void    sugi_gfx_line_no_col(int32_t x1, int32_t y1, int32_t x2, int32_t y2);
void    sugi_gfx_hline(int32_t x1, int32_t x2, int32_t y, uint8_t c_in);
void    sugi_gfx_vline(int32_t x, int32_t y1, int32_t y2, uint8_t c_in);
void    sugi_gfx_rect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int8_t fill, uint8_t c_in);
void    sugi_gfx_rect_no_col(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int8_t fill);
void    sugi_gfx_circ(int32_t xc, int32_t yc, int32_t r, int8_t fill, uint8_t c_in);
void    sugi_gfx_circ_no_col(int32_t xc, int32_t yc, int32_t r, int8_t fill);
void    sugi_gfx_circ_segment_internal(int32_t x0, int32_t y0, int32_t x, int32_t y, int8_t fill, uint8_t c_in);
// clip drawing region
void    sugi_gfx_clip_internal(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void    sugi_gfx_clip_reset(void);
void    sugi_gfx_clip(int32_t x1, int32_t y1, int32_t x2, int32_t y2);
// palette manipulation
void    sugi_gfx_pal_reset(void);
void    sugi_gfx_pal_no_mode(uint8_t c1, uint8_t c2);
void    sugi_gfx_pal(uint8_t c1, uint8_t c2, uint8_t mode);
void    sugi_gfx_palt_reset(void);
void    sugi_gfx_palt(uint8_t c, uint8_t t);
// draw sprites and sprite manipulation
void    sugi_gfx_spr_pset_internal(int32_t x, int32_t y, uint8_t c_in);
void    sugi_gfx_spr(uint8_t s, int32_t x, int32_t y);
void    sugi_gfx_sset_no_col(int32_t x, int32_t t);
void    sugi_gfx_sset(int32_t x, int32_t y, uint8_t c_in);
int8_t  sugi_gfx_sget(int32_t x, int32_t y, uint8_t *c_out);
void    sugi_gfx_sspr(int32_t sx, int32_t sy, int32_t sw, int32_t sh, int32_t x, int32_t y, int32_t w, int32_t h);
// Printing text
void    sugi_gfx_print(char *str, int32_t x, int32_t y, uint8_t c);

// draw map
void    sugi_map_draw(uint8_t ox, uint8_t oy);
// void    sugi_gfx_map(int32_t map_cx, int32_t map_cy, int32_t sx, int32_t sy, int32_t map_cw, int32_t map_ch);
// void    sugi_gfx_mset(int32_t cx, int32_t cy, uint8_t s);
// uint8_t sugi_gfx_mset(int32_t cx, int32_t cy);
#pragma endregion GFX_FUNCTIONS


#pragma region INPUT_FUNCTIONS
// Currently
void    sugi_input_process_kb_press_state(const uint8_t * state);
void    sugi_input_process_kb_release_state(const uint8_t * state);
uint8_t sugi_input_btn(uint8_t b, uint8_t p);
uint8_t sugi_input_btnp(uint8_t b, uint8_t p);
void    sugi_input_clear_btnp_internal(void);
void    sugi_input_process_controller_press_button(uint8_t button, uint8_t player);
void    sugi_input_process_controller_release_button(uint8_t button, uint8_t player);
#pragma endregion INPUT_FUNCTIONS


#pragma region LUA_INTERPRETER_FUNCTIONS
#pragma endregion LUA_INTERPRETER_FUNCTIONS


/* INPUT **********************************************************/
// * button input
// * text input
// * joystick/gamepad input
//


/* SOUND **********************************************************/
// * sound playback
// * mixers
// * soundwaves
// * sound and music creation
//


/* GRAPHICS *******************************************************/
// * text mode (or ascii mode)
//


/* NETWORKING *****************************************************/
// * multiplayer up to 4 PCs (local, when gamepads connected or net)
// * connect to "lobby", when everyone ready, start the cart
// * synchronize cart at startup???
// * in multiplayer, everyone sends its input data, to update every-
// -one else (e.g. P1 moved left, left button will be pressed-
// - for P1 at his PC, and "pressed" at P2,P3,P4's PCs)
//

#endif
