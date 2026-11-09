/**
 * @file map.h
 * @brief Gestione della mappa procedurale 9x9 e nebbia di guerra.
 */

#ifndef MAP_H
#define MAP_H

#include <stdbool.h>

#define MAP_SIZE 9

typedef enum { HIDDEN, REVEALED, VISITED } CellState;
typedef enum { BEACH, FOREST, SWAMP, HILL, SPRING, PATH, WRECK, CAVE, LAIR, LIGHTHOUSE, CAMP } TerrainType;

typedef struct {
    TerrainType type;
    CellState state;
} Cell;

extern Cell map[MAP_SIZE][MAP_SIZE];
extern int camp_r, camp_c;

void map_generate(void);
void map_print(int player_r, int player_c, bool player_active);
void map_reveal_paths(int r, int c);
void map_reveal_around_hill(int r, int c);

#endif