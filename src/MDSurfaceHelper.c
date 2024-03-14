#include "MD.h"
#include "SDL2_rotozoom.h"

SDL_Surface* MD_CopySurface(SDL_Surface* surface)
{
    SDL_Surface* result = MD_CreateSurface(surface->w, surface->h, MD_GetSurfaceFlags(surface));
    size_t size = result->pitch * result->h;
    memcpy(result->pixels, surface->pixels, size);
    return result;
}

SDL_Surface* MD_ClipSurface(SDL_Surface* surface, SDL_Rect* rect)
{
    if (!rect)
        return MD_CopySurface(surface);

    SDL_Surface* result = MD_CreateSurface(rect->w, rect->h, MD_GetSurfaceFlags(surface));
    SDL_BlitSurface(surface, rect, result, NULL);
    return result;
}

SDL_Surface* MD_RotateSurface(SDL_Surface* surface, double angle)
{
    return rotozoomSurface(surface, angle, 1.0, SMOOTHING_OFF);
}

SDL_Surface* MD_ScaleSurface(SDL_Surface* surface, double zoomx, double zoomy)
{
    SDL_Surface* result = MD_CreateSurface(
        (int)(surface->w * SDL_fabs(zoomx)), (int)(surface->h * SDL_fabs(zoomy)),
        MD_GetSurfaceFlags(surface));

    Uint8* pixels = MD_GetSurfacePixels(result);
    Uint8* ogpixels = MD_GetSurfacePixels(surface);
    /*
     * Scale the surface horizontally by zoomx,
     * and scale the surface vertically by zoomy.
     */
    for (int i = 0; i < result->h; i++)
    {
        /*
         * Loop through each row of the scaled surface.
         */
        for (int j = 0; j < result->w; j++)
        {
            /*
             * Find the corresponding pixel in the original
             * surface horizontally by dividing the current
             * column number by the horizontal zoom factor.
             *
             * If the result is negative, add the width of
             * the original surface to wrap around to the
             * beginning of the row.
             */
            int idx = (int)(j / zoomx);
            if (idx < 0) idx += surface->w;

            /*
             * Copy the pixel from the original surface to
             * the current row of the scaled surface.
             */
            pixels[j] = ogpixels[idx];
        }

        /*
         * Advance to the next row of the scaled surface.
         */
        pixels += result->pitch;

        /*
         * Update the pointer to the current row of the
         * original surface by dividing the current row
         * number by the vertical zoom factor.
         *
         * If the result is negative, add the height of
         * the original surface to wrap around to the
         * beginning of the surface.
         */
        int idx = (int)(i / zoomy);
        if (idx < 0) idx += surface->h;
        ogpixels = MD_GetSurfacePixels(surface) + surface->pitch * idx;
    }
    return result;
}

SDL_Surface* MD_ScaleRotateSurface(SDL_Surface* surface, double zoomx, double zoomy, double angle)
{
    SDL_bool need_rotate = MD_NeedRotate(angle);
    SDL_bool need_scale = MD_NeedScale(zoomx, zoomy);

    // no need to change the surface
    if (!need_rotate && !need_scale)
        return MD_CopySurface(surface);

    // don't rotate if not needed
    if (!need_rotate)
        return MD_ScaleSurface(surface, zoomx, zoomy);

    // don't scale if not needed
    if (!need_scale)
        return rotozoomSurface(surface, angle, 1.0, SMOOTHING_OFF);

    SDL_Surface* scaled = MD_ScaleSurface(surface, zoomx, zoomy);
    SDL_Surface* rotated = MD_RotateSurface(scaled, angle);
    SDL_FreeSurface(scaled);
    return rotated;
}

SDL_bool MD_NeedScale(double zoomx, double zoomy)
{
    return SDL_fabs(zoomx - 1.0) > 0.001 || SDL_fabs(zoomy - 1.0) > 0.001;
}

SDL_bool MD_NeedRotate(double angle)
{
    return SDL_fabs(angle) > 0.001;
}

SDL_bool MD_NeedScaleRotate(double zoomx, double zoomy, double angle)
{
    return MD_NeedScale(zoomx, zoomy) || MD_NeedRotate(angle);
}