#include "HydrogenEngine.h"
#include "SDL.h"
#include "SDL_image.h"

static SDL_Window* wnd;

static float fb_zoom_factor = 1.0f;
static SDL_bool running = SDL_TRUE;

SDL_Window* HE_GetWindow(void)
{
    return wnd;
}

void HE_UpdateFBZoom(void)
{
    int w, h;
    SDL_GetWindowSize(wnd, &w, &h);
    float zoomx = 1.0f * w / HE_GetFramebuffer()->w,
        zoomy = 1.0f * h / HE_GetFramebuffer()->h;
    fb_zoom_factor = SDL_min(zoomx, zoomy);
}

float HE_GetFBZoom(void)
{
    return fb_zoom_factor;
}

void HE_Close(void)
{
    running = SDL_FALSE;
}

void HE_Init(void)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);
    wnd = SDL_CreateWindow(HE_WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        HE_WINDOW_WIDTH, HE_WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);

    HE_InputSystemInit();
    HE_PaletteInit();
    HE_RenderInit();
    HE_ObjectSystemInit();

    HE_UpdateFBZoom();

    GameInit();
}

void HE_Update(void)
{
    SDL_Event event;

    while (running)
    {
        HE_InputSystemUpdate();
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    HE_Close();
                    break;

                case SDL_WINDOWEVENT:
                    switch (event.window.event)
                    {
                        case SDL_WINDOWEVENT_RESIZED:
                            HE_UpdateFBZoom();
                            break;
                    }
                    break;
            }

            HE_InputHandleEvent(&event);
        }

        // Update
        HE_ResetCurrentColorPalette();
        GameUpdate();

        // Software rendering
        HE_FillSurface(0, 0);
        GameRender();

        // Hardware rendering
        HE_PreTextureRender();

        SDL_Surface* fb_surface = HE_LockFBTexture();
        SDL_BlitSurface(HE_GetFramebuffer(), NULL, fb_surface, NULL);
        HE_UnlockFBTexture();
        HE_RenderFBTextureToScreen();

        HE_PostTextureRender();
    }
}

void HE_Quit(void)
{
    GameQuit();

    HE_ObjectSystemQuit();
    HE_RenderQuit();
    HE_PaletteQuit();
    HE_InputSystemQuit();

    SDL_DestroyWindow(wnd);
    IMG_Quit();
    SDL_Quit();
}