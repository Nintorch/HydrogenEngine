#ifndef __HE_SURFACE_H__
#define __HE_SURFACE_H__

#include "SDL_surface.h"

#define HE_FB_WIDTH     320
#define HE_FB_HEIGHT    240
#define HE_FB_FORMAT    SDL_PIXELFORMAT_ABGR8888

void HE_RenderInit(void);
void HE_RenderQuit(void);

enum HE_SurfaceFlags
{
    HE_SURFACEFLAG_NONE,
    HE_SURFACEFLAG_TRANSPARENT,
};

#define HE_GetSurfaceFlags(surface) (*(int*)(&surface->userdata))
#define HE_SetSurfaceFlags(surface, flags) HE_GetSurfaceFlags(surface) = flags

SDL_Surface* HE_CreateSurface(int w, int h, int flags);
Uint8* HE_GetSurfacePixels(SDL_Surface* surface);

SDL_Surface* HE_ConvertSurface(SDL_Surface* surface, int palid_start, int palid_end);
SDL_Surface* HE_LoadSurfaceRW(SDL_RWops* rw, SDL_bool free_rwops, int palid_start, int palid_end);
SDL_Surface* HE_LoadSurface(const char* filename, int palid_start, int palid_end);
void HE_SaveSurface(SDL_Surface* surface, const char* filename);

// Rendering

void HE_PreTextureRender(void);
void HE_PostTextureRender(void);
void HE_FlushRenderQueue(void);
void HE_SetRenderTarget(SDL_Surface* surface);

SDL_Surface* HE_GetFramebuffer(void);

SDL_Surface* HE_LockFBTexture(void);
void HE_UnlockFBTexture(void);

void HE_RenderFBTextureToScreen(void);

void HE_FillSurface(int palid, int colorid);
void HE_ClearSurface(void);

void HE_RenderSurfaceEx(SDL_Surface* src, SDL_Rect* srcrect, int x, int y, double zoomx, double zoomy, double angle, double opacity);
void HE_RenderSurfaceAngle(SDL_Surface* src, SDL_Rect* srcrect, int x, int y, double angle);
void HE_RenderSurface(SDL_Surface* src, SDL_Rect* srcrect, int x, int y);
void HE_RenderSurfaceDeform(SDL_Surface* src, int xleft, int ytop, int* hdeform, int deformsize, SDL_bool wrap_deform);

void HE_RenderSDLSurface(SDL_Surface* src, int xleft, int ytop);

typedef struct
{
    SDL_Surface* surface;
    int sprite_w, sprite_h;
    int sprites_h, sprites_v;
    SDL_bool surface_ownership;
} HE_Spritesheet;

HE_Spritesheet* HE_CreateSpritesheet(int sprite_w, int sprite_h, SDL_Surface* spritesheet_surface, SDL_bool take_ownership);
void HE_FreeSpritesheet(HE_Spritesheet* spritesheet);

// NOTE: Mappings and DPLCs are in Sonic 1 format

HE_Spritesheet* HE_LoadSpritesheetHE_RW(int sprite_w, int sprite_h,
    SDL_RWops* tiles, SDL_RWops* mappings, SDL_RWops* dplc, int palid);
HE_Spritesheet* HE_LoadSpritesheetMD(int sprite_w, int sprite_h,
    const char* tiles_file, const char* mappings_file, const char* dplc_file, int palid);
HE_Spritesheet* HE_LoadSpritesheet(int sprite_w, int sprite_h, const char* filename, int palid_start, int palid_end);
void HE_SaveSpritesheet(HE_Spritesheet* spritesheet, const char* filename);

SDL_Rect HE_GetSpriteRect(HE_Spritesheet* spritesheet, int sprite);

void HE_RenderSpritesheetEx(HE_Spritesheet* spritesheet, int sprite, int x, int y, double zoomx, double zoomy, double angle, double opacity);
void HE_RenderSpritesheetAngle(HE_Spritesheet* spritesheet, int sprite, int x, int y, double angle);
void HE_RenderSpritesheet(HE_Spritesheet* spritesheet, int sprite, int x, int y);

// Horizontal interrupt
// Runs before the current line is drawn
// (which is different from Mega Drive because there
// horizontal interrupt is ran after a line is drawn)

typedef void(*HE_HInterrupt)(int y);
void HE_SetHBlank(HE_HInterrupt h_int);
void HE_ResetHBlank(void);

// Mega Drive tile rendering

enum HE_TileFlags
{
    HE_TILE_XFLIP = 1,
    HE_TILE_YFLIP = 2,
};

void HE_DrawTile(SDL_RWops* tiles, int tileid, int palid, int flags, SDL_Surface* dst, int xleft, int ytop);
void HE_DrawSpriteMappings(
    SDL_RWops* tiles, SDL_RWops* map, SDL_RWops* dplc,
    int spriteid, int palid,
    int xoffset, int yoffset,
    SDL_Surface* dst);
int HE_GetSpriteMappingsCount(SDL_RWops* map);

#endif // __HE_SURFACE_H__