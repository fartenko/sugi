#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sugi.h"


static bool     sugi_core_quit    = false;
static uint32_t sugi_time_step_ms = 1000 / 60;
static uint32_t sugi_delta_ticks  = 0;


/* SDL and OpenGL Functions ***********************************************************/
void sugi_sdl_gl_init_internal(void)
{
  sugi_delta_ticks = SDL_GetTicks();
  SDL_Init(SDL_INIT_EVERYTHING); 

  // Setting up SDL Window
  sugi_main_window = SDL_CreateWindow(
      "sugi",
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      SUGI_RENDER_WIDTH,
      SUGI_RENDER_HEIGHT,
      SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
    );
  SDL_SetWindowSize(sugi_main_window, SUGI_SCREEN_WIDTH, SUGI_SCREEN_HEIGHT);
  sugi_main_gl_context = SDL_GL_CreateContext(sugi_main_window);

  // Setting up OpenGL
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetSwapInterval(SUGI_USE_VSYNC);
  printf("%d", SUGI_USE_VSYNC);

  // glewExperimental is probably needed to work on Windows
  glewExperimental = GL_TRUE;
  glewInit();

  /* Clearing GL context */
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
  SDL_GL_SwapWindow(sugi_main_window);
}


void sugi_sdl_gl_deinit_internal(void)
{
  SDL_DestroyWindow(sugi_main_window);
  SDL_GL_DeleteContext(sugi_main_gl_context);
  SDL_Quit();
}


void sugi_sdl_delay_internal()
{
  uint32_t delay = sugi_time_step_ms - (SDL_GetTicks() - sugi_delta_ticks);
  delay = (delay > sugi_time_step_ms) ? sugi_time_step_ms : delay;
  SDL_Delay(delay);
  sugi_delta_ticks = SDL_GetTicks();
}



/* Sugi Functions ********************************************************************/
void sugi_set_init(void (*f)(void))
{ sugi_init_func = f; }

void sugi_set_update(void (*f)(void)) 
{ sugi_update_func = f; }

void sugi_set_draw(void (*f)(void))   
{ sugi_draw_func = f; }

void sugi_call_init_internal(void)    
{ sugi_init_func(); }

void sugi_call_update_internal(void)  
{ sugi_update_func(); }

void sugi_call_draw_internal(void)    
{ sugi_draw_func(); }
  

void sugi_core_init(void) 
{
  sugi_sdl_gl_init_internal();
  sugi_gl_init_texture_internal();
  sugi_gl_set_viewport_internal(SUGI_SCREEN_WIDTH, SUGI_SCREEN_HEIGHT, 0, 0);

  const GLuint shader = sugi_gl_compile_shader_internal();  
  glUseProgram(sugi_gl_program);

  sugi_draw_buffer_ptr   = &sugi_draw_buffer;
  sugi_memory_ptr        = &sugi_memory;
  sugi_memory_screen_ptr = &sugi_memory;
}


void sugi_core_run(void)
{
  sugi_call_init_internal();

  while (!sugi_core_quit) {
    while (SDL_PollEvent(&sugi_sdl_event)) {
      switch (sugi_sdl_event.type) {
      case SDL_QUIT:
        sugi_core_quit = true;
        break;
      case SDL_KEYDOWN:
        // . . . . . . . . . .(SDL_Keycode sdl_key)
        // sugi_poll_controlls(event.key.keysym.sym);
        //  | switch (sdl_key) {
        //  |   case SDLK_UP: keys['up'] = true; break;
        //  |   .....
        //  |   . . .
        //  | }
        break;
      default:
        break;
      }
    }

    // TODO: Add game input polling here
    sugi_call_update_internal();
    sugi_call_draw_internal();

    sugi_gl_render_internal();
    sugi_sdl_delay_internal();
  }

  sugi_sdl_gl_deinit_internal();
}

