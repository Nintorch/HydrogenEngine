#include "HydrogenEngine.h"
#include "SDL2_rotozoom.h"

SDL_Surface* HE_CopySurface(SDL_Surface* surface)
{
    SDL_Surface* result = HE_CreateSurface(surface->w, surface->h, HE_GetSurfaceFlags(surface));
    size_t size = result->pitch * result->h;
    memcpy(result->pixels, surface->pixels, size);
    return result;
}

SDL_Surface* HE_ClipSurface(SDL_Surface* surface, SDL_Rect* rect)
{
    if (!rect)
        return HE_CopySurface(surface);

    SDL_Surface* result = HE_CreateSurface(rect->w, rect->h, HE_GetSurfaceFlags(surface));
    SDL_BlitSurface(surface, rect, result, NULL);
    return result;
}

SDL_Surface* HE_RotateSurface(SDL_Surface* surface, double angle)
{
    return rotozoomSurface(surface, angle, 1.0, SMOOTHING_OFF);
}

SDL_Surface* HE_ScaleSurface(SDL_Surface* surface, double zoomx, double zoomy)
{
    SDL_Surface* result = HE_CreateSurface(
        (int)(surface->w * SDL_fabs(zoomx)), (int)(surface->h * SDL_fabs(zoomy)),
        HE_GetSurfaceFlags(surface));

    Uint8* pixels = HE_GetSurfacePixels(result);
    Uint8* ogpixels = HE_GetSurfacePixels(surface);
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
        ogpixels = HE_GetSurfacePixels(surface) + surface->pitch * idx;
    }
    return result;
}

SDL_Surface* HE_ScaleRotateSurface(SDL_Surface* surface, double zoomx, double zoomy, double angle)
{
    SDL_bool need_rotate = HE_NeedRotate(angle);
    SDL_bool need_scale = HE_NeedScale(zoomx, zoomy);

    // no need to change the surface
    if (!need_rotate && !need_scale)
        return HE_CopySurface(surface);

    // don't rotate if not needed
    if (!need_rotate)
        return HE_ScaleSurface(surface, zoomx, zoomy);

    // don't scale if not needed
    if (!need_scale)
        return rotozoomSurface(surface, angle, 1.0, SMOOTHING_OFF);

    SDL_Surface* scaled = HE_ScaleSurface(surface, zoomx, zoomy);
    SDL_Surface* rotated = HE_RotateSurface(scaled, angle);
    SDL_FreeSurface(scaled);
    return rotated;
}

SDL_bool HE_NeedScale(double zoomx, double zoomy)
{
    return SDL_fabs(zoomx - 1.0) > 0.001 || SDL_fabs(zoomy - 1.0) > 0.001;
}

SDL_bool HE_NeedRotate(double angle)
{
    return SDL_fabs(angle) > 0.001;
}

SDL_bool HE_NeedScaleRotate(double zoomx, double zoomy, double angle)
{
    return HE_NeedScale(zoomx, zoomy) || HE_NeedRotate(angle);
}