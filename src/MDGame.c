#include "MD.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL2_gfxPrimitives.h"

#include "game/Objects.h"

#include <stdio.h>

SDL_Palette* underwater;
MD_Object* sonic;
SDL_Surface* test;

void hblank(int y)
{
    if (y == 140)
        MD_SetColorPalette(underwater);
}

void GameInit(void)
{
    MD_SetHBlank(hblank);
    
    MD_LoadPalette(0, "SonicPal.bin");

    underwater = MD_CreateColorPalette();
    MD_SetColorPalette(underwater);
    MD_LoadPalette(0, "Sonic - LZ Underwater.bin");
    MD_ResetColorPalette();

    test = MD_LoadSurface("result.png", 0, 0);
    sonic = CreateSonicObject();
}

void GameUpdate(void)
{
    MD_ObjectSystemUpdate();
}

int timer;
int hdeform[64];

void GameFramebufferRender(SDL_Surface* framebuffer)
{
    MD_ObjectSystemRender();

    for (int i = 0; i < 64; i++)
    {
        hdeform[i] = sin(i * M_PI / 16 + timer / 6) * 2;
    }

    timer++;
    MD_RenderSurfaceDeform(test, 0, 0, hdeform);
}

void GameTextureRender(SDL_Surface* rgbframebuffer)
{

}

void GameQuit(void)
{
    SDL_FreePalette(underwater);
}