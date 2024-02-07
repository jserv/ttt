#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "negamax.h"

#define MAX_SEARCH_DEPTH 6

static int history_score_sum[N_GRIDS];
static int history_count[N_GRIDS];

static int cmp_moves(const void *a, const void *b)
{
    int *_a = (int *) a, *_b = (int *) b;
    int score_a = 0, score_b = 0;

    if (history_count[*_a])
        score_a = history_score_sum[*_a] / history_count[*_a];
    if (history_count[*_b])
        score_b = history_score_sum[*_b] / history_count[*_b];
    return score_b - score_a;
}

static int eval_line_segment_score(const char *table,
                                   char player,
                                   int i,
                                   int j,
                                   line_t line)
{
    int score = 0;
    for (int k = 0; k < GOAL; k++) {
        char curr =
            table[GET_INDEX(i + k * line.i_shift, j + k * line.j_shift)];
        if (curr == player) {
            if (score < 0)
                return 0;
            if (score)
                score *= 10;
            else
                score = 1;
        } else if (curr != ' ') {
            if (score > 0)
                return 0;
            if (score)
                score *= 10;
            else
                score = -1;
        }
    }
    return score;
}

static int get_score(const char *table, char player)
{
    int score = 0;
    for (int i_line = 0; i_line < 4; ++i_line) {
        line_t line = lines[i_line];
        for (int i = line.i_lower_bound; i < line.i_upper_bound; ++i) {
            for (int j = line.j_lower_bound; j < line.j_upper_bound; ++j) {
                score += eval_line_segment_score(table, player, i, j, line);
            }
        }
    }
    return score;
}

static move_t negamax(char *table, int depth, char player, int alpha, int beta)
{
    if (check_win(table) != ' ' || depth == 0) {
        move_t result = {get_score(table, player), -1};
        return result;
    }

    int score;
    move_t best_move = {-10000, -1};
    int *moves = available_moves(table);
    int n_moves = 0;
    while (n_moves < N_GRIDS && moves[n_moves] != -1)
        ++n_moves;
    qsort(moves, n_moves, sizeof(int), cmp_moves);
    for (int i = 0; i < n_moves; i++) {
        table[moves[i]] = player;
        if (!i)  // do a full search on the first move
            score = -negamax(table, depth - 1, player == 'X' ? 'O' : 'X', -beta,
                             -alpha)
                         .score;
        else {
            // do a null-window search on the rest of the moves
            score = -negamax(table, depth - 1, player == 'X' ? 'O' : 'X',
                             -alpha - 1, -alpha)
                         .score;
            if (alpha < score && score < beta)  // do a full re-search
                score = -negamax(table, depth - 1, player == 'X' ? 'O' : 'X',
                                 -beta, -score)
                             .score;
        }
        history_count[moves[i]]++;
        history_score_sum[moves[i]] += score;
        if (score > best_move.score) {
            best_move.score = score;
            best_move.move = moves[i];
        }
        table[moves[i]] = ' ';
        if (score > alpha)
            alpha = score;
        if (alpha >= beta)
            break;
    }

    free((char *) moves);
    return best_move;
}

move_t negamax_predict(char *table, char player)
{
    memset(history_score_sum, 0, sizeof(history_score_sum));
    memset(history_count, 0, sizeof(history_count));
    move_t result;
    for (int depth = 2; depth <= MAX_SEARCH_DEPTH; depth += 2)
        result = negamax(table, depth, player, -100000, 100000);
    return result;
}