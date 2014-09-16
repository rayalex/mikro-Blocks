#ifndef INPUT_H
#define INPUT_H

#include "stdint.h"
#include "stdbool.h"

#define KEY_LEFT    (1 << 0)
#define KEY_RIGHT   (1 << 1)
#define KEY_DOWN    (1 << 2)
#define KEY_UP      (1 << 3)
#define KEY_RCW     (1 << 4)
#define KEY_RCCW    (1 << 5)

// bit field input status
extern uint8_t inputStatus;

/**
 * Processes the key press.
 * @param key Pressed key.
 */
void keyPress(uint8_t key);

/**
 * Processes the key release.
 * @param key Released key.
 */
void keyRelease(uint8_t key);

/**
 * Clears all pressed keys.
 */
void keyClear();

/**
 * Determines if the key is pressed.
 * @param  key Key to check pressed state for.
 * @return True if the key is pressed, false otherwise.
 */
bool isPressed(uint8_t key);

#endif