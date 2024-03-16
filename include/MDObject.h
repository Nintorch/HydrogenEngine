#ifndef __MDOBJECT_H__
#define __MDOBJECT_H__

#include "MDRender.h"
#include "SDL_stdinc.h"

#define MD_RESERVED_OBJECT_COUNT    10
#define MD_DYNAMIC_OBJECT_COUNT     90
#define MD_TOTAL_OBJECTS (MD_RESERVED_OBJECT_COUNT + MD_DYNAMIC_OBJECT_COUNT)

#define MD_OBJECT_USERDATA_SIZE     20

void MD_ObjectSystemInit(void);
void MD_ObjectSystemQuit(void);

void MD_ObjectSystemUpdate(void);
void MD_ObjectSystemRender(void);

typedef struct MD_Object MD_Object;
typedef void (*MD_ObjectRoutine)(MD_Object*);

struct MD_Object
{
    MD_ObjectRoutine routine, destructor;
    MD_ObjectRoutine render;

    MD_Spritesheet* spritesheet;
    float x, y;
    float xspeed, yspeed;
    int frame;
    float opacity, angle, xscale, yscale;

    SDL_bool visible : 1,
        spritesheet_owner : 1,
        xflip : 1,
        yflip : 1;

    Uint8 userdata[MD_OBJECT_USERDATA_SIZE];
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

MD_Object* MD_GetReservedObjects(void);
MD_Object* MD_GetDynamicObjects(void);

MD_Object* MD_InitializeReservedObject(MD_ObjectRoutine constructor);
MD_Object* MD_InitializeDynamicObject(MD_ObjectRoutine constructor);

// A convenience macro that references a label from an old Sonic 1 GitHub disassembly
#define MD_SingleObjLoad MD_InitializeDynamicObject

SDL_bool MD_ObjectExists(MD_Object* object);
SDL_bool MD_IsObjectDynamic(MD_Object* object);
void MD_DestroyObject(MD_Object* object);

// Advances the object's position based on its speed values.
void MD_SpeedToPos(MD_Object* object);
// Applies a constant downward force to the object, causing it to fall
// (it also calls MD_SpeedToPos internally).
void MD_ObjectFall(MD_Object* object);

#endif // __MDOBJECT_H__