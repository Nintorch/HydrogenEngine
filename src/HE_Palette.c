#include "HydrogenEngine.h"
#include "SDL_image.h"

#include <stdio.h>

static SDL_Palette* default_palette;
static SDL_Palette* palette;

void HE_PaletteInit(void)
{
    default_palette = HE_CreateColorPalette();
    HE_ResetCurrentColorPalette();
    for (int i = 0; i < HE_PALETTE_COUNT; i++)
        HE_ClearPalette(NULL, i);
}

void HE_PaletteQuit(void)
{
    SDL_FreePalette(default_palette);
}

SDL_Palette* HE_GetColorPalette(void)
{
    return palette;
}

SDL_Palette* HE_GetDefaultColorPalette(void)
{
    return default_palette;
}

void HE_SetColorPalette(SDL_Palette* pal)
{
    palette = pal == NULL ? default_palette : pal;
}

void HE_ResetCurrentColorPalette(void)
{
    HE_SetColorPalette(NULL);
}

SDL_Palette* HE_CreateColorPalette(void)
{
    return SDL_AllocPalette(256);
}

static Uint8 get_palette_offset(int palid)
{
    if (palid < 0 || palid >= HE_PALETTE_COUNT)
        return -1;
    return HE_PALETTE_COLORS * palid;
}

Uint8 HE_MapColorInternal(int palid, int colorid)
{
    return get_palette_offset(palid) + colorid;
}

Uint8 HE_MapColor(int palid, int colorid)
{
    return colorid == 0 ? 0 : HE_MapColorInternal(palid, colorid);
}

static SDL_Palette* get_palette(SDL_Palette* pal)
{
    return pal == NULL ? HE_GetColorPalette() : pal;
}

const SDL_Color* HE_GetPaletteColors(SDL_Palette* pal, int palid)
{
    return get_palette(pal)->colors + get_palette_offset(palid);
}

void HE_SetPaletteColors(SDL_Palette* pal, int palid, const SDL_Color* colors, int first_color, int ncolors)
{
    SDL_SetPaletteColors(get_palette(pal), colors, first_color + get_palette_offset(palid), ncolors);
}

void HE_FillPalette(SDL_Palette* pal, int palid, int r, int g, int b)
{
    SDL_Color out_pal[HE_PALETTE_COLORS];
    for (int i = 0; i < HE_PALETTE_COLORS; i++)
    {
        out_pal[i].r = r;
        out_pal[i].g = g;
        out_pal[i].b = b;
        out_pal[i].a = 255;
    }
    HE_SetPaletteColors(pal, palid, out_pal, 0, HE_PALETTE_COLORS);
}

void HE_ClearPalette(SDL_Palette* pal, int palid)
{
    HE_FillPalette(pal, palid, 0, 0, 0);
}

void HE_ConvertColor(Uint16 HE_color, SDL_Color* out)
{
    const int multiplier = 16;
    out->r = (HE_color & 0xF) / 0x1 * multiplier;
    out->g = (HE_color & 0xF0) / 0x10 * multiplier;
    out->b = (HE_color & 0xF00) / 0x100 * multiplier;
    out->a = 255;
}

void HE_LoadPaletteMDRWOut(SDL_RWops* rw, SDL_bool free_rwops, SDL_Color* out, int ncolors)
{
    int rwcount = (SDL_RWsize(rw) - SDL_RWtell(rw)) / 2;
    int n = SDL_min(ncolors, rwcount);

    for (int i = 0; i < n; i++)
    {
        Uint16 color = SDL_ReadBE16(rw);
        HE_ConvertColor(color, out + i);
    }

    if (free_rwops)
        SDL_RWclose(rw);
}

void HE_LoadPaletteMDOut(const char* filename, int pal, SDL_Color* out, int ncolors)
{
    SDL_RWops* rw = SDL_RWFromFile(filename, "rb");
    SDL_RWseek(rw, pal * HE_PALETTE_COLORS * 2, RW_SEEK_SET);
    HE_LoadPaletteMDRWOut(rw, SDL_TRUE, out, ncolors);
}

void HE_LoadMDPaletteRW(SDL_Palette* pal, int palid, int npals, SDL_RWops* rw, SDL_bool free_rwops)
{
    SDL_Color colors[HE_PALETTE_COLORS];
    for (int i = 0; i < npals; i++)
    {
        HE_LoadPaletteMDRWOut(rw, SDL_FALSE, colors, HE_PALETTE_COLORS);
        HE_SetPaletteColors(pal, palid + i, colors, 0, HE_PALETTE_COLORS);
    }
    if (free_rwops)
        SDL_RWclose(rw);
}

void HE_LoadPaletteMD(SDL_Palette* pal, int palid, int npals, const char* filename)
{
    SDL_Color colors[HE_PALETTE_COLORS];
    SDL_RWops* rw = SDL_RWFromFile(filename, "rb");
    for (int i = 0; i < npals; i++)
    {
        HE_LoadPaletteMDRWOut(rw, SDL_FALSE, colors, HE_PALETTE_COLORS);
        HE_SetPaletteColors(pal, palid + i, colors, 0, HE_PALETTE_COLORS);
    }
    SDL_RWclose(rw);
}

static SDL_Color get_color_from_surface(SDL_Surface* surface, int x, int y)
{
    Uint32 pixel;
    if (surface->format->BytesPerPixel == 4)
    {
        pixel = *(Uint32*)((char*)surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel);
    }
    else if (surface->format->BytesPerPixel == 3)
    {
        Uint8* p = (Uint8*)((char*)surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel);
        pixel = SDL_MapRGB(surface->format, p[0], p[1], p[2]);
    }
    SDL_Color color;
    SDL_GetRGBA(pixel, surface->format, &color.r, &color.g, &color.b, &color.a);
    return color;
}

void HE_LoadPaletteSurfaceOut(SDL_Surface* src, SDL_bool free_src, int pal, SDL_Color* out, int ncolors)
{
    if (src->format->BytesPerPixel < 3)
    {
        printf("HE_LoadPaletteSurfaceOut: Unsupported pixel format");
        return;
    }

    int n = SDL_min(ncolors, src->w);

    for (int i = 0; i < n; i++)
        out[i] = get_color_from_surface(src, i, pal);

    if (free_src)
        SDL_FreeSurface(src);
}

void HE_LoadPalette(SDL_Palette* pal, int palid, int npals, const char* filename)
{
    SDL_Color colors[HE_PALETTE_COLORS];
    SDL_Surface* surface = IMG_Load(filename);
    if (surface == NULL)
    {
        printf("HE_LoadPalette: Unable to load image: %s\n", IMG_GetError());
        return;
    }

    for (int i = 0; i < npals; i++)
    {
        HE_LoadPaletteSurfaceOut(surface, SDL_FALSE, i, colors, HE_PALETTE_COLORS);
        HE_SetPaletteColors(pal, palid + i, colors, 0, HE_PALETTE_COLORS);
    }

    SDL_FreeSurface(surface);
}

void HE_LoadPaletteOut(const char* filename, int pal, SDL_Color* out, int ncolors)
{
    SDL_Surface* surface = IMG_Load(filename);
    if (surface == NULL)
    {
        printf("HE_LoadPaletteOut: Unable to load image: %s\n", IMG_GetError());
        return;
    }
    int n = SDL_min(ncolors, surface->w);
    for (int i = 0; i < n; i++)
    {
        out[i] = get_color_from_surface(surface, i, pal);
    }
    SDL_FreeSurface(surface);
}

void HE_SavePalette(SDL_Palette* pal, int palid, const char* filename)
{
    SDL_Surface* surface = SDL_CreateRGBSurface(0, HE_PALETTE_COLORS, 1, 32, 0, 0, 0, 0);

    for (int i = 0; i < surface->w; i++)
    {
        SDL_Color color = *(HE_GetPaletteColors(pal, palid) + i);
        Uint32 pixel = SDL_MapRGB(surface->format, color.r, color.g, color.b);
        *(Uint32*)((char*)surface->pixels + i * surface->format->BytesPerPixel) = pixel;
    }

    HE_SaveSurface(surface, filename);
    SDL_FreeSurface(surface);
}

void HE_CopyPalette(SDL_Palette* srcpal, int srcpalid, SDL_Palette* dstpal, int dstpalid)
{
    const SDL_Color* srccolors = HE_GetPaletteColors(srcpal, srcpalid);
    HE_SetPaletteColors(dstpal, dstpalid, srccolors, 0, HE_PALETTE_COLORS);
}

void HE_CopyPaletteOut(SDL_Palette* srcpal, int srcpalid, SDL_Color* out, int ncolors)
{
    int n = SDL_min(ncolors, HE_PALETTE_COLORS);
    const SDL_Color* colors = HE_GetPaletteColors(srcpal, srcpalid);
    for (int i = 0; i < n; i++)
    {
        out[i] = colors[i];
    }
}