#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sugi.h"


static bool     sugi_core_quit    = false;
static uint32_t sugi_time_step_ms = 1000 / 60;
static uint32_t sugi_delta_ticks  = 0;
uint8_t         sugi_display_mode = 0;


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


void sugi_gl_init_texture_internal(void)
{
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
}


void sugi_gl_set_viewport_internal(int x, int y, int ox, int oy)
{
  // ox, oy -> screen offset
  // x, y   -> screen pos
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, x, y, 0);
  glMatrixMode(GL_MODELVIEW);
  glViewport(ox, oy, x, y); 
}


void sugi_gl_render_internal(void)
{
  int32_t sw, sh;
  SDL_GetWindowSize(sugi_main_window, &sw, &sh);

  uint32_t rw = SUGI_RENDER_WIDTH;
  uint32_t rh = SUGI_RENDER_HEIGHT;

  uint8_t zoom_w = sw / rw;
  uint8_t zoom_h = sh / rh;
  uint8_t zoom   = (zoom_w < zoom_h) ? zoom_w : zoom_h;
  
  // Reading 4-bit screen data from ram and converting ...
  // ... it to 8 bit color for OpenGL 8-bit Texture
  switch(sugi_display_mode)
  {
    case 1: //  96x128 stretched
      for (int i = 0; i < sugi_memory_screen_size; i++)
      {
        if (i % (rw / 2) < (rw / 4))
        {
          // color1, color2: 4 bit colors
          // shift_back:     offsets some adresses back to ...
          // ... fill the gaps of unrendered pixels
          uint8_t color1     = *(sugi_memory_ptr + i) >> 4 & 0xf;
          uint8_t color2     = *(sugi_memory_ptr + i) & 0xf;
          int32_t shift_back = (i / (rw / 2)) * rw;
          *(sugi_draw_buffer_ptr + i * 4 + 0 - shift_back) = color1; 
          *(sugi_draw_buffer_ptr + i * 4 + 1 - shift_back) = color1; 
          *(sugi_draw_buffer_ptr + i * 4 + 2 - shift_back) = color2; 
          *(sugi_draw_buffer_ptr + i * 4 + 3 - shift_back) = color2; 
        }
      }
      break;
    case 2: //  64x64  square (pico8 64x64 mode)
      for (int i = 0; i < sugi_memory_screen_size; i++)
      {
        if (i % (rw / 2) < 32 && i < (rw / 2) * 64)
        {
          // offset_x: offsets render in x axis
          // offset_t: offsets render in y axis
          uint8_t color1     = *(sugi_memory_ptr + i) >> 4 & 0xf;
          uint8_t color2     = *(sugi_memory_ptr + i) & 0xf;
          int32_t shift_back = (i / (rw / 2)) / 64;
          int16_t offset_x   = (rw / 2) - 64;
          int16_t offset_y   = (rh - 128) * rw / 2;
          // upper pixels
          *(sugi_draw_buffer_ptr + offset_x + offset_y + i * 4 + 0 - shift_back) = color1; 
          *(sugi_draw_buffer_ptr + offset_x + offset_y + i * 4 + 1 - shift_back) = color1;
          *(sugi_draw_buffer_ptr + offset_x + offset_y + i * 4 + 2 - shift_back) = color2; 
          *(sugi_draw_buffer_ptr + offset_x + offset_y + i * 4 + 3 - shift_back) = color2; 
          // lower pixels
          offset_x += rw;
          *(sugi_draw_buffer_ptr + offset_x + offset_y + i * 4 + 0 - shift_back) = color1; 
          *(sugi_draw_buffer_ptr + offset_x + offset_y + i * 4 + 1 - shift_back) = color1;
          *(sugi_draw_buffer_ptr + offset_x + offset_y + i * 4 + 2 - shift_back) = color2; 
          *(sugi_draw_buffer_ptr + offset_x + offset_y + i * 4 + 3 - shift_back) = color2; 
        }
      }
      break;
    case 3: // 128x128 square (pico8 mode)
      for (int i = 0; i < sugi_memory_screen_size; i++)
      {
        if (i % (rw / 2) < 64 && i < (rw / 2) * 128)
        {
          uint8_t color1   = *(sugi_memory_ptr + i) >> 4 & 0xf;
          uint8_t color2   = *(sugi_memory_ptr + i) & 0xf;
          int16_t offset_x = (rw - 128) / 2;
          int16_t offset_y = (rh - 128) * rw / 2;
          *(sugi_draw_buffer_ptr + offset_x + offset_y + i * 2 + 0) = color1; 
          *(sugi_draw_buffer_ptr + offset_x + offset_y + i * 2 + 1) = color2;
        }
      }
      break;
    case 4: // square mode
      for (int i = 0; i < sugi_memory_screen_size; i++)
      {
        if (i % (rw / 2) < (rh / 2))
        {
          uint8_t color1   = *(sugi_memory_ptr + i) >> 4 & 0xf;
          uint8_t color2   = *(sugi_memory_ptr + i) & 0xf;
          int16_t offset_x = (rw - rh) / 2;
          *(sugi_draw_buffer_ptr + offset_x + i * 2 + 0) = color1; 
          *(sugi_draw_buffer_ptr + offset_x + i * 2 + 1) = color2;
        }
      }
      break;
    default: 
      for (int i = 0; i < sugi_memory_screen_size; i++)
      {
        *(sugi_draw_buffer_ptr + i * 2 + 0) = *(sugi_memory_ptr + i) >> 4 & 0xf; 
        *(sugi_draw_buffer_ptr + i * 2 + 1) = *(sugi_memory_ptr + i) & 0xf;
      }
      break;
  }
  
  /* Copying pixels to a texture */
  glTexSubImage2D(
      GL_TEXTURE_2D,
      0, 0, 0, rw, rh,
      GL_ALPHA, GL_UNSIGNED_BYTE,
      (GLvoid*)sugi_draw_buffer 
      );

  glClear(GL_COLOR_BUFFER_BIT);
  // Drawing a texture
  glBegin(GL_QUADS);
  glTexCoord2d(0.0, 0.0); glVertex2d(0.0,       0.0);
  glTexCoord2d(1.0, 0.0); glVertex2d(rw * zoom, 0.0);
  glTexCoord2d(1.0, 1.0); glVertex2d(rw * zoom, rh * zoom);
  glTexCoord2d(0.0, 1.0); glVertex2d(0.0,       rh * zoom);
  glEnd();

  // Resizing and moving a viewport every time we resize or draw the screen
  // TODO: Move it to the resized callback
  sugi_gl_set_viewport_internal(rw * zoom, rh * zoom, (sw - rw * zoom) / 2, (sh - rh * zoom) / 2);
  SDL_GL_SwapWindow(sugi_main_window);
}


GLuint sugi_gl_compile_shader_internal()
{
  sugi_gl_program = glCreateProgram();
  
  // Creating and compiling a Vertex Shader
  GLint vert_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vert_shader, 1, vert_shader_src, NULL);
  glCompileShader(vert_shader);
  glAttachShader(sugi_gl_program, vert_shader);

  // Creating and compiling a Fragment Shader
  GLint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(frag_shader, 1, frag_shader_src, NULL);
  glCompileShader(frag_shader);
  glAttachShader(sugi_gl_program, frag_shader);

  // Checking for GL shader compilation errors
  GLint v_compiled = 0;
  glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &v_compiled);
  if (v_compiled == GL_FALSE) {
    printf("ERR: Vertex shader not compiled.\n");
  }

  GLint f_compiled = 0;
  glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &f_compiled);
  if (f_compiled == GL_FALSE) {
    printf("ERR: Fragment shader not compiled.\n");
  }

  if (v_compiled == GL_FALSE || f_compiled == GL_FALSE) {
    printf("ERR: Failed to link gl program.\n");
    return 0;
  }

  glLinkProgram(sugi_gl_program);
  return sugi_gl_program;
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

