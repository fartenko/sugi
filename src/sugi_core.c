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

void sugi_core_null_fn (void) {}

void sugi_core_run(void)
{
    uint8_t  sugi_core_quit    = 0;
    uint32_t sugi_time_step_ms = 1000 / 60;
    uint32_t sugi_delta_ticks  = 0;

    printf("MEMORY TABLE\n");
    printf("\tSTART\tEND\n");
    printf("VRAM:\t0x%04x\t0x%04x\n", SUGI_MEM_SCREEN_PTR, SUGI_MEM_SCREEN_PTR + SUGI_MEM_SCREEN_SIZE - 1);
    printf("STAT:\t0x%04x\t0x%04x\n", SUGI_MEM_SCREEN_PTR + SUGI_MEM_SCREEN_SIZE, SUGI_MEM_SPRSHEET_PTR - 1);
    printf("SPRM:\t0x%04x\t0x%04x\n", SUGI_MEM_SPRSHEET_PTR, SUGI_MEM_SPRSHEET_PTR + SUGI_MEM_SPRSHEET_SIZE - 1);
    printf("MAPM:\t0x%04x\t0x%04x\n", SUGI_MEM_MAPSHEET_PTR, SUGI_MEM_MAPSHEET_PTR + SUGI_MEM_MAPSHEET_SIZE - 1);
    printf("Memory: %d bytes\n", SUGI_MEM_MAPSHEET_PTR + SUGI_MEM_MAPSHEET_SIZE);

    #pragma region INIT_SDL_GL
    // Initializing SDL
    sugi_delta_ticks = 0;
    SDL_Init(SDL_INIT_EVERYTHING);
    // Setting up OpenGL Attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    // Setting Vsync
    SDL_GL_SetSwapInterval(SUGI_USE_VSYNC);
    // Setting OpenGL Attributes
    int32_t gl_major_version = 0;
    int32_t gl_minor_version = 0;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &gl_major_version);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &gl_minor_version);
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
    // Initializing GLEW
    glewInit();
    // Print some information
    printf("VSync: %d\n", SUGI_USE_VSYNC);
    printf("GL Context version: %d.%d\n", gl_major_version, gl_minor_version);
    printf("OpenGL version: %s\n", glGetString(GL_VERSION));
    /* Clearing GL context */
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    // Drawing first frame
    SDL_GL_SwapWindow(sugi_main_window);
    SDL_ShowCursor(0);
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
        // Clearing btnp buffer 
        sugi_input_clear_btnp_internal();
        while (SDL_PollEvent(&sugi_sdl_event)) {
            // Getting a keyboard state
            const uint8_t * kb_state = SDL_GetKeyboardState(NULL);
            // Processing events
            switch (sugi_sdl_event.type) {
                // Quit program event
                case SDL_QUIT:
                {
                    sugi_core_quit = 1;
                    printf("Quit!\n\r");
                    break;
                }
                // Text events
                case SDL_TEXTINPUT:
                {
                    if (sugi_text_field_struct.active != SUGI_TEXTMODE_OFF)
                        sugi_text_insert_str(sugi_sdl_event.text.text);
                    break;
                }
                // Keypresses events
                case SDL_KEYDOWN:
                {
                    sugi_input_process_kb_press_state(kb_state);

                    if (sugi_text_field_struct.active != SUGI_TEXTMODE_OFF)
                    {
                        if (kb_state[SDL_SCANCODE_BACKSPACE])
                            sugi_text_backspace_char();
                        
                        if (kb_state[SDL_SCANCODE_DELETE])
                            sugi_text_delete_char();

                        if (kb_state[SDL_SCANCODE_RETURN])
                            sugi_text_insert_str("\n");

                        if (kb_state[SDL_SCANCODE_LEFT])
                            sugi_text_move_cursor(-1);

                        if (kb_state[SDL_SCANCODE_RIGHT])
                            sugi_text_move_cursor(1);
                    }
                    break;
                }
                case SDL_KEYUP:
                {
                    sugi_input_process_kb_release_state(kb_state);
                    break;
                }
                // Mouse events
                case SDL_MOUSEWHEEL:
                {
                    uint32_t y = sugi_sdl_event.wheel.y;
                    printf("SDL_MOUSEWHEEL: y:%d\n\r", y);
                    break;
                }
                case SDL_MOUSEBUTTONDOWN:
                {
                    uint8_t b = sugi_sdl_event.button.button;
                    printf("SDL_MOUSEBUTTONDOWN: b:%d\n\r", b);
                    break;
                }
                case SDL_MOUSEBUTTONUP:
                {
                    uint8_t b = sugi_sdl_event.button.button;
                    printf("SDL_MOUSEBUTTONUP: b:%d\n\r", b);
                    break;
                }
                case SDL_MOUSEMOTION:
                {
                    int32_t x = sugi_sdl_event.motion.x;
                    int32_t y = sugi_sdl_event.motion.y;
                    int32_t rx = (x - sugi_window_offset_x) / sugi_window_zoom;
                    int32_t ry = (y - sugi_window_offset_y) / sugi_window_zoom;

                    uint8_t x_high = (rx >> 8) & 0xff;
                    uint8_t x_low = rx & 0xff;
                    if (x_high >= 128) 
                    {
                        x_high = 0;
                        x_low = 0;
                    }
                    uint8_t y_high = (ry >> 8) & 0xff;
                    uint8_t y_low = ry & 0xff;
                    if (y_high >= 128) 
                    {
                        y_high = 0;
                        y_low = 0;
                    }

                    *(sugi_memory + SUGI_MEM_CURSOR_LOW_PTR + 0) = rx & 0xff;
                    *(sugi_memory + SUGI_MEM_CURSOR_LOW_PTR + 1) = ry & 0xff;
                    *(sugi_memory + SUGI_MEM_CURSOR_LOW_PTR + 0) = (rx >> 8) & 0xff;
                    *(sugi_memory + SUGI_MEM_CURSOR_LOW_PTR + 1) = (ry >> 8) & 0xff;

                    break;
                }
                // Joystick events
                case SDL_JOYDEVICEADDED:
                {
                    // TODO: move all of this in a separate function
                    // like sugi_input_add_joystick(event);
                    int j_which = sugi_sdl_event.jdevice.which;
                    int j_n = SDL_NumJoysticks(); 
                    printf("SDL_JOYDEVICEADDED which:%d\n\r", j_which);

                    if (j_n > 0 && j_n <= SUGI_MAX_JOYSTICKS)
                    {
                        for (int i = 0; i < SUGI_MAX_JOYSTICKS; i++)
                        {
                            if (!sugi_joysticks_opened[i])
                            {
                                // Open a joystick 
                                sugi_joysticks_opened[i] = 1;
                                sugi_joysticks[i] = SDL_JoystickOpen(j_which);
                                sugi_gamecontrollers[i] = SDL_GameControllerOpen(j_which);
                                sugi_joysticks_id[i] = SDL_JoystickInstanceID(sugi_joysticks[i]);
                                printf("Joystick opened!\n\r");
                                printf("j_opened[i]:\t%d\n\r", i);
                                printf("j_id:\t\t%d\n\r", sugi_joysticks_id[i]);

                                break; // break out of a for loop!
                            }
                            else
                            {
                                printf("Joystick index %d is occupied!\n\r", i);
                            }
                        }
                    }
                    else 
                    {
                        printf("Max number of joysticks is %d\n\n", SUGI_MAX_JOYSTICKS);
                    }
                    break;
                }
                case SDL_JOYDEVICEREMOVED:
                {
                    // TODO: move all of this in a separate function
                    // like sugi_input_remove_joystick(event);
                    int j_id = sugi_sdl_event.jdevice.which;
                    printf("SDL_JOYDEVICEREMOVED which:%d\n\r", j_id);

                    int i = 0;
                    int found = 0;
                    for (i = 0; i < SUGI_MAX_JOYSTICKS; i++)
                    {
                        if (j_id == sugi_joysticks_id[i])
                        {
                            found = 1;
                            break; // break out of a for loop
                        }
                    }

                    if (found)
                    {
                        sugi_joysticks_opened[i] = 0;
                        sugi_joysticks_id[i] = 0;
                        SDL_JoystickClose(sugi_joysticks[i]);
                        SDL_GameControllerClose(sugi_gamecontrollers[i]);
                        printf("Joystick closed!\n\r");
                        printf("j_opened[i]:\t%d\n\r", i);
                        printf("j_id:\t\t%d\n\r", j_id);
                    }
                    else
                    {
                        printf("Could not find joystick with id %d\n\r", j_id);
                    }
                    break;
                }
                case SDL_CONTROLLERDEVICEADDED:
                {
                    printf("SDL_CONTROLLERDEVICEADDED which:%d\n", sugi_sdl_event.cdevice.which);
                    break;
                }
                case SDL_CONTROLLERDEVICEREMOVED:
                {
                    printf("SDL_CONTROLLERDEVICEREMOVED which:%d\n",  sugi_sdl_event.cdevice.which);
                    break;
                }
                case SDL_CONTROLLERBUTTONDOWN:
                {
                    // TODO: move all of this in a separate function
                    // like sugi_input_controller_button_down(event);
                    int j_id = sugi_sdl_event.cdevice.which;
                    int button = sugi_sdl_event.cbutton.button;
                    int i = 0;
                    int found = 0;
                    for (i = 0; i < SUGI_MAX_JOYSTICKS; i++)
                    {
                        if (j_id == sugi_joysticks_id[i])
                        {
                            found = 1;
                            break;
                        }
                    } 

                    if (found)
                    {
                        printf("Controller button down! (j_id: %d, btn: %d, plr: %d, str: %s)\n\r",
                            j_id, button, i, SDL_GameControllerGetStringForButton(button));
                        sugi_input_process_controller_press_button(button, i);
                    }
                    break;
                }
                case SDL_CONTROLLERBUTTONUP:
                {
                    // TODO: move all of this in a separate function
                    // like sugi_input_controller_button_up(event);
                    int j_id = sugi_sdl_event.cdevice.which;
                    int button = sugi_sdl_event.cbutton.button;
                    int i = 0;
                    int found = 0;
                    for (i = 0; i < SUGI_MAX_JOYSTICKS; i++)
                    {
                        if (j_id == sugi_joysticks_id[i])
                        {
                            found = 1;
                            break;
                        }
                    } 

                    if (found)
                    {
                        printf("Controller button up! (j_id: %d, btn: %d, plr: %d, str: %s)\n\r",
                            j_id, button, i, SDL_GameControllerGetStringForButton(button));
                        sugi_input_process_controller_release_button(button, i);
                    }
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
        // TODO: Add game input polling here
        // Calls an ingame update and draw functoins
        // TODO: Replace sugi_call_update and sugi_call_draw
        //       to sugi_call_tick_fn_internal()
        //       because it does the same thing...
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
    SDL_GL_DeleteContext(sugi_main_gl_context);
    SDL_DestroyWindow(sugi_main_window);
    SDL_Quit();
    #pragma endregion DEINIT_SDL_GL
}
