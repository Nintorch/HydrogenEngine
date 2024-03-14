#include "MD.h"
#include "SDL_image.h"

#include <stdio.h>

static SDL_Palette* default_palette;
static SDL_Palette* palette;

void MD_PaletteInit(void)
{
    default_palette = MD_CreateColorPalette();
    MD_ResetCurrentColorPalette();
    for (int i = 0; i < MD_PALETTE_COUNT; i++)
        MD_ClearPalette(NULL, i);
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

void MD_ResetCurrentColorPalette(void)
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

static SDL_Palette* get_palette(SDL_Palette* pal)
{
    return pal == NULL ? MD_GetColorPalette() : pal;
}

const SDL_Color* MD_GetPaletteColors(SDL_Palette* pal, int palid)
{
    return get_palette(pal)->colors + get_palette_offset(palid);
}

void MD_SetPaletteColors(SDL_Palette* pal, int palid, const SDL_Color* colors, int first_color, int ncolors)
{
    SDL_SetPaletteColors(get_palette(pal), colors, first_color + get_palette_offset(palid), ncolors);
}

void MD_FillPalette(SDL_Palette* pal, int palid, int r, int g, int b)
{
    SDL_Color out_pal[MD_PALETTE_COLORS];
    for (int i = 0; i < MD_PALETTE_COLORS; i++)
    {
        out_pal[i].r = r;
        out_pal[i].g = g;
        out_pal[i].b = b;
        out_pal[i].a = 255;
    }
    MD_SetPaletteColors(pal, palid, out_pal, 0, MD_PALETTE_COLORS);
}

void MD_ClearPalette(SDL_Palette* pal, int palid)
{
    MD_FillPalette(pal, palid, 0, 0, 0);
}

void MD_ConvertColor(Uint16 md_color, SDL_Color* out)
{
    const int multiplier = 16;
    out->r = (md_color & 0xF) / 0x1 * multiplier;
    out->g = (md_color & 0xF0) / 0x10 * multiplier;
    out->b = (md_color & 0xF00) / 0x100 * multiplier;
    out->a = 255;
}

void MD_LoadMDPaletteExRW(SDL_RWops* rw, SDL_bool free_rwops, int pal, SDL_Color* out, int ncolors)
{
    int n = SDL_min(ncolors, SDL_RWsize(rw) / 2);

    SDL_RWseek(rw, pal * MD_PALETTE_COLORS * 2, RW_SEEK_CUR);

    for (int i = 0; i < n; i++)
    {
        Uint16 color = SDL_ReadBE16(rw);
        MD_ConvertColor(color, out + i);
    }

    if (free_rwops)
        SDL_RWclose(rw);
}

void MD_LoadMDPaletteEx(const char* filename, int pal, SDL_Color* out, int ncolors)
{
    MD_LoadMDPaletteExRW(SDL_RWFromFile(filename, "rb"), SDL_TRUE, pal, out, ncolors);
}

void MD_LoadMDPaletteRW(SDL_Palette* pal, int palid, int npals, SDL_RWops* rw, SDL_bool free_rwops)
{
    SDL_Color colors[MD_PALETTE_COLORS];
    for (int i = 0; i < npals; i++)
    {
        MD_LoadMDPaletteExRW(rw, SDL_FALSE, i, colors, MD_PALETTE_COLORS);
        MD_SetPaletteColors(pal, palid + i, colors, 0, MD_PALETTE_COLORS);
    }
    if (free_rwops)
        SDL_RWclose(rw);
}

void MD_LoadMDPalette(SDL_Palette* pal, int palid, int npals, const char* filename)
{
    SDL_Color colors[MD_PALETTE_COLORS];
    for (int i = 0; i < npals; i++)
    {
        MD_LoadMDPaletteEx(filename, i, colors, MD_PALETTE_COLORS);
        MD_SetPaletteColors(pal, palid + i, colors, 0, MD_PALETTE_COLORS);
    }
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
    SDL_GetRGB(pixel, surface->format, &color.r, &color.g, &color.b);
    return color;
}

void MD_LoadPaletteExSurface(SDL_Surface* src, SDL_bool free_src, int pal, SDL_Color* out, int ncolors)
{
    if (src->format->BytesPerPixel < 3)
    {
        printf("MD_LoadPaletteExSurface: Unsupported pixel format");
        return;
    }

    int n = SDL_min(ncolors, src->w);

    for (int i = 0; i < n; i++)
        out[i] = get_color_from_surface(src, i, pal);

    if (free_src)
        SDL_FreeSurface(src);
}

void MD_LoadPaletteEx(const char* filename, int pal, SDL_Color* out, int ncolors)
{
    SDL_Surface* surface = IMG_Load(filename);
    if (surface == NULL)
    {
        printf("MD_LoadPaletteEx: Unable to load image: %s\n", IMG_GetError());
        return;
    }
    return MD_LoadPaletteExSurface(surface, SDL_TRUE, pal, out, ncolors);
}

void MD_LoadPaletteSurface(SDL_Palette* pal, int palid, int npals, SDL_Surface* src, SDL_bool free_src)
{
    SDL_Color colors[MD_PALETTE_COLORS];
    for (int i = 0; i < npals; i++)
    {
        MD_LoadPaletteExSurface(src, SDL_FALSE, i, colors, MD_PALETTE_COLORS);
        MD_SetPaletteColors(pal, palid + i, colors, 0, MD_PALETTE_COLORS);
    }
    if (free_src)
        SDL_FreeSurface(src);
}

void MD_LoadPalette(SDL_Palette* pal, int palid, int npals, const char* filename)
{
    SDL_Surface* surface = IMG_Load(filename);
    
    if (surface == NULL)
    {
        printf("MD_LoadPaletteEx: Unable to load image: %s\n", IMG_GetError());
        return;
    }

    MD_LoadPaletteSurface(pal, palid, npals, surface, SDL_TRUE);
}

void MD_SavePalette(SDL_Palette* pal, int palid, const char* filename)
{
    SDL_Surface* surface = SDL_CreateRGBSurface(0, MD_PALETTE_COLORS, 1, 32, 0, 0, 0, 0);

    for (int i = 0; i < surface->w; i++)
    {
        SDL_Color color = *(MD_GetPaletteColors(pal, palid) + i);
        Uint32 pixel = SDL_MapRGB(surface->format, color.r, color.g, color.b);
        *(Uint32*)((char*)surface->pixels + i * surface->format->BytesPerPixel) = pixel;
    }

    if (!strcmp(filename + strlen(filename) - 4, ".png"))
        IMG_SavePNG(surface, filename);
    else
        SDL_SaveBMP(surface, filename);
}

void MD_CopyPalette(SDL_Palette* srcpal, int srcpalid, SDL_Palette* dstpal, int dstpalid)
{
    const SDL_Color* srccolors = MD_GetPaletteColors(srcpal, srcpalid);
    MD_SetPaletteColors(dstpal, dstpalid, srccolors, 0, MD_PALETTE_COLORS);
}