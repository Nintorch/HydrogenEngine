#include "HydrogenEngine.h"
#include "game/Objects.h"

#include <math.h>

typedef struct
{
    Uint8 timer;
} SonicUserData;

static void SonicMain(HE_Object* object)
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

    object->xspeed = HE_GetInputHeld() & KEY_RIGHT ? 1.f : HE_GetInputHeld() & KEY_LEFT ? -1.f : 0;
    object->yspeed = HE_GetInputHeld() & KEY_DOWN ? 1.f : HE_GetInputHeld() & KEY_UP ? -1.f : 0;
    HE_SpeedToPos(object);
}

static void SonicConstructor(HE_Object* object)
{
    object->routine = SonicMain;
    object->spritesheet = //HE_LoadSpritesheet(64, 64, "data/spritesheet.png", 0, 0);
        HE_LoadSpritesheetMD(64, 64, "data/SonicArt.bin", "data/SonicMap.bin", "data/SonicDPLC.bin", 0);
    object->spritesheet_owner = SDL_TRUE;
    object->frame = 6;

    object->x = HE_GetFramebuffer()->w / 2;
    object->y = HE_GetFramebuffer()->h / 2;
}

HE_Object* CreateSonicObject(void)
{
    return HE_InitializeReservedObject(SonicConstructor);
}