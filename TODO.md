# TODO

## Core
- DONE! ~~Move render related stuff to `sugi_render.c`~~
- Implement all gfx functions (`sugi_gfx.c`)
    - DONE! `sugi_gfx_pget`   (Gets a pixel color)
    - DONE! `sugi_gfx_pset`   (Sets a pixel)
    - DONE! `sugi_gfx_clear`  (Clears the screen)
    - DONE! `sugi_gfx_color`  (Sets a current color)
    - DONE! `sugi_gfx_line`   (Draws a line)
    - DONE! `sugi_gfx_rect`   (Draws a rectangle)
    - DONE! `sugi_gfx_circ`   (Draws a circle)
    - DONE! `sugi_gfx_camera` (Moves a "camera")
    - DONE! `sugi_gfx_clip`   (Clips a render region)
    - DONE! `sugi_gfx_pal`    (Palette swap function)
    - DONE! `sugi_gfx_palt`   (Palette transparency for sprites)
    - DONE! `sugi_gfx_spr`    (Draws a sprite)
    - DONE! `sugi_gfx_sspr`   (Draws a part of a spritesheet)
    - DONE! `sugi_gfx_print`  (Prints a text)
    - DONE! `sugi_gfx_sget`   (Gets a pixel from a spritesheet)
    - DONE! `sugi_gfx_sset`   (Sets a pixel in a spritesheet)
    - DONE! `sugi_gfx_flip`   (Flips a screen and a render buffer )
    - `sugi_gfx_fillp`        (up next)(Sets a fill pattern mask (16bit))
    - `sugi_gfx_tri`          (up next)(Draws a triangle)
    - ~~`sugi_gfx_cursor`~~   (under a question)(Sets position of a print command (16bit))

- DONE! Implement control system (default control, text input, gamepad support)
- State machine.
- Add ability to load spritesheet from png image.
- Implement map
- Implement audio system (play simple mp3 or wav, mixers, sythesize sound)

## Fantasy console
- Terminal emulator
- Text editor
- Carts

