#include "MD.h"
#include "game/Objects.h"

#include <math.h>

typedef struct
{
    Uint8 timer;
} SonicUserData;

static void SonicMain(MD_Object* object)
{
    SonicUserData* userdata = (SonicUserData*)object->userdata;

    userdata->timer++;
    if (userdata->timer > 5)
    {
        object->frame++;
        if (object->frame > 0xB)
            object->frame = 6;
        userdata->timer = 0;
    }

    object->xspeed = MD_GetInputHeld() & KEY_RIGHT ? 1.f : MD_GetInputHeld() & KEY_LEFT ? -1.f : 0;
    object->yspeed = MD_GetInputHeld() & KEY_DOWN ? 1.f : MD_GetInputHeld() & KEY_UP ? -1.f : 0;
    MD_SpeedToPos(object);
}

static void SonicConstructor(MD_Object* object)
{
    object->routine = SonicMain;
    object->spritesheet = MD_LoadSpritesheet(64, 64, "data/spritesheet.png", 0, 0);
    object->spritesheet_owner = SDL_TRUE;
    object->frame = 6;

    object->x = MD_GetFramebuffer()->w / 2;
    object->y = MD_GetFramebuffer()->h / 2;
}

MD_Object* CreateSonicObject(void)
{
    return MD_InitializeReservedObject(SonicConstructor);
}