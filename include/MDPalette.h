#ifndef __MDPALETTE_H__
#define __MDPALETTE_H__

#include "SDL_pixels.h"

#define MD_PALETTE_COLORS   16
#define MD_PALETTE_COUNT    (int)(256 / MD_PALETTE_COLORS)

void MD_PaletteInit(void);
void MD_PaletteQuit(void);

SDL_Palette* MD_GetColorPalette(void);
SDL_Palette* MD_GetDefaultColorPalette(void);
void MD_SetColorPalette(SDL_Palette* pal);
void MD_ResetColorPalette(void);

SDL_Palette* MD_CreateColorPalette(void);

Uint8 MD_MapColor(int palid, int colorid);
Uint8 MD_MapColorInternal(int palid, int colorid);
const SDL_Color* MD_GetPaletteColors(int palid);
void MD_SetPaletteColors(int palid, const SDL_Color* colors, int first_color, int ncolors);
void MD_FillPalette(int palid, int r, int g, int b);
void MD_ClearPalette(int palid);

void MD_ConvertColor(Uint16 md_color, SDL_Color* out);
// Load palette in Mega Drive format
void MD_LoadPaletteRW(int palid, SDL_RWops* rw, SDL_bool free_rwops);
void MD_LoadPalette(int palid, const char* filename);

#endif // __MDPALETTE_H__