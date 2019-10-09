#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sugi.h"


void sugi_core_set_init(void (*f)(void))
{ sugi_init_func = f; }

void sugi_core_set_update(void (*f)(void))
{ sugi_update_func = f; }

void sugi_core_set_draw(void (*f)(void))
{ sugi_draw_func = f; }

void sugi_call_init_internal(void)
{ if (sugi_init_func) sugi_init_func(); }

void sugi_call_update_internal(void)
{ if (sugi_update_func) sugi_update_func(); }

void sugi_call_draw_internal(void)
{ if (sugi_draw_func) sugi_draw_func(); }


void sugi_core_run(void)
{
  uint8_t  sugi_core_quit    = 0;
  uint32_t sugi_time_step_ms = 1000 / 60;
  uint32_t sugi_delta_ticks  = 0;


  #pragma region INIT_SDL_GL
  // Initializing SDL
  sugi_delta_ticks = 0;
  SDL_Init(SDL_INIT_EVERYTHING);
  // Setting up SDL Window
  sugi_main_window = SDL_CreateWindow(
      "sugi",
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      SUGI_RENDER_WIDTH,
      SUGI_RENDER_HEIGHT,
      SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
  // Resizing a window
  SDL_SetWindowSize(sugi_main_window, SUGI_SCREEN_WIDTH, SUGI_SCREEN_HEIGHT);
  // Creating OpenGL context
  sugi_main_gl_context = SDL_GL_CreateContext(sugi_main_window);
  // Setting up OpenGL Attributes
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  // Setting Vsync
  SDL_GL_SetSwapInterval(SUGI_USE_VSYNC);
  // Setting OpenGL Attributes
  int32_t gl_major_version = 0;
  int32_t gl_minor_version = 0;
  SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &gl_major_version);
  SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &gl_minor_version);
  // Print some information
  printf("VSync: %d\n", SUGI_USE_VSYNC);
  printf("GL Context version: %d.%d\n", gl_major_version, gl_minor_version);
  printf("OpenGL version: %s\n", glGetString(GL_VERSION));
  // glewExperimental is probably needed to work on Windows
  glewExperimental = GL_TRUE;
  glewInit();
  /* Clearing GL context */
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
  // Drawing first frame
  SDL_GL_SwapWindow(sugi_main_window);
  #pragma endregion INIT_SDL_GL


  #pragma region INIT_GL
  glTexImage2D(
      GL_TEXTURE_2D, 0, GL_ALPHA,
      SUGI_RENDER_WIDTH, SUGI_RENDER_HEIGHT,
      0, GL_ALPHA, GL_UNSIGNED_BYTE,
      (GLvoid*)sugi_draw_buffer
    );
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glEnable(GL_TEXTURE_2D);
  // Set viewport
  sugi_renderer_gl_set_viewport_internal(SUGI_SCREEN_WIDTH, SUGI_SCREEN_HEIGHT, 0, 0);
  // compiles to sugi_gl_program
  sugi_renderer_gl_compile_shader_internal();
  glUseProgram(sugi_gl_program);
  // getting pointer to a draw buffer
  sugi_draw_buffer_ptr = &sugi_draw_buffer;
  #pragma endregion INIT_GL


  #pragma region INIT_RAM
  sugi_memory_ptr        = &sugi_memory;
  sugi_memory_screen_ptr = &sugi_memory;
  sugi_gfx_clip_reset();
  sugi_gfx_pal_reset();
  #pragma endregion INIT_RAM


  #pragma region GAME_LOOP
  // Runs an ingame init function
  sugi_call_init_internal();
  // Starting game loop
  while (!sugi_core_quit) {
    while (SDL_PollEvent(&sugi_sdl_event)) {
      switch (sugi_sdl_event.type) {
      case SDL_QUIT:
        sugi_core_quit = 1;
        break;
      case SDL_KEYDOWN:
        // . . . . . . . . . .(SDL_Keycode sdl_key)
        // sugi_poll_controlls(event.key.keysym.sym);
        //  | switch (sdl_key) {
        //  |   case SDLK_UP: keys['up'] = 1; break;
        //  |   .....
        //  |   . . .
        //  | }
        break;
      default:
        break;
      }
    }
    // TODO: Add game input polling here
    // Calls an ingame update and draw functoins
    sugi_call_update_internal();
    sugi_call_draw_internal();

    // Rendering a screen
    sugi_renderer_draw_internal();

    // Capping a game to 60 FPS
    uint32_t delay = sugi_time_step_ms - (SDL_GetTicks() - sugi_delta_ticks);
    delay = (delay > sugi_time_step_ms) ? sugi_time_step_ms : delay;
    SDL_Delay(delay);
    sugi_delta_ticks = SDL_GetTicks();
  }
  #pragma endregion GAME_LOOP


  #pragma region DEINIT_SDL_GL
  SDL_DestroyWindow(sugi_main_window);
  SDL_GL_DeleteContext(sugi_main_gl_context);
  SDL_Quit();
  #pragma endregion DEINIT_SDL_GL
}
