#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"

#define LOOKUP(table, i, j, else_value)                         \
    ((i) < 0 || (j) < 0 || (i) > BOARD_SIZE || (j) > BOARD_SIZE \
         ? (else_value)                                         \
         : (table)[GET_INDEX(i, j)])

_Static_assert(BOARD_SIZE <= 26, "Board size must not be greater than 26");
_Static_assert(BOARD_SIZE > 0, "Board size must be greater than 0");
_Static_assert(GOAL <= BOARD_SIZE, "Goal must not be greater than board size");
_Static_assert(GOAL > 0, "Goal must be greater than 0");
_Static_assert(ALLOW_EXCEED == 0 || ALLOW_EXCEED == 1,
               "ALLOW_EXCEED must be a boolean that is 0 or 1");

const line_t lines[4] = {
    {1, 0, 0, 0, BOARD_SIZE - GOAL + 1, BOARD_SIZE},             // ROW
    {0, 1, 0, 0, BOARD_SIZE, BOARD_SIZE - GOAL + 1},             // COL
    {1, 1, 0, 0, BOARD_SIZE - GOAL + 1, BOARD_SIZE - GOAL + 1},  // PRIMARY
    {1, -1, 0, GOAL - 1, BOARD_SIZE - GOAL + 1, BOARD_SIZE},     // SECONDARY
};

static char check_line_segment_win(const char *t, int i, int j, line_t line)
{
    char last = t[GET_INDEX(i, j)];
    if (last == ' ')
        return ' ';
    for (int k = 1; k < GOAL; k++) {
        if (last != t[GET_INDEX(i + k * line.i_shift, j + k * line.j_shift)]) {
            return ' ';
        }
    }
#if !ALLOW_EXCEED
    if (last == LOOKUP(t, i - line.i_shift, j - line.j_shift, ' ') ||
        last ==
            LOOKUP(t, i + GOAL * line.i_shift, j + GOAL * line.j_shift, ' '))
        return ' ';
#endif
    return last;
}

char check_win(char *t)
{
    for (int i_line = 0; i_line < 4; ++i_line) {
        line_t line = lines[i_line];
        for (int i = line.i_lower_bound; i < line.i_upper_bound; ++i) {
            for (int j = line.j_lower_bound; j < line.j_upper_bound; ++j) {
                char win = check_line_segment_win(t, i, j, line);
                if (win != ' ')
                    return win;
            }
        }
    }
    for (int i = 0; i < N_GRIDS; i++)
        if (t[i] == ' ')
            return ' ';
    return 'D';
}

int *available_moves(char *table)
{
    int *moves = malloc(N_GRIDS * sizeof(int));
    int m = 0;
    for (int i = 0; i < N_GRIDS; i++)
        if (table[i] == ' ')
            moves[m++] = i;
    if (m < N_GRIDS)
        moves[m] = -1;
    return moves;
}