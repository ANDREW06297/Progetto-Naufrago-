/**
 * @file map.c
 * @brief Generazione procedurale e rivelazione grafica della mappa di gioco.
 */

#include "map.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

Cell map[MAP_SIZE][MAP_SIZE];
int camp_r = 4;
int camp_c = 4;

static void flood_fill_paths_helper(int r, int c, bool visited[MAP_SIZE][MAP_SIZE]) {
    if (r < 0 || r >= MAP_SIZE || c < 0 || c >= MAP_SIZE) return;
    if (visited[r][c]) return;
    if (map[r][c].type != PATH) return;

    visited[r][c] = true;
    if (map[r][c].state == HIDDEN) {
        map[r][c].state = REVEALED;
    }

    flood_fill_paths_helper(r - 1, c, visited);
    flood_fill_paths_helper(r + 1, c, visited);
    flood_fill_paths_helper(r, c - 1, visited);
    flood_fill_paths_helper(r, c + 1, visited);
}

void map_reveal_paths(int r, int c) {
    bool visited[MAP_SIZE][MAP_SIZE] = {false};
    flood_fill_paths_helper(r, c, visited);
}

void map_reveal_around_hill(int r, int c) {
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            int nr = r + dr;
            int nc = c + dc;
            if (nr >= 0 && nr < MAP_SIZE && nc >= 0 && nc < MAP_SIZE) {
                if (map[nr][nc].state == HIDDEN) {
                    map[nr][nc].state = REVEALED;
                }
            }
        }
    }
}

void map_generate(void) {
    // 1. Generazione terreni di base (Tabella 3)
    for (int r = 0; r < MAP_SIZE; r++) {
        for (int c = 0; c < MAP_SIZE; c++) {
            if (r == 0 || r == MAP_SIZE - 1 || c == 0 || c == MAP_SIZE - 1) {
                map[r][c].type = BEACH;
            } else {
                int roll = utils_rand_range(1, 6);
                if (roll <= 2) map[r][c].type = FOREST;
                else if (roll == 3) map[r][c].type = SWAMP;
                else if (roll == 4) map[r][c].type = HILL;
                else if (roll == 5) map[r][c].type = SPRING;
                else map[r][c].type = PATH;
            }
            map[r][c].state = HIDDEN;
        }
    }

    // Accampamento centrale
    map[camp_r][camp_c].type = CAMP;
    map[camp_r][camp_c].state = VISITED;

    // 2. Posizionamento Luoghi Speciali del Bordo (Relitto e Faro) su due caselle distinte
    int r1, c1, r2, c2;
    int edge1 = utils_rand_range(0, 3);
    int pos1 = utils_rand_range(1, MAP_SIZE - 2);
    if (edge1 == 0) { r1 = 0; c1 = pos1; }
    else if (edge1 == 1) { r1 = MAP_SIZE - 1; c1 = pos1; }
    else if (edge1 == 2) { r1 = pos1; c1 = 0; }
    else { r1 = pos1; c1 = MAP_SIZE - 1; }

    int edge2, pos2;
    do {
        edge2 = utils_rand_range(0, 3);
        pos2 = utils_rand_range(1, MAP_SIZE - 2);
        if (edge2 == 0) { r2 = 0; c2 = pos2; }
        else if (edge2 == 1) { r2 = MAP_SIZE - 1; c2 = pos2; }
        else if (edge2 == 2) { r2 = pos2; c2 = 0; }
        else { r2 = pos2; c2 = MAP_SIZE - 1; }
    } while (r1 == r2 && c1 == c2);

    map[r1][c1].type = WRECK;
    map[r2][c2].type = LIGHTHOUSE;

    // 3. Posizionamento Luoghi Speciali Interni (Grotta e Tana) a distanza Manhattan >= 3
    int gr, gc, tr, tc;
    do {
        gr = utils_rand_range(1, MAP_SIZE - 2);
        gc = utils_rand_range(1, MAP_SIZE - 2);
    } while ((gr == camp_r && gc == camp_c) || (abs(gr - camp_r) + abs(gc - camp_c) < 3));
    map[gr][gc].type = CAVE;

    do {
        tr = utils_rand_range(1, MAP_SIZE - 2);
        tc = utils_rand_range(1, MAP_SIZE - 2);
    } while ((tr == camp_r && tc == camp_c) || (abs(tr - camp_r) + abs(tc - camp_c) < 3) || (gr == tr && gc == tc));
    map[tr][tc].type = LAIR;

    // 4. Garantire almeno una Sorgente (eseguita DOPO i luoghi speciali)
    bool has_spring = false;
    for (int r = 1; r < MAP_SIZE - 1; r++) {
        for (int c = 1; c < MAP_SIZE - 1; c++) {
            if (map[r][c].type == SPRING) has_spring = true;
        }
    }

    if (!has_spring) {
        int sr, sc;
        do {
            sr = utils_rand_range(1, MAP_SIZE - 2);
            sc = utils_rand_range(1, MAP_SIZE - 2);
        } while (map[sr][sc].type == CAMP || map[sr][sc].type == CAVE || map[sr][sc].type == LAIR);
        map[sr][sc].type = SPRING;
    }
}

static char map_get_symbol(TerrainType type, CellState state) {
    if (state == HIDDEN) return '?';
    if (type == CAMP) return '@';

    char upper = '?';
    if (type == BEACH) upper = 'B';
    else if (type == FOREST) upper = 'F';
    else if (type == SWAMP) upper = 'P';
    else if (type == HILL) upper = 'C';
    else if (type == SPRING) upper = 'O';
    else if (type == PATH) upper = '=';
    else if (type == WRECK) upper = 'R';
    else if (type == CAVE) upper = 'G';
    else if (type == LAIR) upper = 'T';
    else if (type == LIGHTHOUSE) upper = 'L';

    if (state == VISITED) return upper;

    if (upper >= 'A' && upper <= 'Z') return upper + 32;
    if (upper == '=') return '-';
    return '?';
}

void map_print(int player_r, int player_c, bool player_active) {
    printf("   0 1 2 3 4 5 6 7 8\n");
    for (int r = 0; r < MAP_SIZE; r++) {
        printf("%d  ", r);
        for (int c = 0; c < MAP_SIZE; c++) {
            if (player_active && r == player_r && c == player_c) {
                printf("# ");
            } else {
                printf("%c ", map_get_symbol(map[r][c].type, map[r][c].state));
            }
        }
        printf("\n");
    }
    printf("Legenda: # naufrago @ Accampamento b/B Spiaggia f/F Foresta p/P Palude c/C Collina o/O Sorgente -/= Sentiero r/R Relitto g/G Grotta t/T Tana l/L Faro ? nascosta\n");
}