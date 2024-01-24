#define SDL_MAIN_HANDLED
#include "MDWindow.h"

int main(int argc, char* argv[])
{
    MD_Init();
    MD_Update();
    MD_Quit();
}