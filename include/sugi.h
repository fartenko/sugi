#ifndef SUGI_H
#define SUGI_H

/* SDL headers */
#include "SDL2/SDL.h"
/* OpenGL, GLEW headers */
#include "GL/glew.h"
#include "GL/gl.h"
#include "GL/glu.h"
/* Custom headers */
#include "bool.h"



/* CONFIG VALUES **************************************************/
enum sugi_config {
  SUGI_SCREEN_WIDTH  = 480,
  SUGI_SCREEN_HEIGHT = 384,
  SUGI_RENDER_WIDTH  = 160,
  SUGI_RENDER_HEIGHT = 128,
  SUGI_USE_VSYNC     = 1,
  SUGI_RESIZABLE     = 0,
};


/* CORE ***********************************************************/
SDL_Window    *sugi_main_window;
SDL_GLContext  sugi_main_gl_context;
SDL_Event      sugi_sdl_event;
GLuint         sugi_gl_program;

/* Sugi render buffer */
uint8_t  sugi_draw_buffer[SUGI_RENDER_WIDTH * SUGI_RENDER_HEIGHT];
uint8_t *sugi_draw_buffer_ptr;

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

enum sugi_memory_table {
  // VRAM
  SUGI_MEM_SCREEN_PTR     = 0x0000,
  // DRAW STATE
  SUGI_MEM_COLOR_PTR      = 0x2800,
  SUGI_MEM_DISP_MODE_PTR  = 0x2801,
  SUGI_MEM_CAMERA_X_PTR   = 0x2802,
  SUGI_MEM_CAMERA_Y_PTR   = 0x2806,
  SUGI_MEM_CLIP_PTR       = 0x280A, // 0x280B, 0x280C, 0x280D
  SUGI_MEM_PAL_DRAW_PTR   = 0x280E, // 0x280F .. 0x281D
  SUGI_MEM_PAL_SCREEN_PTR = 0x281E, // 0x281F .. 0x282D
  SUGI_MEM_PALT_PTR       = 0x282E, // 0x282F
  SUGI_MEM_PALT_SET_PTR   = 0x2830,
  SUGI_MEM_FILLP_PTR      = 0x2831,
  SUGI_MEM_CURSOR_PTR     = 0x2833,
                       // = 0x2835,
  SUGI_MEM_SPRSHEET_PTR   = 0x3000, // size = 0x2000
                       // = 0x5000,
  SUGI_MEM_MAPSHEET_PTR   = 0x5000, // size = 0x5000
                       // = 0xA000,
};

// Regarding map:
// sprite 0 is a transparent sprite in map
// optimal mem_set for screen to spritescheet is memset(0x7800,0x0000,0x2800)

// uint8_t sugi_display_mode;
static const uint32_t sugi_memory_screen_size =
                      (SUGI_RENDER_WIDTH / 2) * SUGI_RENDER_HEIGHT;

//                   0x10000   // 0x0000 -> 0xFFFF   64kB
//                   0x18000   // 0x0000 -> 0x17FFF  96kB
uint8_t  sugi_memory[0x20000]; // 0x0000 -> 0x1FFFF 128kB
uint8_t *sugi_memory_ptr;
uint8_t *sugi_memory_screen_ptr;


/* Functions pointers */
void (*sugi_init_func)(void);
void (*sugi_update_func)(void);
void (*sugi_draw_func)(void);


/* Functions */
void sugi_core_init(void);             /* Main initialization */
void sugi_core_mem_init(void);         /* Memory initialization */
void sugi_core_run(void);              /* Runs an engine */
void sugi_set_init(void (*f)(void));   /* Sets custom Init func */
void sugi_set_update(void (*f)(void)); /* Sets custom Update func */
void sugi_set_draw(void (*f)(void));   /* Sets custom Draw func */

/* Internal SUGI, SDL Functions */
void   sugi_call_init_internal(void);     /* Calls Init Func */
void   sugi_call_update_internal(void);   /* Calls Update Func */
void   sugi_call_draw_internal(void);     /* Calls Draw Func */
void   sugi_sdl_gl_init_internal(void);   /* SDL and OpenGL Init */
void   sugi_sdl_gl_deinit_internal(void); /* SDL and OpenGL Deinit*/
void   sugi_sdl_delay_internal(void);     /* SDL Constant FPS Delay*/

/* Internal OpenGL Fcuntions */
void   sugi_gl_init_texture_internal();   /* OpenGL Texture Init */
void   sugi_gl_set_viewport_internal();   /* OpenGL Viewport Setup */
GLuint sugi_gl_compile_shader_internal(); /* Shader Compilation */
void   sugi_gl_render_internal();         /* Render to Screen */
/* Display modes */
void sugi_gl_render_mode_default_internal(uint32_t rw, uint32_t rh);      /* Default display mode */
void sugi_gl_render_mode_stretched_internal(uint32_t rw, uint32_t rh);    /* Horizontally stretched mode */
void sugi_gl_render_mode_square_internal(uint32_t rw, uint32_t rh);       /* Square display mode */
void sugi_gl_render_mode_square_small_internal(uint32_t rw, uint32_t rh); /* 64x64 display mode */
void sugi_gl_render_mode_square_pico_internal(uint32_t rw, uint32_t rh);  /* 128x128 pico-8 display mode */


/* GRAPHICS *******************************************************/
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
float   sugi_gfx_circ_turnatan2_internal(float y, float x);
// clip drawing region
void    sugi_gfx_clip_internal(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
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
// void sugi_gfx_sspr(...);
// draw map
// void    sugi_gfx_map(int32_t map_cx, int32_t map_cy, int32_t sx, int32_t sy, int32_t map_cw, int32_t map_ch);
// void    sugi_gfx_mset(int32_t cx, int32_t cy, uint8_t s);
// uint8_t sugi_gfx_mset(int32_t cx, int32_t cy);


/* SHADERS ********************************************************/
const char *vert_shader_src[];
const char *frag_shader_src[];


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


/* NETWORKING *****************************************************/
// * multiplayer up to 4 PCs (local, when gamepads connected or net)
// * connect to "lobby", when everyone ready, start the cart
// * synchronize cart at startup???
// * in multiplayer, everyone sends its input data, to update every-
// -one else (e.g. P1 moved left, left button will be pressed-
// - for P1 at his PC, and "pressed" at P2,P3,P4's PCs)
//

#endif
