/**
 * @file squad.h
 * @brief Gestione della squadra dei naufraghi tramite lista dinamica.
 */

#ifndef SQUAD_H
#define SQUAD_H

#include <stdbool.h>

#define MAX_HP 10

/**
 * @struct Survivor
 * @brief Nodo della lista dinamica rappresentante un naufrago.
 */
typedef struct Survivor {
    int id;                 /**< Identificativo univoco progressivo */
    int hp;                 /**< Punti salute attuali (max MAX_HP) */
    struct Survivor* next;  /**< Puntatore al prossimo naufrago */
} Survivor;

/**
 * @struct Squad
 * @brief Struttura di controllo della squadra.
 */
typedef struct {
    Survivor* head;         /**< Testa della lista dinamica */
    int next_id_counter;    /**< Contatore per ID univoci mai riutilizzati */
} Squad;

Squad* squad_create(void);
void squad_add_member(Squad* squad, int initial_hp);
void squad_remove_member(Squad* squad, int id);
void squad_cleanup(Squad* squad);
void squad_print_status(const Squad* squad);
int squad_count(const Squad* squad);
bool squad_is_empty(const Squad* squad);
Survivor* squad_find_member(Squad* squad, int id);

#endif