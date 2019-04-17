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
  SUGI_SCREEN_WIDTH  = 512,
  SUGI_SCREEN_HEIGHT = 384,
  SUGI_RENDER_WIDTH  = 224,
  SUGI_RENDER_HEIGHT = 160,
  SUGI_USE_VSYNC     = 1,
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
// 0 - 0x3000 screen data...

uint8_t sugi_display_mode;

static const uint32_t sugi_memory_screen_size = // = 0x2800;
                      (SUGI_RENDER_WIDTH / 2) * SUGI_SCREEN_HEIGHT;

//                   0x10000   // 0x0000 -> 0xFFFF   64kB
//                   0x18000   // 0x0000 -> 0x17FFF  96kB 
uint8_t  sugi_memory[0xF0000]; // 0x0000 -> 0x1FFFF 128kB
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

/* Internal SUGI, SDL and OpenGL Functions */
void   sugi_call_init_internal(void);     /* Calls Init Func */
void   sugi_call_update_internal(void);   /* Calls Update Func */
void   sugi_call_draw_internal(void);     /* Calls Draw Func */
void   sugi_sdl_gl_init_internal();       /* SDL and OpenGL Init */
void   sugi_sdl_gl_deinit_internal();     /* SDL and OpenGL Deinit*/
void   sugi_sdl_delay_internal();         /* SDL Constant FPS Delay*/
void   sugi_gl_init_texture_internal();   /* OpenGL Texture Init */
void   sugi_gl_set_viewport_internal();   /* OpenGL Viewport Setup */
GLuint sugi_gl_compile_shader_internal(); /* Shader Compilation */
void   sugi_gl_render_internal();         /* Render to Screen */



/* GRAPHICS *******************************************************/
int8_t sugi_gfx_pset(int32_t x, int32_t y, uint8_t c_in);
int8_t sugi_gfx_pget(int32_t x, int32_t y, uint8_t *c_out);
void   sugi_gfx_camera(int32_t x, int32_t y);
void   sugi_gfx_clear(uint8_t c);



/* SHADERS ********************************************************/
const char *vert_shader_src[];
const char *frag_shader_src[];



/*  *********/


#endif
