#ifndef __HE_SURFACEHELPER_H__
#define __HE_SURFACEHELPER_H__

#include "SDL_surface.h"

SDL_Surface* HE_CopySurface(SDL_Surface* surface);
SDL_Surface* HE_ClipSurface(SDL_Surface* surface, SDL_Rect* rect);

SDL_Surface* HE_ScaleSurface(SDL_Surface* surface, double zoomx, double zoomy);
SDL_Surface* HE_RotateSurface(SDL_Surface* surface, double angle);
SDL_Surface* HE_ScaleRotateSurface(SDL_Surface* surface, double zoomx, double zoomy, double angle);

SDL_bool HE_NeedScale(double zoomx, double zoomy);
SDL_bool HE_NeedRotate(double angle);
SDL_bool HE_NeedScaleRotate(double zoomx, double zoomy, double angle);

#endif // __HE_SURFACEHELPER_H__