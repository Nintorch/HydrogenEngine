#include "MD.h"
#include "MDUtils.h"
#include "SDL.h"
#include "SDL_image.h"

#include "game/Objects.h"

#include <math.h>
#include <stdio.h>

SDL_Palette* underwater;
MD_Object* sonic;
SDL_Surface *test, *test2;

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

    // MD_LoadPalette(NULL, 0, 1, "data/sonicpal.png");
    MD_LoadPaletteMD(NULL, 0, 1, "data/SonicPal.bin");
    MD_LoadPalette(NULL, 1, 1, "data/ghzpal.png");

    underwater = MD_CreateColorPalette();
    MD_LoadPaletteMD(underwater, 0, 1, "data/Sonic - LZ Underwater.bin");

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
    test2 = IMG_Load("data/test.png");
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

void GameRender(void)
{
    for (int i = 0; i < 240; i++)
    {
        hdeform[i] = i < 140 ? 0 : hdeform0[MD_UtilsOffset(i, 240, timer / 4)];
    }
    timer++;
    MD_RenderSurfaceDeform(test, 0, 0, hdeform, 240, SDL_FALSE);
    MD_RenderSurfaceDeform(test, 256, 0, hdeform, 240, SDL_FALSE);
    MD_RenderSDLSurface(test2, 50, 100);
    MD_ObjectSystemRender();
}

void GameQuit(void)
{
    SDL_FreeSurface(test2);
    SDL_FreeSurface(test);
    SDL_FreePalette(underwater);
}