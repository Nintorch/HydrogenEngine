#include "HE_Utils.h"

int HE_UtilsOffset(int x, int size, int offset)
{
    x += offset;
    x %= size;
    if (x < 0)
        x += size;
    return x;
}