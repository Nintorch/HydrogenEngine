#include "MD.h"

static SDL_Palette* default_palette;
static SDL_Palette* palette;

void MD_PaletteInit(void)
{
    default_palette = MD_CreateColorPalette();
    MD_ResetColorPalette();
    for (int i = 0; i < MD_PALETTE_COUNT; i++)
        MD_ClearPalette(i);
}

void MD_PaletteQuit(void)
{
    SDL_FreePalette(default_palette);
}

SDL_Palette* MD_GetColorPalette(void)
{
    return palette;
}

SDL_Palette* MD_GetDefaultColorPalette(void)
{
    return default_palette;
}

void MD_SetColorPalette(SDL_Palette* pal)
{
    palette = pal;
}

void MD_ResetColorPalette(void)
{
    MD_SetColorPalette(default_palette);
}

SDL_Palette* MD_CreateColorPalette(void)
{
    return SDL_AllocPalette(256);
}

static Uint8 get_palette_offset(int palid)
{
    if (palid < 0 || palid >= MD_PALETTE_COUNT)
        return -1;
    return MD_PALETTE_COLORS * palid;
}

Uint8 MD_MapColorInternal(int palid, int colorid)
{
    return get_palette_offset(palid) + colorid;
}

Uint8 MD_MapColor(int palid, int colorid)
{
    return colorid == 0 ? 0 : MD_MapColorInternal(palid, colorid);
}

const SDL_Color* MD_GetPaletteColors(int palid)
{
    return palette->colors + get_palette_offset(palid);
}

void MD_SetPaletteColors(int palid, const SDL_Color* colors, int first_color, int ncolors)
{
    SDL_SetPaletteColors(MD_GetColorPalette(), colors, first_color + get_palette_offset(palid), ncolors);
}

void MD_FillPalette(int palid, int r, int g, int b)
{
    SDL_Color pal[MD_PALETTE_COLORS];
    for (int i = 0; i < MD_PALETTE_COLORS; i++)
    {
        pal[i].r = r;
        pal[i].g = g;
        pal[i].b = b;
        pal[i].a = 255;
    }
    MD_SetPaletteColors(palid, pal, 0, MD_PALETTE_COLORS);
}

void MD_ClearPalette(int palid)
{
    MD_FillPalette(palid, 0, 0, 0);
}

void MD_ConvertColor(Uint16 md_color, SDL_Color* out)
{
    const int multiplier = 16;
    out->r = (md_color & 0xF) / 0x1 * multiplier;
    out->g = (md_color & 0xF0) / 0x10 * multiplier;
    out->b = (md_color & 0xF00) / 0x100 * multiplier;
    out->a = 255;
}

void MD_LoadPaletteRW(int palid, SDL_RWops* rw, SDL_bool free_rwops)
{
    SDL_Color colors[MD_PALETTE_COLORS] = {};

    int ncolors = SDL_min(MD_PALETTE_COLORS, SDL_RWsize(rw) / 2);
    SDL_RWseek(rw, 0, RW_SEEK_SET);
    for (int i = 0; i < ncolors; i++)
    {
        Uint16 color = SDL_ReadBE16(rw);
        MD_ConvertColor(color, colors + i);
    }

    if (free_rwops)
        SDL_RWclose(rw);

    MD_SetPaletteColors(palid, colors, 0, MD_PALETTE_COLORS);
}

void MD_LoadPalette(int palid, const char* filename)
{
    MD_LoadPaletteRW(palid, SDL_RWFromFile(filename, "rb"), SDL_TRUE);
}