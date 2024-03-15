#ifndef __MDSURFACE_H__
#define __MDSURFACE_H__

#include "SDL_surface.h"

#define MD_FB_WIDTH     320
#define MD_FB_HEIGHT    240

void MD_RenderInit(void);
void MD_RenderQuit(void);

// Surfaces
// NOTE: Hydrogen Engine API mainly works with 8 bit surfaces,
// the only functions that take in an RGBA surface are
// MD_ConvertSurface and MD_RenderFBToSurface

enum MD_SurfaceFlags
{
    MD_SURFACEFLAG_NONE,
    MD_SURFACEFLAG_TRANSPARENT,
};

#define MD_GetSurfaceFlags(surface) (*(int*)(&surface->userdata))
#define MD_SetSurfaceFlags(surface, flags) MD_GetSurfaceFlags(surface) = flags

SDL_Surface* MD_CreateSurface(int w, int h, int flags);
Uint8* MD_GetSurfacePixels(SDL_Surface* surface);
void MD_FillSurface(SDL_Surface* surface, int palid, int colorid);
void MD_ClearSurface(SDL_Surface* surface);

SDL_Surface* MD_ConvertSurface(SDL_Surface* surface, int palid_start, int palid_end);
SDL_Surface* MD_LoadSurfaceRW(SDL_RWops* rw, SDL_bool free_rwops, int palid_start, int palid_end);
SDL_Surface* MD_LoadSurface(const char* filename, int palid_start, int palid_end);
void MD_SaveSurface(SDL_Surface* surface, const char* filename);

// Rendering

void MD_PreTextureRender(void);
void MD_PostTextureRender(void);

SDL_Surface* MD_GetFramebuffer(void);

SDL_Surface* MD_LockFBTexture(void);
void MD_UnlockFBTexture(void);

void MD_RenderFBToSurface(SDL_Surface* surface);
void MD_RenderFBTextureToScreen(void);

void MD_RenderSurfaceEx(SDL_Surface* src, SDL_Rect* srcrect, int x, int y, double zoomx, double zoomy, double angle);
void MD_RenderSurfaceAngle(SDL_Surface* src, SDL_Rect* srcrect, int x, int y, double angle);
void MD_RenderSurface(SDL_Surface* src, SDL_Rect* srcrect, int x, int y);

void MD_RenderSurfaceDeform(SDL_Surface* src, int xleft, int ytop, int* hdeform, int deformsize);
void MD_RenderSurfaceDeform2(SDL_Surface* src, int xleft, int ytop, int* hdeform, int deformsize);

typedef struct
{
    SDL_Surface* surface;
    int sprite_w, sprite_h;
    int sprites_h, sprites_v;
    SDL_bool surface_ownership;
} MD_Spritesheet;

MD_Spritesheet* MD_CreateSpritesheet(int sprite_w, int sprite_h, SDL_Surface* spritesheet_surface, SDL_bool take_ownership);
void MD_FreeSpritesheet(MD_Spritesheet* spritesheet);

// NOTE: Mappings and DPLCs are in Sonic 1 format

MD_Spritesheet* MD_LoadSpritesheetMD_RW(int sprite_w, int sprite_h,
    SDL_RWops* tiles, SDL_RWops* mappings, SDL_RWops* dplc, int palid);
MD_Spritesheet* MD_LoadSpritesheetMD(int sprite_w, int sprite_h,
    const char* tiles_file, const char* mappings_file, const char* dplc_file, int palid);
MD_Spritesheet* MD_LoadSpritesheet(int sprite_w, int sprite_h, const char* filename, int palid_start, int palid_end);
void MD_SaveSpritesheet(MD_Spritesheet* spritesheet, const char* filename);

SDL_Rect MD_GetSpriteRect(MD_Spritesheet* spritesheet, int sprite);

void MD_RenderSpritesheetEx(MD_Spritesheet* spritesheet, int sprite, int x, int y, double zoomx, double zoomy, double angle);
void MD_RenderSpritesheetAngle(MD_Spritesheet* spritesheet, int sprite, int x, int y, double angle);
void MD_RenderSpritesheet(MD_Spritesheet* spritesheet, int sprite, int x, int y);

// Horizontal interrupt
// Runs before the current line is drawn
// (which is different from Mega Drive because there
// horizontal interrupt is ran after a line is drawn)

typedef void(*MD_HInterrupt)(int y);
void MD_SetHBlank(MD_HInterrupt h_int);
void MD_ResetHBlank(void);

// Mega Drive tile rendering

enum MD_TileFlags
{
    MD_TILE_XFLIP = 1,
    MD_TILE_YFLIP = 2,
};

void MD_DrawTile(SDL_RWops* tiles, int tileid, int palid, int flags, SDL_Surface* dst, int xleft, int ytop);
void MD_DrawSpriteMappings(
    SDL_RWops* tiles, SDL_RWops* map, SDL_RWops* dplc,
    int spriteid, int palid,
    int xoffset, int yoffset,
    SDL_Surface* dst);
int MD_GetSpriteMappingsCount(SDL_RWops* map);

#endif // __MDSURFACE_H__