#pragma once

typedef struct {
    int score, move;
} move_t;

move_t negamax_predict(char *table, char player);