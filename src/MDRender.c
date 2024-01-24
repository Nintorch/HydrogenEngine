#include "MD.h"
#include "SDL_image.h"

#include <stdio.h>

static SDL_Renderer* hwrender;
static SDL_Surface* framebuffer;
static SDL_Texture* fb_texture;

static MD_HInterrupt hblank;

void MD_RenderInit(void)
{
    hwrender = SDL_CreateRenderer(MD_GetWindow(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    framebuffer = MD_CreateSurface(MD_FB_WIDTH, MD_FB_HEIGHT, MD_SURFACEFLAG_NONE);
    fb_texture = SDL_CreateTexture(hwrender,
        SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING,
        MD_FB_WIDTH, MD_FB_HEIGHT);

    MD_ResetHBlank();
}

void MD_RenderQuit(void)
{
    SDL_FreeSurface(framebuffer);
    SDL_DestroyTexture(fb_texture);
    SDL_DestroyRenderer(hwrender);
}

static void setup_surface(SDL_Surface* surface, int flags)
{
    SDL_SetSurfacePalette(surface, MD_GetDefaultColorPalette());
    if (flags & MD_SURFACEFLAG_TRANSPARENT)
        SDL_SetColorKey(surface, SDL_TRUE, 0);
    MD_SetSurfaceFlags(surface, flags);
}

SDL_Surface* MD_CreateSurface(int w, int h, int flags)
{
    SDL_Surface* surface = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 8, 0, 0, 0, 0);
    setup_surface(surface, flags);
    return surface;
}

Uint8* MD_GetSurfacePixels(SDL_Surface* surface)
{
    return (Uint8*)surface->pixels;
}

void MD_FillSurface(SDL_Surface* surface, int palid, int colorid)
{
    Uint8 color = MD_MapColorInternal(palid, colorid);
    memset(surface->pixels, color, surface->pitch * surface->h);
}

void MD_ClearSurface(SDL_Surface* surface)
{
    MD_FillSurface(surface, 0, 0);
}

static SDL_bool colors_equal(const SDL_Color* a, const SDL_Color* b)
{
    return a->r == b->r
        && a->g == b->g
        && a->b == b->b;
}

static int find_color(const SDL_Color* color, int palid_start, int palid_end)
{
    if (color->a == 0)
        return 0;

    SDL_Color* palette = MD_GetColorPalette()->colors;
    int color_start = MD_MapColorInternal(palid_start, 0) + 1;
    int color_end = MD_MapColorInternal(palid_end, MD_PALETTE_COLORS - 1);

    for (int i = color_start; i <= color_end; i++)
    {
        // Skip the transparent colors
        if ((i % MD_PALETTE_COLORS) == 0)
            continue;
        if (colors_equal(palette + i, color))
            return i;
    }

    return 0;
}

SDL_Surface* MD_ConvertSurface(SDL_Surface* surface, int palid_start, int palid_end)
{
    if (surface->format->BytesPerPixel != 4)
    {
        // TODO
        printf("Can't convert this format\n");
        return NULL;
    }

    SDL_Surface* mdsurface = MD_CreateSurface(surface->w, surface->h, MD_SURFACEFLAG_TRANSPARENT);
    Uint8* pixels = surface->pixels;
    Uint8* mdpixels = MD_GetSurfacePixels(mdsurface);
    SDL_PixelFormat* pixel_format = surface->format;
    int BytesPerPixel = surface->format->BytesPerPixel;

    for (int i = 0; i < surface->h; i++)
    {
        for (int j = 0; j < surface->w; j++)
        {
            SDL_Color color;
            Uint32 pixel = *(Uint32*)(pixels + j * BytesPerPixel);
            SDL_GetRGBA(pixel, pixel_format, &color.r, &color.g, &color.b, &color.a);
            mdpixels[j] = find_color(&color, palid_start, palid_end);
        }
        mdpixels += mdsurface->pitch;
        pixels += surface->pitch;
    }
    return mdsurface;
}

SDL_Surface* MD_LoadSurfaceRW(SDL_RWops* rw, SDL_bool free_rwops, int palid_start, int palid_end)
{
    SDL_Surface* rgb = IMG_Load_RW(rw, free_rwops);
    SDL_Surface* result = MD_ConvertSurface(rgb, palid_start, palid_end);
    SDL_FreeSurface(rgb);
    return result;
}

SDL_Surface* MD_LoadSurface(const char* filename, int palid_start, int palid_end)
{
    return MD_LoadSurfaceRW(SDL_RWFromFile(filename, "rb"), SDL_TRUE, palid_start, palid_end);
}

// Rendering

void MD_PreTextureRender(void)
{
    SDL_SetRenderDrawColor(hwrender, 0, 0, 0, 255);
    SDL_RenderClear(hwrender);
}

void MD_PostTextureRender(void)
{
    SDL_RenderPresent(hwrender);
}

SDL_Surface* MD_GetFramebuffer(void)
{
    return framebuffer;
}

SDL_Surface* MD_LockFBTexture(void)
{
    SDL_Surface* fb_surface;
    SDL_LockTextureToSurface(fb_texture, NULL, &fb_surface);
    return fb_surface;
}

void MD_UnlockFBTexture(void)
{
    SDL_UnlockTexture(fb_texture);
}

void MD_RenderFBToSurface(SDL_Surface* surface)
{
    if (surface->format->format != SDL_PIXELFORMAT_ABGR8888)
    {
        // TODO
        printf("MD_RenderFBToSurface: Invalid format\n");
        return;
    }

    Uint32* pixels = surface->pixels;
    Uint8* fb_pixels = framebuffer->pixels;
    for (int i = 0; i < framebuffer->h; i++)
    {
        hblank(i);
        Uint32* colors = (Uint32*)MD_GetColorPalette()->colors;
        for (int j = 0; j < framebuffer->w; j++)
        {
            Uint8 colorid = fb_pixels[j];
            Uint32 color = colors[colorid];
            pixels[j] = color;
        }
        pixels += surface->pitch / 4;
        fb_pixels += framebuffer->pitch;
    }
}

void MD_RenderFBTextureToScreen()
{
    int w, h;
    SDL_GetWindowSize(MD_GetWindow(), &w, &h);

    float fb_zoom_factor = MD_GetFBZoom();
    SDL_Rect rect = {
        w / 2 - framebuffer->w * fb_zoom_factor / 2,
        h / 2 - framebuffer->h * fb_zoom_factor / 2,
        framebuffer->w * fb_zoom_factor,
        framebuffer->h * fb_zoom_factor,
    };
    SDL_RenderCopy(hwrender, fb_texture, NULL, &rect);
}

void MD_RenderSurfaceEx(SDL_Surface* src, SDL_Rect* srcrect, int x, int y, double zoomx, double zoomy, double angle)
{
    int flags = MD_GetSurfaceFlags(src);
    SDL_Surface* surface = src;
    if (srcrect)
        surface = MD_ClipSurface(src, srcrect);

    SDL_bool need_scalerotate = MD_NeedScaleRotate(zoomx, zoomy, angle);
    SDL_Surface* rotozoom = surface;
    if (need_scalerotate)
    {
        rotozoom = MD_ScaleRotateSurface(surface, zoomx, zoomy, angle);
        setup_surface(rotozoom, flags);
    }

    // If a clipped surface was created and if we don't need it
    // in case it was scaled/rotated, free it
    // (don't free it if it wasn't rotated, we will draw it below)
    if (srcrect && need_scalerotate) SDL_FreeSurface(surface);

    SDL_Rect rect = { x - rotozoom->w / 2, y - rotozoom->h / 2, rotozoom->w, rotozoom->h };
    SDL_BlitSurface(rotozoom, NULL, framebuffer, &rect);

    // If the clipped surface was scaled/rotated, flip the modified surface
    // (the clipped surface was freed above)
    // If the clipped surface wasn't scaled/rotated, free the clipped surface
    // that we don't free above (rotozoom == surface)
    if (srcrect || need_scalerotate)
        SDL_FreeSurface(rotozoom);
}

void MD_RenderSurfaceAngle(SDL_Surface* src, SDL_Rect* srcrect, int x, int y, double angle)
{
    MD_RenderSurfaceEx(src, srcrect, x, y, 1.0, 1.0, angle);
}

void MD_RenderSurface(SDL_Surface* src, SDL_Rect* srcrect, int x, int y)
{
    MD_RenderSurfaceEx(src, srcrect, x, y, 1.0, 1.0, 0.0);
}

static int offset(int x, int size, int offset)
{
    x += offset;
    x %= size;
    if (x < 0)
        x += size;
    return x;
}

void MD_RenderSurfaceDeform(SDL_Surface* src, int xleft, int ytop, int* hdeform)
{
    Uint8* pixels = src->pixels;
    Uint8* fbpixels = framebuffer->pixels;

    int xstart = SDL_max(xleft, 0);
    int xend = SDL_min((xleft + src->w), framebuffer->w);
    int ystart = SDL_max(ytop, 0);
    int yend = SDL_min((ytop + src->h), framebuffer->h);

    if (ytop < 0)
        pixels += src->pitch * -ytop;
    fbpixels += framebuffer->pitch * ystart;

    for (int i = ystart; i < yend; i++)
    {
        int y = i - ytop;
        for (int j = xstart; j < xend; j++)
        {
            Uint8 pixel = pixels[offset(j - xleft, src->w, hdeform[y])];
            if (pixel)
                fbpixels[j] = pixel;
        }
        pixels += src->pitch;
        fbpixels += framebuffer->pitch;
    }
}

MD_Spritesheet* MD_CreateSpritesheet(int sprite_w, int sprite_h, SDL_Surface* spritesheet_surface, SDL_bool take_ownership)
{
    MD_Spritesheet* spritesheet = (MD_Spritesheet*)malloc(sizeof(*spritesheet));

    spritesheet->surface = spritesheet_surface;
    spritesheet->surface_ownership = take_ownership;

    spritesheet->sprite_w = sprite_w;
    spritesheet->sprite_h = sprite_h;

    spritesheet->sprites_h = spritesheet_surface->w / sprite_w;
    spritesheet->sprites_v = spritesheet_surface->h / sprite_h;

    return spritesheet;
}

void MD_FreeSpritesheet(MD_Spritesheet* spritesheet)
{
    if (spritesheet->surface_ownership)
        SDL_FreeSurface(spritesheet->surface);
    free(spritesheet);
}

MD_Spritesheet* MD_LoadSpritesheetMD_RW(int sprite_w, int sprite_h,
    SDL_RWops* tiles, SDL_RWops* mappings, SDL_RWops* dplc, int palid)
{
    int sprite_count = MD_GetSpriteMappingsCount(mappings);
    int sprites_h = 10;
    int sprites_v = sprite_count / sprites_h + 1;

    SDL_Surface* spritesheet_surface = MD_CreateSurface(
        sprite_w * sprites_h, sprite_h * sprites_v,
        MD_SURFACEFLAG_TRANSPARENT);

    int sprite = 0;
    for (int i = 0; i < sprites_v; i++)
    {
        for (int j = 0; j < sprites_h; j++)
        {
            if (sprite >= sprite_count)
                break;
            MD_DrawSpriteMappings(tiles, mappings, dplc, sprite, palid,
                sprite_w * j + (sprite_w / 2), sprite_h * i + (sprite_h / 2), spritesheet_surface);
            sprite++;
        }
    }

    MD_Spritesheet* spritesheet = MD_CreateSpritesheet(sprite_w, sprite_h, spritesheet_surface, SDL_TRUE);
    return spritesheet;
}

MD_Spritesheet* MD_LoadSpritesheetMD(int sprite_w, int sprite_h,
    const char* tiles_file, const char* mappings_file, const char* dplc_file, int palid)
{
    SDL_RWops* tiles = SDL_RWFromFile(tiles_file, "rb");
    SDL_RWops* mappings = SDL_RWFromFile(mappings_file, "rb");
    SDL_RWops* dplc = SDL_RWFromFile(dplc_file, "rb");

    MD_Spritesheet* spritesheet = MD_LoadSpritesheetMD_RW(sprite_w, sprite_h,
        tiles, mappings, dplc, palid);

    SDL_RWclose(tiles);
    SDL_RWclose(mappings);
    SDL_RWclose(dplc);

    return spritesheet;
}

MD_Spritesheet* MD_LoadSpritesheet(int sprite_w, int sprite_h, const char* filename, int palid_start, int palid_end)
{
    return MD_CreateSpritesheet(sprite_w, sprite_h, MD_LoadSurface(filename, palid_start, palid_end), SDL_TRUE);
}

void MD_SaveSpritesheet(MD_Spritesheet* spritesheet, const char* filename)
{
    const char* filename_test = filename + strlen(filename) - 4;
    if (strlen(filename) < 5)
        return;
    if (!strcmp(filename_test, ".png"))
        IMG_SavePNG(spritesheet->surface, filename);
    else if (!strcmp(filename_test, ".bmp"))
        SDL_SaveBMP(spritesheet->surface, filename);
}

SDL_Rect MD_GetSpriteRect(MD_Spritesheet* spritesheet, int sprite)
{
    int sprite_h = sprite % spritesheet->sprites_h;
    int sprite_v = sprite / spritesheet->sprites_h;

    SDL_Rect out = {
        sprite_h * spritesheet->sprite_w,
        sprite_v * spritesheet->sprite_h,
        spritesheet->sprite_w,
        spritesheet->sprite_h,
    };

    return out;
}

void MD_RenderSpritesheetEx(MD_Spritesheet* spritesheet, int sprite, int x, int y, double zoomx, double zoomy, double angle)
{
    SDL_Rect rect = MD_GetSpriteRect(spritesheet, sprite);
    MD_RenderSurfaceEx(spritesheet->surface, &rect, x, y, zoomx, zoomy, angle);
}

void MD_RenderSpritesheetAngle(MD_Spritesheet* spritesheet, int sprite, int x, int y, double angle)
{
    MD_RenderSpritesheetEx(spritesheet, sprite, x, y, 1.0, 1.0, angle);
}

void MD_RenderSpritesheet(MD_Spritesheet* spritesheet, int sprite, int x, int y)
{
    MD_RenderSpritesheetEx(spritesheet, sprite, x, y, 1.0, 1.0, 0.0);
}

static void default_hblank(int y) {}

void MD_SetHBlank(MD_HInterrupt h_int)
{
    hblank = h_int;
}
void MD_ResetHBlank(void)
{
    MD_SetHBlank(default_hblank);
}

// Mega Drive tile rendering

static void draw_tile_hline(SDL_RWops* tiles, Uint8* pixels, int palid, int xleft, int flipped)
{
    if (!flipped)
    {
        for (int j = 0; j < 8 / 2; j++)
        {
            Uint8 byte = SDL_ReadU8(tiles);
            pixels[xleft + j * 2] = MD_MapColor(palid, (byte & 0xF0) / 0x10);
            pixels[xleft + j * 2 + 1] = MD_MapColor(palid, byte & 0xF);
        }
    }
    else
    {
        for (int j = 8 / 2 - 1; j >= 0; j--)
        {
            Uint8 byte = SDL_ReadU8(tiles);
            pixels[xleft + j * 2 + 1] = MD_MapColor(palid, (byte & 0xF0) / 0x10);
            pixels[xleft + j * 2] = MD_MapColor(palid, byte & 0xF);
        }
    }
}

void MD_DrawTile(SDL_RWops* tiles, int tileid, int palid, int flags, SDL_Surface* dst, int xleft, int ytop)
{
    Uint8* pixels = MD_GetSurfacePixels(dst);
    pixels += dst->pitch * ytop;

    SDL_RWseek(tiles, tileid * 0x20, RW_SEEK_SET);
    if ((flags & MD_TILE_YFLIP) == 0)
    {
        for (int i = 0; i < 8; i++)
        {
            draw_tile_hline(tiles, pixels, palid, xleft, flags & MD_TILE_XFLIP);
            pixels += dst->pitch;
        }
    }
    else
    {
        pixels += dst->pitch * 7;
        for (int i = 7; i >= 0; i--)
        {
            draw_tile_hline(tiles, pixels, palid, xleft, flags & MD_TILE_XFLIP);
            pixels -= dst->pitch;
        }
    }
}


static void draw_mappings_vline(SDL_RWops* tiles,
    int tilesh, int pal, int map_flags, int xleft, int tilex, int ytop, int tile,
    SDL_Surface* dst)
{
    if ((map_flags & MD_TILE_YFLIP) == 0)
    {
        for (int h = 0; h < tilesh; h++)
        {
            MD_DrawTile(tiles, tile, pal, map_flags, dst, xleft + tilex * 8, ytop + h * 8);
            tile++;
        }
    }
    else
    {
        for (int h = tilesh - 1; h >= 0; h--)
        {
            MD_DrawTile(tiles, tile, pal, map_flags, dst, xleft + tilex * 8, ytop + h * 8);
            tile++;
        }
    }
}

// TODO: comments
void MD_DrawSpriteMappings(
    SDL_RWops* tiles, SDL_RWops* map, SDL_RWops* dplc,
    int spriteid, int palid,
    int xoffset, int yoffset,
    SDL_Surface* dst)
{
    SDL_RWseek(map, 2 * spriteid, RW_SEEK_SET);
    SDL_RWseek(dplc, 2 * spriteid, RW_SEEK_SET);

    int map_offset = SDL_ReadBE16(map);
    SDL_RWseek(map, map_offset, RW_SEEK_SET);
    int mappings_count = SDL_ReadU8(map);

    int dplc_offset = SDL_ReadBE16(dplc);
    // +1 to skip the DPLC count for the sprite
    SDL_RWseek(dplc, dplc_offset, RW_SEEK_SET);
    int dplc_count = SDL_ReadU8(dplc);

    int dplc_tile = -1;
    if (dplc_count == 1)
        dplc_tile = SDL_ReadBE16(dplc) & 0xFFF;

    for (int i = 0; i < mappings_count; i++)
    {
        int ytop = (Sint8)SDL_ReadU8(map) + yoffset;

        Uint8 temp1 = SDL_ReadU8(map);
        int tilesw = (temp1 & 0xC) / 0x4 + 1;
        int tilesh = (temp1 & 0x3) + 1;

        Uint16 temp2 = SDL_ReadBE16(map);
        int pal = (temp2 & 0x6000) / 0x2000 + palid;
        int map_flags = (temp2 & 0x1800) / 0x800;

        int xleft = (Sint8)SDL_ReadU8(map) + xoffset;

        int tile;
        if (dplc_count > 1)
        {
            Uint16 dplc_value = SDL_ReadBE16(dplc);
            tile = dplc_value & 0xFFF;
        }
        else
        {
            tile = dplc_tile;
        }

        if ((map_flags & MD_TILE_XFLIP) == 0)
        {
            for (int j = 0; j < tilesw; j++)
            {
                draw_mappings_vline(tiles, tilesh, pal, map_flags, xleft, j, ytop, tile, dst);
                tile += tilesh;
            }
        }
        else
        {
            for (int j = tilesw - 1; j >= 0; j--)
            {
                draw_mappings_vline(tiles, tilesh, pal, map_flags, xleft, j, ytop, tile, dst);
                tile += tilesh;
            }
        }
    }
}

int MD_GetSpriteMappingsCount(SDL_RWops* map)
{
    SDL_RWseek(map, 0, RW_SEEK_SET);
    return SDL_ReadBE16(map) / 2;
}