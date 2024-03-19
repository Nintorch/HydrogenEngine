#ifndef __HE_PALETTE_H__
#define __HE_PALETTE_H__

#include "SDL_surface.h"
#include "SDL_pixels.h"
#include "SDL_rwops.h"

#define HE_PALETTE_COLORS   16
#define HE_PALETTE_COUNT    (int)(256 / HE_PALETTE_COLORS)

void HE_PaletteInit(void);
void HE_PaletteQuit(void);

SDL_Palette* HE_GetColorPalette(void);
SDL_Palette* HE_GetDefaultColorPalette(void);
void HE_SetColorPalette(SDL_Palette* pal);
void HE_ResetCurrentColorPalette(void);

SDL_Palette* HE_CreateColorPalette(void);

Uint8 HE_MapColor(int palid, int colorid);
Uint8 HE_MapColorInternal(int palid, int colorid);

// Pass NULL if you want to use the current color palette
const SDL_Color* HE_GetPaletteColors(SDL_Palette* pal, int palid);
void HE_SetPaletteColors(SDL_Palette* pal, int palid, const SDL_Color* colors, int first_color, int ncolors);
void HE_FillPalette(SDL_Palette* pal, int palid, int r, int g, int b);
void HE_ClearPalette(SDL_Palette* pal, int palid);

void HE_ConvertColor(Uint16 HE_color, SDL_Color* out);

// TODO: allow loading several palettes from a single file

// Load a palette in Mega Drive format
void HE_LoadPaletteMD(SDL_Palette* pal, int palid, int npals, const char* filename);
// Load a palette from an image file
void HE_LoadPalette(SDL_Palette* pal, int palid, int npals, const char* filename);

void HE_LoadPaletteMDOut(const char* filename, int pal, SDL_Color* out, int ncolors);
void HE_LoadPaletteOut(const char* filename, int pal, SDL_Color* out, int ncolors);

void HE_LoadPaletteMDRWOut(SDL_RWops* rw, SDL_bool free_rwops, SDL_Color* out, int ncolors);
void HE_LoadPaletteSurfaceOut(SDL_Surface* src, SDL_bool free_src, int pal, SDL_Color* out, int ncolors);

// Save a palette to an image file
void HE_SavePalette(SDL_Palette* pal, int palid, const char* filename);

void HE_CopyPalette(SDL_Palette* srcpal, int srcpalid, SDL_Palette* dstpal, int dstpalid);
void HE_CopyPaletteOut(SDL_Palette* srcpal, int srcpalid, SDL_Color* out, int ncolors);

#endif // __HE_PALETTE_H__