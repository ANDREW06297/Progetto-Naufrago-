/**
 * @file squad.c
 * @brief Implementazione delle funzioni per la lista dinamica dei naufraghi.
 */

#include "squad.h"
#include <stdio.h>
#include <stdlib.h>

Squad* squad_create(void) {
    Squad* squad = (Squad*)malloc(sizeof(Squad));
    if (squad) {
        squad->head = NULL;
        squad->next_id_counter = 1;
    }
    return squad;
}

void squad_add_member(Squad* squad, int initial_hp) {
    if (!squad) return;

    Survivor* new_member = (Survivor*)malloc(sizeof(Survivor));
    if (!new_member) return;

    new_member->id = squad->next_id_counter++;
    new_member->hp = (initial_hp > MAX_HP) ? MAX_HP : initial_hp;
    new_member->next = NULL;

    // Inserimento in coda per garantire l'ordine di arrivo
    if (!squad->head) {
        squad->head = new_member;
    } else {
        Survivor* current = squad->head;
        while (current->next) {
            current = current->next;
        }
        current->next = new_member;
    }
}

void squad_remove_member(Squad* squad, int id) {
    if (!squad || !squad->head) return;

    if (squad->head->id == id) {
        Survivor* temp = squad->head;
        squad->head = squad->head->next;
        free(temp);
        return;
    }

    Survivor* current = squad->head;
    while (current->next && current->next->id != id) {
        current = current->next;
    }

    if (current->next) {
        Survivor* temp = current->next;
        current->next = current->next->next;
        free(temp);
    }
}

void squad_cleanup(Squad* squad) {
    if (!squad) return;
    Survivor* current = squad->head;
    while (current) {
        Survivor* temp = current;
        current = current->next;
        free(temp);
    }
    free(squad);
}

void squad_print_status(const Squad* squad) {
    if (!squad || !squad->head) {
        printf("Nessun naufrago in squadra.\n");
        return;
    }

    Survivor* current = squad->head;
    while (current) {
        printf("Naufrago %d (HP: %d/%d)\n", current->id, current->hp, MAX_HP);
        current = current->next;
    }
}

int squad_count(const Squad* squad) {
    int count = 0;
    if (squad) {
        Survivor* current = squad->head;
        while (current) {
            count++;
            current = current->next;
        }
    }
    return count;
}

bool squad_is_empty(const Squad* squad) {
    return !squad || !squad->head;
}

Survivor* squad_find_member(Squad* squad, int id) {
    if (!squad) return NULL;
    Survivor* current = squad->head;
    while (current) {
        if (current->id == id) return current;
        current = current->next;
    }
    return NULL;
}