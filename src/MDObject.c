#include "MD.h"
#include <stdio.h>
#include <memory.h>

static MD_Object objects[MD_TOTAL_OBJECTS];

static void render_object(MD_Object* object)
{
    MD_RenderSpritesheetEx(object->spritesheet, object->frame,
        (int)object->x, (int)object->y,
        (object->xflip ? -1.0 : 1.0) * object->xscale,
        (object->yflip ? -1.0 : 1.0) * object->yscale,
        object->angle, object->opacity);
}

static void empty_routine(MD_Object* object) {}

static void setup_object(MD_Object* object)
{
    memset(object, 0, sizeof(MD_Object));
    object->visible = SDL_TRUE;
    object->xscale = object->yscale = object->opacity = 1.0f;
    object->render = render_object;
    object->routine = empty_routine;
    object->destructor = empty_routine;
}

void MD_ObjectSystemInit(void)
{
    memset(objects, 0, sizeof(objects));
}

void MD_ObjectSystemQuit(void)
{
    for (int i = 0; i < MD_TOTAL_OBJECTS; i++)
        MD_DestroyObject(objects + i);
}

static void update_object(MD_Object* object)
{
    object->routine(object);
}

void MD_ObjectSystemUpdate(void)
{
    for (int i = 0; i < MD_TOTAL_OBJECTS; i++)
    {
        MD_Object* object = objects + i;
        if (!MD_ObjectExists(object))
            continue;
        update_object(object);
    }
}

void MD_ObjectSystemRender(void)
{
    for (int i = 0; i < MD_TOTAL_OBJECTS; i++)
    {
        MD_Object* object = objects + i;
        if (!MD_ObjectExists(object) || !object->visible)
            continue;
        object->render(object);
    }
}

MD_Object* MD_GetReservedObjects(void)
{
    return objects + 0;
}

MD_Object* MD_GetDynamicObjects(void)
{
    return objects + MD_RESERVED_OBJECT_COUNT;
}

static MD_Object* prepare_object(MD_Object* object, MD_ObjectRoutine constructor)
{
    setup_object(object);
    constructor(object);
    return object;
}

MD_Object* MD_InitializeReservedObject(MD_ObjectRoutine constructor)
{
    for (int i = 0; i < MD_RESERVED_OBJECT_COUNT; i++)
    {
        MD_Object* object = MD_GetReservedObjects() + i;
        if (!MD_ObjectExists(object))
            return prepare_object(object, constructor);
    }
    printf("MD_InitializeReservedObject: no free object slots\n");
    return NULL;
}

MD_Object* MD_InitializeDynamicObject(MD_ObjectRoutine constructor)
{
    for (int i = 0; i < MD_DYNAMIC_OBJECT_COUNT; i++)
    {
        MD_Object* object = MD_GetDynamicObjects() + i;
        if (!MD_ObjectExists(object))
            return prepare_object(object, constructor);
    }
    printf("MD_InitializeDynamicObject: no free object slots\n");
    return NULL;
}

SDL_bool MD_ObjectExists(MD_Object* object)
{
    return object != NULL && object->routine != NULL;
}

SDL_bool MD_IsObjectDynamic(MD_Object* object)
{
    return object >= MD_GetDynamicObjects()
        && object < MD_GetDynamicObjects() + MD_DYNAMIC_OBJECT_COUNT;
}

void MD_DestroyObject(MD_Object* object)
{
    if (!MD_ObjectExists(object))
        return;

    object->destructor(object);

    if (object->spritesheet_owner && object->spritesheet)
        MD_FreeSpritesheet(object->spritesheet);

    // Remember that objects are NOT dynamically allocated
    memset(object, 0, sizeof(MD_Object));
}

void MD_SpeedToPos(MD_Object* object)
{
    object->x += object->xspeed;
    object->y += object->yspeed;
}

void MD_ObjectFall(MD_Object* object)
{
    object->yspeed += 0x38 / 256.f;
    MD_SpeedToPos(object);
}