/**
 * @file utils.c
 * @brief Implementazione sicura delle utility di sistema e algoritmi ricorsivi.
 */
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void utils_clear_screen(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void utils_wait_enter(void) {
    int c;
    while ((c = getchar()) != '\n') {
        if (c == EOF) exit(0);
    }
}

int utils_get_int_input(void) {
    char line[64];
    if (!fgets(line, sizeof(line), stdin)) exit(0); // Gestione sicura EOF
    
    int value;
    char extra;
    if (sscanf(line, "%d %c", &value, &extra) == 1) {
        return value;
    }
    return -1;
}

int utils_rand_range(int min, int max) {
    return min + rand() % (max - min + 1);
}

/**
 * @brief Calcola il n-esimo numero di Tribonacci tramite ricorsione pura.
 * @param n Indice del numero di Tribonacci.
 * @param profondita_abisso Parametro per tracciare la profondita ricorsiva (come da PDF).
 * @return Valore n-esimo di Tribonacci.
 */
static long long utils_tribonacci_calc(int n, int profondita_abisso) {
    if (n <= 0) return 0;
    if (n == 1) return 0;
    if (n == 2) return 1;
    return utils_tribonacci_calc(n - 1, profondita_abisso + 1) +
           utils_tribonacci_calc(n - 2, profondita_abisso + 1) +
           utils_tribonacci_calc(n - 3, profondita_abisso + 1);
}

/**
 * @brief Wrapper ricorsivo per la ricerca di un target nella sequenza.
 * @param target Il numero da cercare.
 * @param n L'indice corrente valutato.
 * @return true se trovato, false se superato.
 */
static bool check_tribonacci_rec(int target, int n) {
    long long t = utils_tribonacci_calc(n, 0);
    if (t == target) return true;
    if (t > target) return false;
    return check_tribonacci_rec(target, n + 1);
}

bool check_tribonacci(int num) {
    if (num == 0) return true;
    return check_tribonacci_rec(num, 2);
}