#include "MD.h"
#include "MDUtils.h"
#include "SDL.h"
#include "SDL_image.h"

#include "game/Objects.h"

#include <math.h>
#include <stdio.h>

SDL_Palette* underwater;
MD_Object* sonic;
SDL_Surface* test;

void hblank(int y)
{
    if (y == 140)
        MD_SetColorPalette(underwater);
}

int timer;
int hdeform0[240];
int hdeform[240];

void GameInit(void)
{
    MD_SetHBlank(hblank);

    MD_LoadPalette(NULL, 0, 1, "data/sonicpal.png");
    MD_LoadPalette(NULL, 1, 1, "data/ghzpal.png");

    underwater = MD_CreateColorPalette();
    MD_LoadMDPalette(underwater, 0, 1, "data/Sonic - LZ Underwater.bin");

    SDL_Color buffer[MD_PALETTE_COLORS];
    MD_CopyPaletteOut(NULL, 1, buffer, MD_PALETTE_COLORS);
    for (int i = 0; i < MD_PALETTE_COLORS; i++)
    {
        buffer[i].r /= 2;
        buffer[i].g /= 2;
        buffer[i].b /= 2;
    }
    MD_SetPaletteColors(underwater, 1, buffer, 0, MD_PALETTE_COLORS);

    test = MD_LoadSurface("data/ghz.png", 1, 1);
    sonic = CreateSonicObject();

    for (int i = 0; i < 240; i++)
    {
        hdeform0[i] = round(sin(i * M_PI / 64) * 2);
    }
}

void GameUpdate(void)
{
    MD_ObjectSystemUpdate();
}

void GameFramebufferRender(SDL_Surface* framebuffer)
{
    for (int i = 0; i < 240; i++)
    {
        hdeform[i] = i < 140 ? 0 : hdeform0[MD_UtilsOffset(i, 240, timer / 4)];
    }
    timer++;
    MD_RenderSurfaceDeform2(test, 0, 0, hdeform, 240);
    MD_ObjectSystemRender();
}

void GameTextureRender(SDL_Surface* rgbframebuffer)
{

}

void GameQuit(void)
{
    SDL_FreeSurface(test);
    SDL_FreePalette(underwater);
}