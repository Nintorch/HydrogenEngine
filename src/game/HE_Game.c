#include "HydrogenEngine.h"
#include "HE_Utils.h"
#include "SDL.h"
#include "SDL_image.h"

#include "game/Objects.h"

#include <math.h>
#include <stdio.h>

SDL_Palette* underwater;
HE_Object* sonic;
SDL_Surface *test, *test2;

void hblank(int y)
{
    if (y == 140)
        HE_SetColorPalette(underwater);
}

int timer;
int hdeform0[240];
int hdeform[240];

void GameInit(void)
{
    HE_SetHBlank(hblank);

    // HE_LoadPalette(NULL, 0, 1, "data/sonicpal.png");
    HE_LoadPaletteMD(NULL, 0, 1, "data/SonicPal.bin");
    HE_LoadPalette(NULL, 1, 1, "data/ghzpal.png");

    underwater = HE_CreateColorPalette();
    HE_LoadPaletteMD(underwater, 0, 1, "data/Sonic - LZ Underwater.bin");

    SDL_Color buffer[HE_PALETTE_COLORS];
    HE_CopyPaletteOut(NULL, 1, buffer, HE_PALETTE_COLORS);
    for (int i = 0; i < HE_PALETTE_COLORS; i++)
    {
        buffer[i].r /= 2;
        buffer[i].g /= 2;
        buffer[i].b /= 2;
    }
    HE_SetPaletteColors(underwater, 1, buffer, 0, HE_PALETTE_COLORS);

    test = HE_LoadSurface("data/ghz.png", 1, 1);
    test2 = IMG_Load("data/test.png");
    sonic = CreateSonicObject();

    for (int i = 0; i < 240; i++)
    {
        hdeform0[i] = round(sin(i * M_PI / 64) * 2);
    }
}

void GameUpdate(void)
{
    HE_ObjectSystemUpdate();
}

void GameRender(void)
{
    for (int i = 0; i < 240; i++)
    {
        hdeform[i] = i < 140 ? 0 : hdeform0[HE_UtilsOffset(i, 240, timer / 4)];
    }
    timer++;
    HE_RenderSurfaceDeform(test, 0, 0, hdeform, 240, SDL_FALSE);
    HE_RenderSurfaceDeform(test, 256, 0, hdeform, 240, SDL_FALSE);
    HE_RenderSDLSurface(test2, 50, 100);
    HE_ObjectSystemRender();
}

void GameQuit(void)
{
    SDL_FreeSurface(test2);
    SDL_FreeSurface(test);
    SDL_FreePalette(underwater);
}