#ifndef __MDGAME_H__
#define __MDGAME_H__

#include "SDL_surface.h"

void GameInit(void);
void GameUpdate(void);
void GameFramebufferRender(SDL_Surface* framebuffer);
void GameTextureRender(SDL_Surface* rgbframebuffer);
void GameQuit(void);

#endif // __MDGAME_H__