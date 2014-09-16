#include "input.h"

uint8_t inputStatus = 0;

void keyPress(uint8_t key)
{
    inputStatus |= key;
}

void keyRelease(uint8_t key)
{
    inputStatus &= ~key;
}

void keyClear()
{
    inputStatus = 0;
}

bool isPressed(uint8_t key)
{
    return inputStatus & key;
}