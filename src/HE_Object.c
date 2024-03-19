#include "HydrogenEngine.h"
#include <stdio.h>
#include <memory.h>

static HE_Object objects[HE_TOTAL_OBJECTS];

static void render_object(HE_Object* object)
{
    HE_RenderSpritesheetEx(object->spritesheet, object->frame,
        (int)object->x, (int)object->y,
        (object->xflip ? -1.0 : 1.0) * object->xscale,
        (object->yflip ? -1.0 : 1.0) * object->yscale,
        object->angle, object->opacity);
}

static void empty_routine(HE_Object* object) {}

static void setup_object(HE_Object* object)
{
    memset(object, 0, sizeof(HE_Object));
    object->visible = SDL_TRUE;
    object->xscale = object->yscale = object->opacity = 1.0f;
    object->render = render_object;
    object->routine = empty_routine;
    object->destructor = empty_routine;
}

void HE_ObjectSystemInit(void)
{
    memset(objects, 0, sizeof(objects));
}

void HE_ObjectSystemQuit(void)
{
    for (int i = 0; i < HE_TOTAL_OBJECTS; i++)
        HE_DestroyObject(objects + i);
}

static void update_object(HE_Object* object)
{
    object->routine(object);
}

void HE_ObjectSystemUpdate(void)
{
    for (int i = 0; i < HE_TOTAL_OBJECTS; i++)
    {
        HE_Object* object = objects + i;
        if (!HE_ObjectExists(object))
            continue;
        update_object(object);
    }
}

void HE_ObjectSystemRender(void)
{
    for (int i = 0; i < HE_TOTAL_OBJECTS; i++)
    {
        HE_Object* object = objects + i;
        if (!HE_ObjectExists(object) || !object->visible)
            continue;
        object->render(object);
    }
}

HE_Object* HE_GetReservedObjects(void)
{
    return objects + 0;
}

HE_Object* HE_GetDynamicObjects(void)
{
    return objects + HE_RESERVED_OBJECT_COUNT;
}

static HE_Object* prepare_object(HE_Object* object, HE_ObjectRoutine constructor)
{
    setup_object(object);
    constructor(object);
    return object;
}

HE_Object* HE_InitializeReservedObject(HE_ObjectRoutine constructor)
{
    for (int i = 0; i < HE_RESERVED_OBJECT_COUNT; i++)
    {
        HE_Object* object = HE_GetReservedObjects() + i;
        if (!HE_ObjectExists(object))
            return prepare_object(object, constructor);
    }
    printf("HE_InitializeReservedObject: no free object slots\n");
    return NULL;
}

HE_Object* HE_InitializeDynamicObject(HE_ObjectRoutine constructor)
{
    for (int i = 0; i < HE_DYNAMIC_OBJECT_COUNT; i++)
    {
        HE_Object* object = HE_GetDynamicObjects() + i;
        if (!HE_ObjectExists(object))
            return prepare_object(object, constructor);
    }
    printf("HE_InitializeDynamicObject: no free object slots\n");
    return NULL;
}

SDL_bool HE_ObjectExists(HE_Object* object)
{
    return object != NULL && object->routine != NULL;
}

SDL_bool HE_IsObjectDynamic(HE_Object* object)
{
    return object >= HE_GetDynamicObjects()
        && object < HE_GetDynamicObjects() + HE_DYNAMIC_OBJECT_COUNT;
}

void HE_DestroyObject(HE_Object* object)
{
    if (!HE_ObjectExists(object))
        return;

    object->destructor(object);

    if (object->spritesheet_owner && object->spritesheet)
        HE_FreeSpritesheet(object->spritesheet);

    // Remember that objects are NOT dynamically allocated
    memset(object, 0, sizeof(HE_Object));
}

void HE_SpeedToPos(HE_Object* object)
{
    object->x += object->xspeed;
    object->y += object->yspeed;
}

void HE_ObjectFall(HE_Object* object)
{
    object->yspeed += 0x38 / 256.f;
    HE_SpeedToPos(object);
}