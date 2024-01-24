#include "MD.h"
#include "SDL.h"
#include "SDL_image.h"

static SDL_Window* wnd;

static float fb_zoom_factor = 1.0f;
static SDL_bool running = SDL_TRUE;

SDL_Window* MD_GetWindow(void)
{
    return wnd;
}

void MD_UpdateFBZoom()
{
    int w, h;
    SDL_GetWindowSize(wnd, &w, &h);
    float zoomx = 1.0f * w / MD_GetFramebuffer()->w,
        zoomy = 1.0f * h / MD_GetFramebuffer()->h;
    fb_zoom_factor = SDL_min(zoomx, zoomy);
}

float MD_GetFBZoom()
{
    return fb_zoom_factor;
}

void MD_Close()
{
    running = SDL_FALSE;
}

void MD_Init(void)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);
    wnd = SDL_CreateWindow(MD_WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        MD_WINDOW_WIDTH, MD_WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);

    MD_InputSystemInit();
    MD_PaletteInit();
    MD_RenderInit();
    MD_ObjectSystemInit();

    MD_UpdateFBZoom();

    GameInit();
}

void MD_Update(void)
{
    SDL_Event event;

    while (running)
    {
        MD_InputSystemUpdate();
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    MD_Close();
                    break;

                case SDL_WINDOWEVENT:
                    switch (event.window.event)
                    {
                        case SDL_WINDOWEVENT_RESIZED:
                            MD_UpdateFBZoom();
                            break;
                    }
                    break;
            }

            MD_InputHandleEvent(&event);
        }

        // Update
        MD_ResetColorPalette();
        GameUpdate();

        // Software rendering
        MD_FillSurface(MD_GetFramebuffer(), 0, 4);
        GameFramebufferRender(MD_GetFramebuffer());

        // Hardware rendering
        MD_PreTextureRender();

        SDL_Surface* fb_surface = MD_LockFBTexture();
        MD_RenderFBToSurface(fb_surface);
        GameTextureRender(fb_surface);
        MD_UnlockFBTexture();
        MD_RenderFBTextureToScreen();

        MD_PostTextureRender();
    }
}

void MD_Quit(void)
{
    GameQuit();

    // MD_ObjectSystemQuit();
    MD_RenderQuit();
    MD_PaletteQuit();
    MD_InputSystemQuit();

    SDL_DestroyWindow(wnd);
    IMG_Quit();
    SDL_Quit();
}