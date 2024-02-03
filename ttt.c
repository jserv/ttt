#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SEARCH_DEPTH 6
#define BOARD_SIZE 4
#define GOAL 3
#define N_GRIDS (BOARD_SIZE) * (BOARD_SIZE)
#define GET_INDEX(i, j) (i) * (BOARD_SIZE) + (j)
#define GET_COL(x) ((x) % (BOARD_SIZE))
#define GET_ROW(x) ((x) / (BOARD_SIZE))

_Static_assert(BOARD_SIZE < 26, "Board size must be less than 26");

typedef struct {
    int i_shift, j_shift;
    int i_lower_bound, j_lower_bound, i_upper_bound, j_upper_bound;
} line_t;
static line_t lines[4] = {
    {1, 0, 0, 0, BOARD_SIZE - GOAL + 1, BOARD_SIZE},             // ROW
    {0, 1, 0, 0, BOARD_SIZE, BOARD_SIZE - GOAL + 1},             // COL
    {1, 1, 0, 0, BOARD_SIZE - GOAL + 1, BOARD_SIZE - GOAL + 1},  // PRIMARY
    {1, -1, 0, GOAL - 1, BOARD_SIZE - GOAL + 1, BOARD_SIZE},     // SECONDARY
};

static int *move_record = NULL;
static int move_count = 0;

static int history_score_sum[N_GRIDS];
static int history_count[N_GRIDS];

void record_move(int move)
{
    static int n_move_records = 0;
    if (move_count == 0) {
        // minimum of 5 moves is required to determine the winner
        n_move_records = 5;
        move_record = malloc(sizeof(int) * (5));
    } else if (move_count == n_move_records) {
        // TODO: find a better size to resize move_record
        n_move_records = move_count + BOARD_SIZE;
        move_record = realloc(move_record, sizeof(int) * n_move_records);
    }

    if (!move_record)
        exit(1);
    move_record[move_count++] = move;
}

void print_moves()
{
    printf("Moves: ");
    for (int i = 0; i < move_count; i++) {
        printf("%c%d", 'A' + GET_COL(move_record[i]),
               1 + GET_ROW(move_record[i]));
        if (i < move_count - 1) {
            printf(" -> ");
        }
    }
    printf("\n");
}

void draw_board(const char *t)
{
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (BOARD_SIZE < 10)
            printf("%2d | ", i + 1);
        else if (BOARD_SIZE >= 10 && BOARD_SIZE < 100)
            printf("%3d | ", i + 1);
        else
            printf("%4d | ", i + 1);

        for (int j = 0; j < BOARD_SIZE; j++) {
            // make background color alter between high-intensity and standard
            if ((i + j) & 1U)
                printf("\x1b[47m");
            else
                printf("\x1b[107m");

            switch (t[GET_INDEX(i, j)]) {
            case 'O':
                printf("\x1b[31m");
                printf(" ○ ");
                printf("\x1b[39m");
                break;
            case 'X':
                printf("\x1b[34m");
                printf(" × ");
                printf("\x1b[39m");
                break;
            default:
                printf("   ");
                break;
            }
            printf("\x1b[49m");
        }
        printf("\n");
    }
    if (BOARD_SIZE >= 10)
        printf("-");
    if (BOARD_SIZE >= 100)
        printf("-");
    printf("---+-");
    for (int i = 0; i < BOARD_SIZE; i++)
        printf("---");
    printf("\n");
    if (BOARD_SIZE >= 10)
        printf(" ");
    if (BOARD_SIZE >= 100)
        printf(" ");
    printf("    ");
    for (int i = 0; i < BOARD_SIZE; i++)
        printf(" %2c", 'A' + i);
    printf("\n");
}

char check_line_segment_win(const char *t, int i, int j, line_t line)
{
    char last = t[GET_INDEX(i, j)];
    if (last == ' ')
        return ' ';
    for (int k = 1; k < GOAL; k++) {
        if (last != t[GET_INDEX(i + k * line.i_shift, j + k * line.j_shift)]) {
            return ' ';
        }
    }
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

int cmp_moves(const void *a, const void *b)
{
    int *_a = (int *) a, *_b = (int *) b;
    int score_a = 0, score_b = 0;

    if (history_count[*_a])
        score_a = history_score_sum[*_a] / history_count[*_a];
    if (history_count[*_b])
        score_b = history_score_sum[*_b] / history_count[*_b];
    return score_b - score_a;
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
    qsort(moves, m, sizeof(int), cmp_moves);
    return moves;
}

int eval_line_segment_score(const char *table,
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
            if (score == -1) {
                score = 0;
                break;
            }
            if (score)
                score *= 10;
            else
                score = 1;
        } else if (curr != ' ') {
            if (score == 1) {
                score = 0;
                break;
            }
            if (score)
                score *= 10;
            else
                score = -1;
        }
    }
    return score;
}

int get_score(const char *table, char player)
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

int negamax(char *table, int depth, char player, int alpha, int beta)
{
    if (check_win(table) != ' ')
        return get_score(table, player);
    if (depth == 0)
        return get_score(table, player);

    int score;
    int best_score = -10000;
    int best_move = -1;
    const int *moves = available_moves(table);
    for (int i = 0; i < N_GRIDS; i++) {
        if (moves[i] == -1)
            break;
        table[moves[i]] = player;
        if (!i)  // do a full search on the first move
            score = -negamax(table, depth - 1, player == 'X' ? 'O' : 'X', -beta,
                             -alpha);
        else {
            // do a null-window search on the rest of the moves
            score = -negamax(table, depth - 1, player == 'X' ? 'O' : 'X',
                             -alpha - 1, -alpha);
            if (alpha < score && score < beta)  // do a full re-search
                score = -negamax(table, depth - 1, player == 'X' ? 'O' : 'X',
                                 -beta, -score);
        }
        history_count[moves[i]]++;
        history_score_sum[moves[i]] += score;
        if (score > best_score) {
            best_score = score;
            best_move = moves[i];
        }
        table[moves[i]] = ' ';
        if (score > alpha)
            alpha = score;
        if (alpha >= beta)
            break;
    }

    if (depth == MAX_SEARCH_DEPTH && best_move != -1) {
        table[best_move] = player;
        record_move(best_move);
    }
    free((char *) moves);
    return best_score;
}

int get_input(char player)
{
    char *line = NULL;
    size_t line_length = 0;
    int parseX = 1;

    int x = -1, y = -1;
    while (x < 0 || x > (BOARD_SIZE - 1) || y < 0 || y > (BOARD_SIZE - 1)) {
        printf("%c> ", player);
        int r = getline(&line, &line_length, stdin);
        if (r == -1)
            exit(1);
        if (r < 2)
            continue;
        x = 0;
        y = 0;
        parseX = 1;
        for (int i = 0; i < (r - 1); i++) {
            if (isalpha(line[i]) && parseX) {
                x = x * 26 + (tolower(line[i]) - 'a' + 1);
                if (x > BOARD_SIZE) {
                    // could be any value in [BOARD_SIZE + 1, INT_MAX]
                    x = BOARD_SIZE + 1;
                    printf("Invalid operation: index exceeds board size\n");
                    break;
                }
                continue;
            }
            // input does not have leading alphabets
            if (x == 0) {
                printf("Invalid operation: No leading alphabet\n");
                y = 0;
                break;
            }
            parseX = 0;
            if (isdigit(line[i])) {
                y = y * 10 + line[i] - '0';
                if (y > BOARD_SIZE) {
                    // could be any value in [BOARD_SIZE + 1, INT_MAX]
                    y = BOARD_SIZE + 1;
                    printf("Invalid operation: index exceeds board size\n");
                    break;
                }
                continue;
            }
            // any other character is invalid
            // any non-digit char during digit parsing is invalid
            // TODO: Error message could be better by separating these two cases
            printf("Invalid operation\n");
            x = y = 0;
            break;
        }
        x -= 1;
        y -= 1;
    }
    free(line);
    return GET_INDEX(y, x);
}

int main()
{
    char *table = malloc(sizeof(char) * N_GRIDS);
    if (!table)
        return -1;
    memset(table, ' ', sizeof(char) * N_GRIDS);
    char turn = 'X';
    char ai = 'O';

    while (1) {
        char win = check_win(table);
        if (win == 'D') {
            draw_board(table);
            printf("It is a draw!\n");
            break;
        } else if (win != ' ') {
            draw_board(table);
            printf("%c won!\n", win);
            break;
        }

        if (turn == ai) {
            memset(history_score_sum, 0, sizeof(history_score_sum));
            memset(history_count, 0, sizeof(history_count));
            for (int depth = 2; depth <= MAX_SEARCH_DEPTH; depth += 2)
                negamax(table, depth, ai, -100000, 100000);
        } else {
            draw_board(table);
            int move;
            while (1) {
                move = get_input(turn);
                if (table[move] == ' ') {
                    break;
                }
                printf("Invalid operation: the position has been marked\n");
            }
            table[move] = turn;
            record_move(move);
        }
        turn = turn == 'X' ? 'O' : 'X';
    }
    print_moves();
    free(move_record);
    free(table);

    return 0;
}
