#ifndef __HE_INPUT_H__
#define __HE_INPUT_H__

#include "SDL_events.h"
#include "SDL_stdinc.h"

typedef Uint8 HE_InputBitfield;

enum HE_InputKeys
{
    KEY_UP      = 1 << 0,
    KEY_DOWN    = 1 << 1,
    KEY_LEFT    = 1 << 2,
    KEY_RIGHT   = 1 << 3,
    KEY_B       = 1 << 4,
    KEY_C       = 1 << 5,
    KEY_A       = 1 << 6,
    KEY_START   = 1 << 7,
};

void HE_InputSystemInit(void);
void HE_InputSystemUpdate(void);
void HE_InputSystemQuit(void);

void HE_InputHandleEvent(SDL_Event* event);

HE_InputBitfield HE_GetInputPressed(void);
HE_InputBitfield HE_GetInputHeld(void);

void HE_SetPressed(int keys);
void HE_SetHeld(int keys);

#endif // __HE_INPUT_H__