#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "sugi.h"


void sugi_map_draw(uint8_t ox, uint8_t oy)
{
    // get sprite numbers
    // draw all of the sprites
    // .. via sugi_gfx_spr()

    // room size 20x16
    uint8_t rx = 20; // room x
    uint8_t ry = 16; // room y

    for (uint8_t x = 0; x < rx; x++)
    {
        for (uint8_t y = 0; y < ry; y++)
        {
            uint8_t off = ox % (rx * 8) + x % rx + oy * rx + y * rx;
            uint8_t s = *(sugi_memory_ptr + SUGI_MEM_MAPSHEET_PTR + off);

            if (s == 0)
                continue;

            int32_t sx = x * 8;
            int32_t sy = y * 8;

            sugi_gfx_spr(s, sx, sy);
        }
    }
}
