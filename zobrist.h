#pragma once

#include "game.h"
#include "list.h"

#define HASH_TABLE_SIZE ((int) 1e6 + 3)  // choose a large prime number

extern unsigned long long zobrist_table[N_GRIDS][2];

typedef struct {
    unsigned long long key;
    int score;
    int move;
    struct hlist_node ht_list;
} zobrist_entry_t;

void zobrist_init(void);
zobrist_entry_t *zobrist_get(unsigned long long key);
void zobrist_put(unsigned long long key, int score, int move);
void zobrist_clear(void);
