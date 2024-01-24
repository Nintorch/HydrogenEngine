#ifndef __MDINPUT_H__
#define __MDINPUT_H__

#include "SDL_events.h"
#include "SDL_stdinc.h"

typedef Uint8 MD_InputBitfield;

enum MD_InputKeys
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

void MD_InputSystemInit(void);
void MD_InputSystemUpdate(void);
void MD_InputSystemQuit(void);

void MD_InputHandleEvent(SDL_Event* event);

MD_InputBitfield MD_GetInputPressed(void);
MD_InputBitfield MD_GetInputHeld(void);

void MD_SetPressed(int keys);
void MD_SetHeld(int keys);

#endif // __MDINPUT_H__