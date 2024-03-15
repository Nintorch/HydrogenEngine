#ifndef __MDWINDOW_H__
#define __MDWINDOW_H__

#include "MDRender.h"
#include "SDL_render.h"

#define MD_WINDOW_TITLE     "Hydrogen Engine"
#define MD_WINDOW_WIDTH     (MD_FB_WIDTH * 2)
#define MD_WINDOW_HEIGHT    (MD_FB_HEIGHT * 2)

SDL_Window* MD_GetWindow(void);

void MD_UpdateFBZoom(void);
float MD_GetFBZoom(void);

void MD_Close(void);

void MD_Init(void);
void MD_Update(void);
void MD_Quit(void);

#endif // __MDWINDOW_H__