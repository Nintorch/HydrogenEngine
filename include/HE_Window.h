#ifndef __HE_WINDOW_H__
#define __HE_WINDOW_H__

#include "HE_Render.h"
#include "SDL_render.h"

#define HE_WINDOW_TITLE     "Hydrogen Engine"
#define HE_WINDOW_WIDTH     (HE_FB_WIDTH * 2)
#define HE_WINDOW_HEIGHT    (HE_FB_HEIGHT * 2)

SDL_Window* HE_GetWindow(void);

void HE_UpdateFBZoom(void);
float HE_GetFBZoom(void);

void HE_Close(void);

void HE_Init(void);
void HE_Update(void);
void HE_Quit(void);

#endif // __HE_WINDOW_H__