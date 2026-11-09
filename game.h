/**
 * @file game.h
 * @brief Strutture e firme per la gestione dello stato della partita.
 */
#ifndef GAME_H
#define GAME_H

#include "squad.h"
#include <stdbool.h>

/**
 * @struct Warehouse
 * @brief Rappresenta le risorse e le strutture costruite nell'accampamento.
 */
typedef struct {
    int food;               /**< Unità di cibo disponibili */
    int water;              /**< Unità di acqua disponibili */
    int wood;               /**< Unità di legna disponibili */
    int rope;               /**< Unità di corda disponibili */
    bool has_oil;           /**< Flag per l'Olio del Faro (Relitto) */
    bool has_lens;          /**< Flag per la Lente del Faro (Grotta) */
    bool has_key;           /**< Flag per la Chiave del Faro (Tana) */
    bool net_built;         /**< Flag costruzione Rete per pesci */
    bool cistern_built;     /**< Flag costruzione Cisterna */
    bool spring_discovered; /**< Flag bonus Sorgente trovata */
} Warehouse;

/**
 * @struct TrickSettings
 * @brief Mantiene la configurazione personalizzata dei trucchi.
 */
typedef struct {
    int initial_hp;         /**< HP di partenza forzati */
    int num_survivors;      /**< Numero iniziale di naufraghi */
    int food;               /**< Cibo iniziale forzato */
    int water;              /**< Acqua iniziale forzata */
    int wood;               /**< Legna iniziale forzata */
    int rope;               /**< Corda iniziale forzata */
    bool reveal_map;        /**< Se true, ignora nebbia di guerra */
    bool unlock_final;      /**< Se true, assegna oggetti chiave subito */
} TrickSettings;

/**
 * @brief Avvia il ciclo del Menu Principale.
 */
void game_main_menu(void);

#endif