/**
 * @file main.c
 * @brief Point of Entry principale dell'applicazione.
 */

#include "game.h"
#include <time.h>
#include <stdlib.h>

/**
 * @brief Punto di ingresso dell'applicazione; inizializza il generatore casuale e avvia il menu principale.
 * @return 0 al termine regolare del programma.
 */
int main(void) {
    srand((unsigned int)time(NULL));
    game_main_menu();
    return 0;
}