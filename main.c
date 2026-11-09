/**
 * @file main.c
 * @brief Point of Entry principale dell'applicazione.
 */

#include "game.h"
#include <time.h>
#include <stdlib.h>

int main(void) {
    srand((unsigned int)time(NULL));
    game_main_menu();
    return 0;
}