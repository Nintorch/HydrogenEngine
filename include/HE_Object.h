#ifndef __HE_OBJECT_H__
#define __HE_OBJECT_H__

#include "HE_Render.h"
#include "SDL_stdinc.h"

#define HE_RESERVED_OBJECT_COUNT    10
#define HE_DYNAMIC_OBJECT_COUNT     90
#define HE_TOTAL_OBJECTS (HE_RESERVED_OBJECT_COUNT + HE_DYNAMIC_OBJECT_COUNT)

#define HE_OBJECT_USERDATA_SIZE     20

void HE_ObjectSystemInit(void);
void HE_ObjectSystemQuit(void);

void HE_ObjectSystemUpdate(void);
void HE_ObjectSystemRender(void);

typedef struct HE_Object HE_Object;
typedef void (*HE_ObjectRoutine)(HE_Object*);

struct HE_Object
{
    HE_ObjectRoutine routine, destructor;
    HE_ObjectRoutine render;

    HE_Spritesheet* spritesheet;
    float x, y;
    float xspeed, yspeed;
    int frame;
    float opacity, angle, xscale, yscale;

    SDL_bool visible : 1,
        spritesheet_owner : 1,
        xflip : 1,
        yflip : 1;

    Uint8 userdata[HE_OBJECT_USERDATA_SIZE];
};

/*
 * Reserved objects are always present in memory, and are used for objects
 * that are usually loaded from the start of the level. These objects are not
 * affected by the current screen position in the level.
 *
 * Dynamic objects, on the other hand, are only created if they are on
 * screen in the level. These objects are destroyed when they are
 * no longer on screen. This allows levels to be memory-efficient
 * and only load objects that are actually needed.
 */

// TODO: now the only thing to do is to actually make levels and dynamic object loading

HE_Object* HE_GetReservedObjects(void);
HE_Object* HE_GetDynamicObjects(void);

HE_Object* HE_InitializeReservedObject(HE_ObjectRoutine constructor);
HE_Object* HE_InitializeDynamicObject(HE_ObjectRoutine constructor);

// A convenience macro that references a label from an old Sonic 1 GitHub disassembly
#define HE_SingleObjLoad HE_InitializeDynamicObject

SDL_bool HE_ObjectExists(HE_Object* object);
SDL_bool HE_IsObjectDynamic(HE_Object* object);
void HE_DestroyObject(HE_Object* object);

// Advances the object's position based on its speed values.
void HE_SpeedToPos(HE_Object* object);
// Applies a constant downward force to the object, causing it to fall
// (it also calls HE_SpeedToPos internally).
void HE_ObjectFall(HE_Object* object);

#endif // __HE_OBJECT_H__