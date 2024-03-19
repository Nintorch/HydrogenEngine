#include "HydrogenEngine.h"

typedef struct
{
    Uint8 HE_key;
    SDL_Scancode key;
} HE_KeyMap;

static HE_InputBitfield pressed = 0;
static HE_InputBitfield held = 0;

HE_KeyMap key_map[8] = {
    {KEY_UP, SDL_SCANCODE_UP},
    {KEY_DOWN, SDL_SCANCODE_DOWN},
    {KEY_LEFT, SDL_SCANCODE_LEFT},
    {KEY_RIGHT, SDL_SCANCODE_RIGHT},
    {KEY_B, SDL_SCANCODE_X},
    {KEY_C, SDL_SCANCODE_C},
    {KEY_A, SDL_SCANCODE_Z},
    {KEY_START, SDL_SCANCODE_RETURN}
};

void HE_InputSystemInit(void)
{

}

void HE_InputSystemUpdate(void)
{
    pressed = 0;
}

void HE_InputSystemQuit(void)
{

}

static int find_key(SDL_Scancode key)
{
    HE_KeyMap* entry = key_map;
    for (int i = 0; i < 8; i++)
    {
        if (entry->key == key)
            return entry->HE_key;
        entry++;
    }
    return 0;
}

void HE_InputHandleEvent(SDL_Event* event)
{
    SDL_Scancode key;
    switch (event->type)
    {
        case SDL_KEYDOWN:
            if (event->key.repeat)
                break;
            key = find_key(event->key.keysym.scancode);
            pressed |= key;
            held |= key;
            break;
        case SDL_KEYUP:
            key = find_key(event->key.keysym.scancode);
            held &= ~key;
            break;
    }
}

HE_InputBitfield HE_GetInputPressed(void)
{
    return pressed;
}

HE_InputBitfield HE_GetInputHeld(void)
{
    return held;
}

void HE_SetPressed(int keys)
{
    pressed = keys;
}

void HE_SetHeld(int keys)
{
    held = keys;
}