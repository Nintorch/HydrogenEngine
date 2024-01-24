#ifndef __MDSURFACEHELPER_H__
#define __MDSURFACEHELPER_H__

#include "SDL_surface.h"

SDL_Surface* MD_CopySurface(SDL_Surface* surface);
SDL_Surface* MD_ClipSurface(SDL_Surface* surface, SDL_Rect* rect);

SDL_Surface* MD_ScaleSurface(SDL_Surface* surface, double zoomx, double zoomy);
SDL_Surface* MD_RotateSurface(SDL_Surface* surface, double angle);
SDL_Surface* MD_ScaleRotateSurface(SDL_Surface* surface, double zoomx, double zoomy, double angle);

SDL_bool MD_NeedScale(double zoomx, double zoomy);
SDL_bool MD_NeedRotate(double angle);
SDL_bool MD_NeedScaleRotate(double zoomx, double zoomy, double angle);

#endif // __MDSURFACEHELPER_H__