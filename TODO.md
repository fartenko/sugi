# Next Steps
- Map the RAM for sptiresheet, map, etc.
- Implement spr and sspr (sspr mainly for testing)
- Implement cart file structure
- Implement cart file loading and saving
  * Cart file loads data into RAM
  * After RAM edit, it saves back to RAM file

# Core
- ~~Move render related stuff to `sugi_render.c`~~
- Implement all gfx functions (`sugi_gfx`)
  * ~~`sugi_gfx_pget`~~   (Gets a pixel color)
  * ~~`sugi_gfx_pset`~~   (Sets a pixel)
  * ~~`sugi_gfx_clear`~~  (Clears the screen)
  * ~~`sugi_gfx_color`~~  (Sets a current color)
  * ~~`sugi_gfx_line`~~   (Draws a line)
  * ~~`sugi_gfx_rect`~~   (Draws a rectangle)
  * ~~`sugi_gfx_circ`~~   (Draws a circle)
  * ~~`sugi_gfx_camera`~~ (Moves a "camera")
  * ~~`sugi_gfx_clip`~~   (Clips a render region)
  * ~~`sugi_gfx_pal`~~    (Palette swap function)
  * ~~`sugi_gfx_spr`~~    (Draws a sprite)
  * `sugi_gfx_sspr`       (Draws a part of a spritesheet)
  * ~~`sugi_gfx_palt`~~   (Palette transparency for sprites)
  * `sugi_gfx_print`      (Prints a text)
  * `sugi_gfx_fillp`      (Sets a fill pattern mask (16bit))
  * ~~`sugi_gfx_sget`~~   (Gets a pixel from a spritesheet)
  * ~~`sugi_gfx_sset`~~   (Sets a pixel in a spritesheet)
  * `sugi_gfx_cursor`     (Sets position of a print command (16bit))
  * `sugi_gfx_flip`   ??? (Flips a screen and a render buffer )
  * `sugi_gfx_tri`    ??? (Draws a triangle)
- Implement map
- Implement control system (default control, text input, gamepad support)
- Implement audio system (play simple mp3 or wav, mixers, sythesize sound)

# Fantasy console
- Terminal emulator
- Text editor
- Carts
