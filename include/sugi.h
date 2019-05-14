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
enum config {
  SUGI_SCREEN_WIDTH  = 480,
  SUGI_SCREEN_HEIGHT = 384, // ^               ^       ^
  SUGI_RENDER_WIDTH  = 160, // 160, // 224, // 224, // 224,
  SUGI_RENDER_HEIGHT = 128, // 128, // 144, // 160, // 128,
  SUGI_USE_VSYNC     = 1,
  // GL_MAJOR_VERSION   = 3,
  // GL_MINOR_VERSION   = 2,
  // GL_USE_VSYNC       = 1,
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
// 0x3000 - 0x4FFF spritesheet 0-255
// 0x5000 - 0x9FFF map 160x128
// 0xA000 - ......

uint8_t sugi_display_mode;

static const uint32_t sugi_memory_screen_size = // = 0x2800;
                      (SUGI_RENDER_WIDTH / 2) * SUGI_SCREEN_HEIGHT;

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
int8_t sugi_gfx_pset(int32_t x, int32_t y, uint8_t c_in);
int8_t sugi_gfx_pget(int32_t x, int32_t y, uint8_t *c_out);
void   sugi_gfx_camera(int32_t x, int32_t y);
void   sugi_gfx_clear(uint8_t c);
void   sugi_gfx_line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint8_t c_in);
void   sugi_gfx_hline(int32_t x1, int32_t x2, int32_t y, uint8_t c_in);
void   sugi_gfx_vline(int32_t x, int32_t y1, int32_t y2, uint8_t c_in);
void   sugi_gfx_rect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int8_t fill, uint8_t c_in);
void   sugi_gfx_circ(int32_t xc, int32_t yc, int32_t r, int8_t fill, uint8_t c_in);
void   sugi_gfx_circ_segment_internal(int32_t x0, int32_t y0, int32_t x, int32_t y, int8_t fill, uint8_t c_in);
float  sugi_gfx_circ_turnatan2_internal(float y, float x);


/* SHADERS ********************************************************/
const char *vert_shader_src[];
const char *frag_shader_src[];



/*  *********/


#endif
