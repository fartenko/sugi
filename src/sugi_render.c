#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sugi.h"


#pragma region RENDERER_FUNCTIONS
void sugi_renderer_gl_set_viewport_internal(int x, int y, int ox, int oy)
{
  // ox, oy -> screen offset
  // x, y   -> screen pos
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, x, y, 0);
  glMatrixMode(GL_MODELVIEW);
  glViewport(ox, oy, x, y);
}


GLuint sugi_renderer_gl_compile_shader_internal()
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


void sugi_renderer_draw_internal(void)
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
  //uint8_t sugi_display_mode = *(sugi_memory_ptr + SUGI_MEM_DISP_MODE_PTR);
  uint8_t sugi_display_mode = 0;//2;
  switch(sugi_display_mode)
  {
    case 1:
      sugi_render_mode_stretched_internal(rw, rh);
      break;
    case 2:
      sugi_render_mode_zoomed_internal(rw, rh);
      break;
    case 3:
      sugi_render_mode_square_small_internal(rw, rh);
      break;
    case 4:
      sugi_render_mode_square_pico_internal(rw, rh);
      break;
    case 5:
      sugi_render_mode_square_internal(rw, rh);
      break;
    default:
      sugi_render_mode_default_internal(rw, rh);
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

  // TODO: Move it to the resized callback
  // Resizing and moving a viewport every time we resize or draw the screen
  sugi_renderer_gl_set_viewport_internal(rw * zoom, rh * zoom, (sw - rw * zoom) / 2, (sh - rh * zoom) / 2);
  // TODO: If using imgui, put igRender before SwapWindow
  SDL_GL_SwapWindow(sugi_main_window);
}
#pragma endregion RENDERER_FUNCTIONS


#pragma region RENDER_MODES
void sugi_render_mode_default_internal(uint32_t rw, uint32_t rh)
{
  for (int i = 0; i < sugi_memory_screen_size; i++)
  {
    // we split a 8 bit value into two 4bit colors
    uint8_t color1 = *(sugi_memory_ptr + i) >> 4 & 0xf;
    uint8_t color2 = *(sugi_memory_ptr + i) & 0xf;
    // applying a screen palette
    color1 = *(sugi_memory + SUGI_MEM_PAL_SCREEN_PTR + color1);
    color2 = *(sugi_memory + SUGI_MEM_PAL_SCREEN_PTR + color2);
    // TODO: rename sugi_draw_buffer_ptr to sugi_screen_buffer_ptr
    // putting pixels to a draw buffer (or screen buffer)
    *(sugi_draw_buffer_ptr + i * 2 + 0) = color1;
    *(sugi_draw_buffer_ptr + i * 2 + 1) = color2;
  }
}


void sugi_render_mode_stretched_internal(uint32_t rw, uint32_t rh)
{
  for (int i = 0; i < sugi_memory_screen_size; i++)
  {
    if (i % (rw / 2) < (rw / 4))
    {
      // color1, color2: 4 bit colors
      // shift_back:     offsets some adresses back to ..
      //             .. fill the gaps of unrendered pixels
      uint8_t color1 = *(sugi_memory_ptr + i) >> 4 & 0xf;
      uint8_t color2 = *(sugi_memory_ptr + i) & 0xf;
      // shifting back to skip second half of a screen
      int32_t shift_back = (i / (rw / 2)) * rw;
      // applying a screen palette
      color1 = *(sugi_memory + SUGI_MEM_PAL_SCREEN_PTR + color1);
      color2 = *(sugi_memory + SUGI_MEM_PAL_SCREEN_PTR + color2);
      // putting 'stretched' pixels
      *(sugi_draw_buffer_ptr + i * 4 + 0 - shift_back) = color1;
      *(sugi_draw_buffer_ptr + i * 4 + 1 - shift_back) = color1;
      *(sugi_draw_buffer_ptr + i * 4 + 2 - shift_back) = color2;
      *(sugi_draw_buffer_ptr + i * 4 + 3 - shift_back) = color2;
    }
  }
}


void sugi_render_mode_zoomed_internal(uint32_t rw, uint32_t rh)
{
  for (int i = 0; i < sugi_memory_screen_size / 2; i++)
  {
    if (i % (rw / 2) < (rw / 4))
    {
      // color1, color2: 4 bit colors
      // shift_back:     offsets some adresses back to ..
      //             .. fill the gaps of unrendered pixels
      uint8_t color1     = *(sugi_memory_ptr + i) >> 4 & 0xf;
      uint8_t color2     = *(sugi_memory_ptr + i) & 0xf;
      // shifting back to skip second half of a screen
      int32_t shift_back = (i / (rw / 2)) * rw;
      int32_t zoom_off = (rw / 2) * (int)(i / (rw / 4));

      // applying a screen palette
      color1 = *(sugi_memory + SUGI_MEM_PAL_SCREEN_PTR + color1);
      color2 = *(sugi_memory + SUGI_MEM_PAL_SCREEN_PTR + color2);
      // putting 'stretched' pixels
      *(sugi_draw_buffer_ptr + i * 4 + 0 - shift_back + zoom_off) = color1;
      *(sugi_draw_buffer_ptr + i * 4 + 1 - shift_back + zoom_off) = color1;
      *(sugi_draw_buffer_ptr + i * 4 + 2 - shift_back + zoom_off) = color2;
      *(sugi_draw_buffer_ptr + i * 4 + 3 - shift_back + zoom_off) = color2;
      *(sugi_draw_buffer_ptr + i * 4 + 0 - shift_back + zoom_off + rw) = color1;
      *(sugi_draw_buffer_ptr + i * 4 + 1 - shift_back + zoom_off + rw) = color1;
      *(sugi_draw_buffer_ptr + i * 4 + 2 - shift_back + zoom_off + rw) = color2;
      *(sugi_draw_buffer_ptr + i * 4 + 3 - shift_back + zoom_off + rw) = color2;
    }
  }
}


void sugi_render_mode_square_internal(uint32_t rw, uint32_t rh)
{
  for (int i = 0; i < sugi_memory_screen_size; i++)
  {
    if (i % (rw / 2) < (rh / 2))
    {
      uint8_t color1   = *(sugi_memory_ptr + i) >> 4 & 0xf;
      uint8_t color2   = *(sugi_memory_ptr + i) & 0xf;
      // offsets screen right
      int16_t offset_x = (rw - rh) / 2;
      // applying a screen palette
      color1 = *(sugi_memory + SUGI_MEM_PAL_SCREEN_PTR + color1);
      color2 = *(sugi_memory + SUGI_MEM_PAL_SCREEN_PTR + color2);
      *(sugi_draw_buffer_ptr + offset_x + i * 2 + 0) = color1;
      *(sugi_draw_buffer_ptr + offset_x + i * 2 + 1) = color2;
    }
  }
}


void sugi_render_mode_square_small_internal(uint32_t rw, uint32_t rh)
{
  for (int i = 0; i < sugi_memory_screen_size; i++)
  {
    if (i % (rw / 2) < 32 && i < (rw / 2) * 64)
    {
      // offset_x: offsets render in x axis
      // offset_t: offsets render in y axis
      uint8_t color1     = *(sugi_memory_ptr + i) >> 4 & 0xf;
      uint8_t color2     = *(sugi_memory_ptr + i) & 0xf;
      // shifting back to skip unnecesary pixels
      int32_t shift_back = (i / (rw / 2)) / 64;
      // offsetting right and down
      int16_t offset_x   = (rw / 2) - 64;
      int16_t offset_y   = (rh - 128) * rw / 2;
      // applying a screen palette
      color1 = *(sugi_memory + SUGI_MEM_PAL_SCREEN_PTR + color1);
      color2 = *(sugi_memory + SUGI_MEM_PAL_SCREEN_PTR + color2);
      // puting pixels twise to make a zoomed in effect for 64x64 screen
      // putting upper pixels
      *(sugi_draw_buffer_ptr + offset_x + offset_y + i * 4 + 0 - shift_back) = color1;
      *(sugi_draw_buffer_ptr + offset_x + offset_y + i * 4 + 1 - shift_back) = color1;
      *(sugi_draw_buffer_ptr + offset_x + offset_y + i * 4 + 2 - shift_back) = color2;
      *(sugi_draw_buffer_ptr + offset_x + offset_y + i * 4 + 3 - shift_back) = color2;
      // putting lower pixels
      offset_x += rw;
      *(sugi_draw_buffer_ptr + offset_x + offset_y + i * 4 + 0 - shift_back) = color1;
      *(sugi_draw_buffer_ptr + offset_x + offset_y + i * 4 + 1 - shift_back) = color1;
      *(sugi_draw_buffer_ptr + offset_x + offset_y + i * 4 + 2 - shift_back) = color2;
      *(sugi_draw_buffer_ptr + offset_x + offset_y + i * 4 + 3 - shift_back) = color2;
    }
  }
}


void sugi_render_mode_square_pico_internal(uint32_t rw, uint32_t rh)
{
  for (int i = 0; i < sugi_memory_screen_size; i++)
  {
    if (i % (rw / 2) < 64 && i < (rw / 2) * 128)
    {
      uint8_t color1   = *(sugi_memory_ptr + i) >> 4 & 0xf;
      uint8_t color2   = *(sugi_memory_ptr + i) & 0xf;
      int16_t offset_x = (rw - 128) / 2;
      int16_t offset_y = (rh - 128) * rw / 2;
      // applying a screen palette
      color1 = *(sugi_memory + SUGI_MEM_PAL_SCREEN_PTR + color1);
      color2 = *(sugi_memory + SUGI_MEM_PAL_SCREEN_PTR + color2);
      // putting pixels
      *(sugi_draw_buffer_ptr + offset_x + offset_y + i * 2 + 0) = color1;
      *(sugi_draw_buffer_ptr + offset_x + offset_y + i * 2 + 1) = color2;
    }
  }
}
#pragma endregion RENDER_MODES
