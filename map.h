/**
 * @file map.h
 * @brief Gestione della mappa procedurale 9x9 e nebbia di guerra.
 */

#ifndef MAP_H
#define MAP_H

#include <stdbool.h>

/**
 * @def MAP_SIZE
 * @brief Dimensione del lato della mappa quadrata dell'isola (9x9).
 */
#define MAP_SIZE 9

/**
 * @enum CellState
 * @brief Stato di visibilita' ed esplorazione di una cella della mappa.
 */
typedef enum {
    HIDDEN,   /**< Cella coperta dalla nebbia di guerra ('?'). */
    REVEALED, /**< Cella visibile sulla mappa ma non ancora visitata (simbolo minuscolo). */
    VISITED   /**< Cella gia' esplorata dal giocatore (simbolo maiuscolo). */
} CellState;

/**
 * @enum TerrainType
 * @brief Tipologia di terreno o luogo speciale presente in una cella.
 */
typedef enum {
    BEACH,      /**< Spiaggia lungo il perimetro dell'isola. */
    FOREST,     /**< Foresta interna ricca di risorse e insidie. */
    SWAMP,      /**< Palude con rischio di sanguisughe. */
    HILL,       /**< Collina che rivela le celle adiacenti quando scalata. */
    SPRING,     /**< Sorgente d'acqua dolce. */
    PATH,       /**< Sentiero collegato che si rivela a catena. */
    WRECK,      /**< Relitto sulla costa contenente l'Olio per il Faro. */
    CAVE,       /**< Grotta interna contenente la Lente per il Faro. */
    LAIR,       /**< Tana interna custodita da un giaguaro con la Chiave. */
    LIGHTHOUSE, /**< Faro sulla costa, obiettivo della missione finale. */
    CAMP        /**< Accampamento base dei naufraghi. */
} TerrainType;

/**
 * @struct Cell
 * @brief Rappresenta una singola casella della griglia dell'isola.
 */
typedef struct {
    TerrainType type; /**< Tipologia di terreno della casella. */
    CellState state;  /**< Stato attuale di visibilita' della casella. */
} Cell;

/**
 * @brief Matrice bidimensionale globale che rappresenta l'intera mappa dell'isola.
 */
extern Cell map[MAP_SIZE][MAP_SIZE];

/**
 * @brief Coordinata di riga dell'accampamento base.
 */
extern int camp_r;

/**
 * @brief Coordinata di colonna dell'accampamento base.
 */
extern int camp_c;

/**
 * @brief Genera proceduralmente i terreni, l'accampamento e i luoghi speciali della mappa.
 */
void map_generate(void);

/**
 * @brief Stampa a video la mappa dell'isola con la legenda dei simboli.
 * @param player_r Riga corrente dell'esploratore.
 * @param player_c Colonna corrente dell'esploratore.
 * @param player_active Se true, mostra il simbolo '#' nella posizione dell'esploratore.
 */
void map_print(int player_r, int player_c, bool player_active);

/**
 * @brief Rivela tutte le celle di tipo PATH collegate ortogonalmente alla posizione indicata.
 * @param r Riga di partenza del sentiero.
 * @param c Colonna di partenza del sentiero.
 */
void map_reveal_paths(int r, int c);

/**
 * @brief Rivela le celle nascoste nell'intorno 3x3 centrato sulla collina indicata.
 * @param r Riga della collina scalata.
 * @param c Colonna della collina scalata.
 */
void map_reveal_around_hill(int r, int c);

#endif