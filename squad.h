/**
 * @file squad.h
 * @brief Gestione della squadra dei naufraghi tramite lista dinamica.
 */

#ifndef SQUAD_H
#define SQUAD_H

#include <stdbool.h>

/**
 * @def MAX_HP
 * @brief Punti salute massimi consentiti per ogni naufrago.
 */
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

/**
 * @brief Alloca e inizializza una nuova squadra vuota.
 * @return Puntatore alla struttura Squad allocata, oppure NULL in caso di errore di allocazione.
 */
Squad* squad_create(void);

/**
 * @brief Aggiunge un nuovo naufrago in coda alla squadra con un ID univoco.
 * @param squad Puntatore alla squadra (non fa nulla se NULL).
 * @param initial_hp Punti salute iniziali del naufrago (limitati a MAX_HP se superiori).
 */
void squad_add_member(Squad* squad, int initial_hp);

/**
 * @brief Rimuove dalla squadra e dealloca il naufrago con l'ID specificato.
 * @param squad Puntatore alla squadra (non fa nulla se NULL o vuota).
 * @param id Identificativo del naufrago da rimuovere (nessun effetto se non trovato).
 */
void squad_remove_member(Squad* squad, int id);

/**
 * @brief Dealloca tutti i nodi dei naufraghi e la struttura Squad stessa.
 * @param squad Puntatore alla squadra da liberare (nessun effetto se NULL).
 */
void squad_cleanup(Squad* squad);

/**
 * @brief Stampa a video l'elenco dei naufraghi attivi con i rispettivi punti salute.
 * @param squad Puntatore alla squadra da stampare (stampa un messaggio se NULL o vuota).
 */
void squad_print_status(const Squad* squad);

/**
 * @brief Conta il numero di naufraghi attualmente presenti nella squadra.
 * @param squad Puntatore alla squadra.
 * @return Numero di naufraghi vivi nella squadra, oppure 0 se squad e' NULL o vuota.
 */
int squad_count(const Squad* squad);

/**
 * @brief Verifica se la squadra e' priva di naufraghi.
 * @param squad Puntatore alla squadra.
 * @return true se squad e' NULL o non contiene membri, false altrimenti.
 */
bool squad_is_empty(const Squad* squad);

/**
 * @brief Cerca un naufrago nella squadra tramite il suo identificativo univoco.
 * @param squad Puntatore alla squadra.
 * @param id Identificativo del naufrago da cercare.
 * @return Puntatore al nodo Survivor trovato, oppure NULL se squad e' NULL o l'ID non esiste.
 */
Survivor* squad_find_member(Squad* squad, int id);

#endif