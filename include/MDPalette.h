#ifndef __MDPALETTE_H__
#define __MDPALETTE_H__

#include "SDL_surface.h"
#include "SDL_pixels.h"
#include "SDL_rwops.h"

#define MD_PALETTE_COLORS   16
#define MD_PALETTE_COUNT    (int)(256 / MD_PALETTE_COLORS)

void MD_PaletteInit(void);
void MD_PaletteQuit(void);

SDL_Palette* MD_GetColorPalette(void);
SDL_Palette* MD_GetDefaultColorPalette(void);
void MD_SetColorPalette(SDL_Palette* pal);
void MD_ResetCurrentColorPalette(void);

SDL_Palette* MD_CreateColorPalette(void);

Uint8 MD_MapColor(int palid, int colorid);
Uint8 MD_MapColorInternal(int palid, int colorid);

// Pass NULL if you want to use the current color palette
const SDL_Color* MD_GetPaletteColors(SDL_Palette* pal, int palid);
void MD_SetPaletteColors(SDL_Palette* pal, int palid, const SDL_Color* colors, int first_color, int ncolors);
void MD_FillPalette(SDL_Palette* pal, int palid, int r, int g, int b);
void MD_ClearPalette(SDL_Palette* pal, int palid);

void MD_ConvertColor(Uint16 md_color, SDL_Color* out);

// Load a palette in Mega Drive format
void MD_LoadMDPalette(SDL_Palette* pal, int palid, const char* filename);
// Load a palette from an image file
void MD_LoadPalette(SDL_Palette* pal, int palid, const char* filename);

void MD_LoadMDPaletteRW(SDL_Palette* pal, int palid, SDL_RWops* rw, SDL_bool free_rwops);
void MD_LoadPaletteSurface(SDL_Palette* pal, int palid, SDL_Surface* src, SDL_bool free_src);

void MD_LoadMDPaletteEx(const char* filename, SDL_Color* out, int ncolors);
void MD_LoadPaletteEx(const char* filename, SDL_Color* out, int ncolors);

void MD_LoadMDPaletteExRW(SDL_RWops* rw, SDL_bool free_rwops, SDL_Color* out, int ncolors);
void MD_LoadPaletteExSurface(SDL_Surface* src, SDL_bool free_src, SDL_Color* out, int ncolors);

// Save a palette to an image file
void MD_SavePalette(SDL_Palette* pal, int palid, const char* filename);

void MD_CopyPalette(SDL_Palette* srcpal, int srcpalid, SDL_Palette* dstpal, int dstpalid);

#endif // __MDPALETTE_H__