/**
 * @file game.c
 * @brief Implementazione del core loop, eventi, esplorazione e combattimento.
 */
#include "game.h"
#include "map.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int days = 1;
static Squad* team = NULL;
static Warehouse warehouse;
static TrickSettings trick_settings = {10, 4, 8, 8, 0, 0, false, false};

static int secret_code_mastermind[4];
static bool tricks_unlocked = false;
static bool game_won = false; // Flag per evitare il falso Game Over

static void check_key_items_message(void) {
    if (warehouse.has_oil && warehouse.has_lens && warehouse.has_key) {
        printf("\n*** AVETE TUTTO IL NECESSARIO PER RIACCENDERE IL FARO! ***\n");
    }
}

static void setup_new_game(void) {
    if (team) {
        squad_cleanup(team);
        team = NULL;
    }
    team = squad_create();
    for (int i = 0; i < trick_settings.num_survivors; i++) {
        squad_add_member(team, trick_settings.initial_hp);
    }
    
    warehouse.food = trick_settings.food;
    warehouse.water = trick_settings.water;
    warehouse.wood = trick_settings.wood;
    warehouse.rope = trick_settings.rope;
    warehouse.has_oil = false;
    warehouse.has_lens = false;
    warehouse.has_key = false;
    warehouse.net_built = false;
    warehouse.cistern_built = false;
    warehouse.spring_discovered = false;
    
    days = 1;
    game_won = false;

    map_generate();

    for (int i = 0; i < 4; i++) {
       secret_code_mastermind[i] = utils_rand_range(1, 9);
    }
    
    if (trick_settings.reveal_map) {
        for (int r = 0; r < MAP_SIZE; r++) {
            for (int c = 0; c < MAP_SIZE; c++) {
                if (map[r][c].state == HIDDEN) map[r][c].state = REVEALED;
            }
        }
    }
    if (trick_settings.unlock_final) {
        warehouse.has_oil = true;
        warehouse.has_lens = true;
        warehouse.has_key = true;
        for (int r = 0; r < MAP_SIZE; r++) {
            for (int c = 0; c < MAP_SIZE; c++) {
                if (map[r][c].type == LIGHTHOUSE) map[r][c].state = REVEALED;
            }
        }
    }
}

static void evening_phase(void) {
    printf("\n--- FASE SERALE (Giorno %d) ---\n", days);
    Survivor* current = team->head;

    while (current) {
        Survivor* next_survivor = current->next;
        bool missing_food = (warehouse.food > 0) ? (warehouse.food--, false) : true;
        bool missing_water = (warehouse.water > 0) ? (warehouse.water--, false) : true;

        if (missing_food) {
            current->hp -= 2;
            printf("Naufrago %d non ha mangiato! (-2 HP)\n", current->id);
        }
        if (missing_water) {
            current->hp -= 2;
            printf("Naufrago %d non ha bevuto! (-2 HP)\n", current->id);
        }

        if (current->hp <= 0) {
            printf("Naufrago %d e' morto di stenti.\n", current->id);
            squad_remove_member(team, current->id);
        }
        current = next_survivor;
    }

    if (warehouse.net_built && !squad_is_empty(team)) {
        warehouse.food++;
        printf("Effetto Rete per pesci: +1 Cibo.\n");
    }
    if (warehouse.cistern_built && !squad_is_empty(team)) {
        warehouse.water++;
        printf("Effetto Cisterna: +1 Acqua.\n");
    }
    days++;
}

static bool combat_encounter(Survivor* explorer, int min_fatal, int damage, const char* enemy_name) {
    printf("\n--- COMBATTIMENTO: %s ---\n", enemy_name);
    while (explorer->hp > 0) {
        printf("Premi INVIO per lanciare il dado d'attacco...");
        utils_wait_enter(); 
        
        int roll = utils_rand_range(1, 6);
        printf("Risultato dado: %d (Colpo Fatale: > %d)\n", roll, min_fatal);

        if (roll > min_fatal) {
            printf("Hai sconfitto %s!\n", enemy_name);
            return true;
        } else {
            explorer->hp -= damage;
            printf("Fallito! Subisci %d danni. (Salute: %d)\n", damage, explorer->hp > 0 ? explorer->hp : 0);
            if (explorer->hp <= 0) {
                printf("Naufrago %d sconfitto da %s...\n", explorer->id, enemy_name);
                return false;
            }
        }
    }
    return false;
}

static void do_expedition(void) {
    if (squad_is_empty(team)) return;

    printf("\nSquadra disponibile per la spedizione:\n");
    Survivor* curr = team->head;
    bool any_valid = false;
    while (curr) {
        if (curr->hp >= 3) {
            printf("- Naufrago %d (HP: %d/%d)\n", curr->id, curr->hp, MAX_HP);
            any_valid = true;
        }
        curr = curr->next;
    }

    if (!any_valid) {
        printf("Nessun naufrago ha abbastanza salute (minimo 3 HP).\n");
        printf("Premi INVIO per tornare..."); utils_wait_enter();
        return;
    }

    printf("Inserisci l'ID del naufrago scelto: ");
    int chosen_id = utils_get_int_input();
    Survivor* explorer = squad_find_member(team, chosen_id);

    if (!explorer || explorer->hp < 3) {
        printf("Scelta non valida.\n");
        printf("Premi INVIO per tornare..."); utils_wait_enter();
        return;
    }

    int pr = camp_r, pc = camp_c;
    int steps = 8;

    while (steps > 0 && explorer->hp > 0) {
        utils_clear_screen();
        printf("Mappa | Giorno %d | Passi: %d | Naufrago %d (%d HP)\n", days, steps, explorer->id, explorer->hp);
        map_print(pr, pc, true);
        printf("Seleziona direzione (w, a, s, d, r=rientra): ");
        
        char line[64];
        if (!fgets(line, sizeof(line), stdin)) exit(0);
        char dir = line[0];

        if (dir == 'r') {
            printf("Rientro anticipato.\n");
            break;
        }

        int nr = pr, nc = pc;
        if (dir == 'w') nr--;
        else if (dir == 's') nr++;
        else if (dir == 'a') nc--;
        else if (dir == 'd') nc++;
        else {
            printf("Input non valido!\n"); 
            printf("Premi INVIO per riprovare..."); utils_wait_enter();
            continue;
        }

        if (nr < 0 || nr >= MAP_SIZE || nc < 0 || nc >= MAP_SIZE) {
            printf("Impossibile uscire dall'isola!\n");
            printf("Premi INVIO per riprovare..."); utils_wait_enter();
            continue;
        }

        pr = nr; pc = nc;
        steps--;
        Cell* cell = &map[pr][pc];

        if (cell->state != VISITED || cell->type == WRECK || cell->type == CAVE || cell->type == LAIR || cell->type == LIGHTHOUSE) {
            
            if (cell->state != VISITED && cell->type == PATH) {
                printf("Sentiero visitato. Percorso rivelato.\n");
                map_reveal_paths(pr, pc);
            } else if (cell->state != VISITED && cell->type == HILL) {
                printf("Collina scalata. Dintorni rivelati.\n");
                map_reveal_around_hill(pr, pc);
            } else if (cell->state != VISITED && cell->type == SPRING) {
                printf("Sorgente scoperta! +4 Acqua.\n");
                warehouse.water += 4;
                warehouse.spring_discovered = true;
            } else if (cell->type == BEACH && cell->state != VISITED) {
                int roll = utils_rand_range(1, 6);
                if (roll == 1) { printf("Conchiglie, nulla di utile.\n"); }
                else if (roll == 2) { printf("Granchi! +2 Cibo.\n"); warehouse.food += 2; }
                else if (roll == 3) { printf("Legname! +2 Legna.\n"); warehouse.wood += 2; }
                else if (roll == 4) { printf("Barile! +1 Corda, +1 Cibo.\n"); warehouse.rope++; warehouse.food++; }
                else if (roll == 5) {
                    int dmg = utils_rand_range(1, 6);
                    printf("Scogli taglienti! -%d HP.\n", dmg);
                    explorer->hp -= dmg;
                } else if (roll == 6) {
                    printf("Superstite trovato! Unito con 6 HP.\n");
                    squad_add_member(team, 6);
                }
            } else if (cell->type == FOREST && cell->state != VISITED) {
                int roll = utils_rand_range(1, 6);
                if (roll == 1) { printf("Alberi da frutto! +2 Cibo.\n"); warehouse.food += 2; }
                else if (roll == 2) { printf("Tronchi caduti! +3 Legna.\n"); warehouse.wood += 3; }
                else if (roll == 3) {
                    if (utils_rand_range(0, 1) == 0) printf("Le scimmie scappano.\n");
                    else {
                        printf("Scimmie ladre! Rubano 2 Cibo.\n");
                        warehouse.food = (warehouse.food >= 2) ? warehouse.food - 2 : 0;
                    }
                } else if (roll == 4) {
                    if (combat_encounter(explorer, 3, 2, "Cinghiale")) {
                        warehouse.food += 4;
                        printf("+4 Cibo ottenuti.\n");
                    }
                } else if (roll == 5) {
                    combat_encounter(explorer, 3, 3, "Serpente");
                } else if (roll == 6) {
                    printf("Rovi! -2 HP, +1 Legna.\n");
                    explorer->hp -= 2; warehouse.wood++;
                }
            } else if (cell->type == SWAMP && cell->state != VISITED) {
                int roll = utils_rand_range(1, 6);
                if (roll <= 2) { printf("Sanguisughe! -2 HP.\n"); explorer->hp -= 2; }
                else { printf("+1 Cibo trovato.\n"); warehouse.food++; }
            } else if (cell->type == WRECK && !warehouse.has_oil) {
                printf("Relitto instabile! -2 HP.\n");
                explorer->hp -= 2;
                if (explorer->hp > 0) {
                    printf("Ottieni l'Olio e +3 Legna.\n");
                    warehouse.has_oil = true;
                    warehouse.wood += 3;
                    check_key_items_message();
                }
            } else if (cell->type == CAVE && !warehouse.has_lens) {
                int num = utils_rand_range(1, 500);
                printf("Grotta: il numero %d appartiene a Tribonacci? [1=Si / 0=No]: ", num);
                int ans = utils_get_int_input();
                bool is_trib = check_tribonacci(num);
                if ((ans == 1 && is_trib) || (ans == 0 && !is_trib)) {
                    printf("Corretto! Ottieni la Lente.\n");
                    warehouse.has_lens = true;
                    check_key_items_message();
                } else {
                    printf("Errato! Crollo parziale: -3 HP.\n");
                    explorer->hp -= 3;
                }
            } else if (cell->type == LAIR && !warehouse.has_key) {
                if (combat_encounter(explorer, 4, 4, "Giaguaro")) {
                    printf("Ottieni la Chiave e +4 Cibo.\n");
                    warehouse.has_key = true;
                    warehouse.food += 4;
                    check_key_items_message();
                }
            } else if (cell->type == LIGHTHOUSE) {
                if (!warehouse.has_oil || !warehouse.has_lens || !warehouse.has_key || warehouse.wood < 10) {
                    printf("Materiali insufficienti per il Faro (servono 3 Oggetti e 10 Legna).\n");
                } else {
                    printf("\n*** MISSIONE FINALE: MASTERMIND ***\n");
                    warehouse.wood -= 10;
                    
                    bool won = false;
                    for (int t = 1; t <= 8; t++) {
                        printf("\nTentativo %d/8. Inserisci 4 cifre (1-9): ", t);
                        char input_line[100];
                        if (!fgets(input_line, sizeof(input_line), stdin)) exit(0);

                        int digits[4], dummy;
                        int parsed = sscanf(input_line, "%d %d %d %d %d", &digits[0], &digits[1], &digits[2], &digits[3], &dummy);
                        
                        if (parsed != 4) {
                            char raw[10];
                            if (sscanf(input_line, "%9s", raw) == 1 && strlen(raw) == 4) {
                                parsed = 0;
                                for (int k = 0; k < 4; k++) {
                                    if (raw[k] >= '1' && raw[k] <= '9') {
                                        digits[k] = raw[k] - '0';
                                        parsed++;
                                    }
                                }
                            }
                        }

                        if (parsed != 4) {
                            printf("Errore: devi inserire ESATTAMENTE 4 cifre.\n");
                            t--; continue;
                        }

                        int right_pos = 0, right_val = 0;
                        int secret_used[4] = {0}, guess_used[4] = {0};

                        for (int k = 0; k < 4; k++) {
                            if (digits[k] == secret_code_mastermind[k]) {
                                right_pos++;
                                secret_used[k] = guess_used[k] = 1;
                            }
                        }

                        for (int k = 0; k < 4; k++) {
                            if (!guess_used[k]) {
                                for (int j = 0; j < 4; j++) {
                                    if (!secret_used[j] && (digits[k] == secret_code_mastermind[j])) {
                                        right_val++;
                                        secret_used[j] = 1;
                                        break;
                                    }
                                }
                            }
                        }

                        printf("Posizione Corretta: %d | Cifra Presente: %d\n", right_pos, right_val);

                        if (right_pos == 4) {
                            won = true;
                            break;
                        } else {
                            printf("La ruggine resiste ancora.\n");
                        }
                    }

                    if (won) {
                        utils_clear_screen();
                        printf("====================================================\n");
                        printf(" Siete stati salvati! VITTORIA al Giorno %d!\n", days);
                        printf("====================================================\n");
                        squad_print_status(team);
                        game_won = true; // FLAG VITTORIA SETTATO
                        printf("\nPremi INVIO per terminare..."); utils_wait_enter();
                        return; // Esce direttamente dalla spedizione
                    } else {
                        printf("Tentativi esauriti! Le legna si sono spente.\n");
                    }
                }
            }

            if (cell->type != LIGHTHOUSE && cell->type != CAVE && cell->type != LAIR && cell->type != WRECK) {
                cell->state = VISITED;
            } else if (cell->type == WRECK && warehouse.has_oil) cell->state = VISITED;
            else if (cell->type == CAVE && warehouse.has_lens) cell->state = VISITED;
            else if (cell->type == LAIR && warehouse.has_key) cell->state = VISITED;
        }

        if (explorer->hp > 0) {
            printf("Premi INVIO per continuare..."); utils_wait_enter();
        } else {
            printf("\nNaufrago %d morto in spedizione!\n", explorer->id);
            squad_remove_member(team, explorer->id);
            printf("Premi INVIO per continuare..."); utils_wait_enter();
            break;
        }
    }

    if (explorer && explorer->hp > 0) {
        printf("\nRitorni all'accampamento.\n");
        printf("Premi INVIO per continuare..."); utils_wait_enter();
    }
    
    // Se la partita non è stata vinta e c'è qualcuno vivo, passa la fase serale
    if (!game_won && !squad_is_empty(team)) {
        evening_phase();
    }
}

static void do_gather(void) {
    if (squad_is_empty(team)) return;
    Survivor* curr = team->head;
    bool gathered = false;

    while (curr) {
        if (curr->hp >= 3) {
            gathered = true;
            int roll = utils_rand_range(1, 6);
            if (roll == 1) { curr->hp -= 2; printf("N.%d ferito (-2 HP)\n", curr->id); }
            else if (roll == 2) { warehouse.wood += 2; printf("N.%d: +2 Legna\n", curr->id); }
            else if (roll == 3) { warehouse.food += 2; printf("N.%d: +2 Cibo\n", curr->id); }
            else if (roll == 4) { warehouse.water += 2; printf("N.%d: +2 Acqua\n", curr->id); }
            else if (roll == 5) { warehouse.food++; warehouse.water++; printf("N.%d: +1 Cibo, +1 Acqua\n", curr->id); }
            else if (roll == 6) { warehouse.food += 2; warehouse.water += 2; printf("N.%d: +2 Cibo, +2 Acqua\n", curr->id); }
        } else {
            printf("N.%d (HP:%d) troppo debole per raccogliere.\n", curr->id, curr->hp);
        }
        curr = curr->next;
    }

    if (gathered && warehouse.spring_discovered) {
        warehouse.water += 2;
        printf("Sorgente: +2 Acqua bonus complessive.\n");
    }

    printf("Premi INVIO per continuare..."); utils_wait_enter();
    evening_phase();
}

static void do_rest(void) {
    Survivor* curr = team->head;
    while (curr) {
        curr->hp += 3;
        if (curr->hp > MAX_HP) curr->hp = MAX_HP;
        curr = curr->next;
    }
    printf("Tutti i naufraghi hanno riposato.\n");
    printf("Premi INVIO per continuare..."); utils_wait_enter();
    evening_phase();
}

static void do_lab(void) {
    printf("1. Corda (2 Legna)\n2. Infuso (2 Cibo)\n3. Rete (2 L, 2 C)\n4. Cisterna (4 L, 1 C)\n5. Indietro\nScelta: ");
    int choice = utils_get_int_input();
    if (choice == 1 && warehouse.wood >= 2) { warehouse.wood -= 2; warehouse.rope++; printf("Corda creata.\n"); }
    else if (choice == 2 && warehouse.food >= 2) {
        printf("ID naufrago: "); int id = utils_get_int_input();
        Survivor* t = squad_find_member(team, id);
        if (t) { warehouse.food -= 2; t->hp += utils_rand_range(1, 6); if(t->hp>MAX_HP) t->hp=MAX_HP; printf("Curato!\n");}
        else { printf("ID Errato.\n"); }
    }
    else if (choice == 3 && warehouse.wood >= 2 && warehouse.rope >= 2 && !warehouse.net_built) {
        warehouse.wood -= 2; warehouse.rope -= 2; warehouse.net_built = true; printf("Rete costruita.\n");
    }
    else if (choice == 4 && warehouse.wood >= 4 && warehouse.rope >= 1 && !warehouse.cistern_built) {
        warehouse.wood -= 4; warehouse.rope--; warehouse.cistern_built = true; printf("Cisterna costruita.\n");
    }
    else if (choice != 5) printf("Azione fallita (risorse insufficienti o già costruita).\n");
    
    printf("Premi INVIO per continuare..."); utils_wait_enter();
}

static void game_camp_menu(void) {
    while (days <= 30 && !squad_is_empty(team) && !game_won) {
        utils_clear_screen();
        printf("=== Giorno %d/30 | Vivi: %d ===\n", days, squad_count(team));
        printf("1. Spedizione\n2. Raccogli\n3. Riposa\n4. Magazzino/Squadra\n5. Laboratorio\n6. Abbandona\nScelta: ");
        
        int choice = utils_get_int_input();
        if (choice == 1) do_expedition();
        else if (choice == 2) do_gather();
        else if (choice == 3) do_rest();
        else if (choice == 4) { squad_print_status(team); printf("Premi INVIO..."); utils_wait_enter(); }
        else if (choice == 5) do_lab();
        else if (choice == 6) {
            printf("Uscire? [1=Si/0=No]: ");
            if (utils_get_int_input() == 1) { squad_cleanup(team); team = NULL; return; }
        } else {
            printf("Scelta non valida.\n"); printf("Premi INVIO..."); utils_wait_enter();
        }
    }

    if (game_won) {
        // Pulizia post vittoria senza stampare Game Over
        squad_cleanup(team);
        team = NULL;
        return; 
    }

    utils_clear_screen();
    if (squad_is_empty(team)) printf("\nNessuno e' sopravvissuto.\nGAME OVER\n");
    else if (days > 30) printf("\nUragano in arrivo...\nGAME OVER\n");
    
    squad_cleanup(team);
    team = NULL;
    printf("Premi INVIO per tornare al menu..."); utils_wait_enter();
}

static void do_tricks_menu(void) {
    while(1) {
        utils_clear_screen();
        printf("1. Imposta HP iniziali (%d)\n2. Imposta N. Naufraghi (%d)\n3. Imposta Risorse\n4. Sblocca tutto (%d)\n5. Indietro\nScelta: ", trick_settings.initial_hp, trick_settings.num_survivors, trick_settings.unlock_final);
        
        int c = utils_get_int_input();
        if (c == 1) { 
            printf("Nuovi HP (1-10): "); int v = utils_get_int_input(); 
            if (v >= 1 && v <= 10) trick_settings.initial_hp = v; 
        }
        else if (c == 2) { 
            printf("Numero naufraghi (>0): "); int v = utils_get_int_input(); 
            if (v > 0) trick_settings.num_survivors = v; 
        }
        else if (c == 3) {
            printf("Cibo: "); int v = utils_get_int_input(); if(v>=0) trick_settings.food = v;
            printf("Acqua: "); v = utils_get_int_input(); if(v>=0) trick_settings.water = v;
            printf("Legna: "); v = utils_get_int_input(); if(v>=0) trick_settings.wood = v;
            printf("Corda: "); v = utils_get_int_input(); if(v>=0) trick_settings.rope = v;
        }
        else if (c == 4) { trick_settings.unlock_final = !trick_settings.unlock_final; trick_settings.reveal_map = trick_settings.unlock_final;}
        else if (c == 5) { break; }
    }
}

void game_main_menu(void) {
    while (1) {
        utils_clear_screen();
        printf("1. Nuova partita\n2. Crediti\n");
        if (tricks_unlocked) printf("3. Trucchi\n4. Esci\nScelta: ");
        else printf("3. Esci\nScelta: ");

        char line[64];
        if (!fgets(line, sizeof(line), stdin)) exit(0);

        if (strstr(line, "...---...")) {
            tricks_unlocked = true;
            printf("TRUCCHI SBLOCCATI!\n"); printf("Premi INVIO..."); utils_wait_enter();
            continue;
        }

        if (strcmp(line, "1\n") == 0) { setup_new_game(); game_camp_menu(); }
        else if (strcmp(line, "2\n") == 0) { printf("Crediti: Andrea Grandesso.\n"); printf("Premi INVIO..."); utils_wait_enter(); }
        else if (tricks_unlocked && strcmp(line, "3\n") == 0) { do_tricks_menu(); }
        else if ((tricks_unlocked && strcmp(line, "4\n") == 0) || (!tricks_unlocked && strcmp(line, "3\n") == 0)) { break; }
    }
}