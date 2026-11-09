/**
 * @file utils.h
 * @brief Dichiarazioni delle funzioni di utilità per input, casualità e matematica.
 */
#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

/**
 * @brief Pulisce il terminale in modo cross-platform.
 */
void utils_clear_screen(void);

/**
 * @brief Mette in pausa l'esecuzione attendendo la pressione di INVIO. Gestisce EOF.
 */
void utils_wait_enter(void);

/**
 * @brief Legge un intero da standard input gestendo buffer sporchi ed EOF.
 * @return L'intero letto, oppure -1 in caso di input non valido.
 */
int utils_get_int_input(void);

/**
 * @brief Genera un numero casuale compreso in un intervallo.
 * @param min Valore minimo (incluso).
 * @param max Valore massimo (incluso).
 * @return Numero casuale generato.
 */
int utils_rand_range(int min, int max);

/**
 * @brief Verifica se un numero appartiene alla sequenza di Tribonacci (algoritmo ricorsivo).
 * @param num Il numero da cercare nella sequenza.
 * @return true se appartiene, false altrimenti.
 */
bool check_tribonacci(int num);

#endif