#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SEARCH_DEPTH 6
#define BOARD_SIZE 4
#define N_GRIDS (BOARD_SIZE) * (BOARD_SIZE)
#define GET_INDEX(i, j) (i) * (BOARD_SIZE) + (j)
#define GET_COL(x) ((x) % (BOARD_SIZE))
#define GET_ROW(x) ((x) / (BOARD_SIZE))

enum { ROW, COL, PRIMARY, SECONDARY };
static int no_step[2] = {0, 0};
static int step_forward[2] = {1, 2};
static int step_backward[2] = {-1, -2};

static int *move_record = NULL;
static int move_count = 0;

void record_move(int move)
{
    if (move_count == 0) {
        // minimum of 5 moves is required to determine the winner
        move_record = malloc(sizeof(int) * (5));
    } else {
        // Todo : find a better size to resize move_record
        move_record =
            realloc(move_record, sizeof(int) * (move_count + BOARD_SIZE));
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
    for (int i = 0; i < BOARD_SIZE; i++) {
        // could be more generalized
        if (i < 26)
            printf(" %2c", 'A' + i);
        else {
            int k = i / 26 - 1;
            printf(" %c", 'A' + k);
            printf("%c", 'A' + i % 26);
        }
    }
    printf("\n");
}

char check_row_col(const char *t, int flag)
{
    int i_shift[2], j_shift[2];
    if (flag == ROW) {
        memcpy(i_shift, no_step, sizeof(i_shift));
        memcpy(j_shift, step_forward, sizeof(j_shift));
    } else if (flag == COL) {
        memcpy(i_shift, step_forward, sizeof(i_shift));
        memcpy(j_shift, no_step, sizeof(j_shift));
    }
    for (int i = 0; i < BOARD_SIZE - i_shift[1]; i++) {
        for (int j = 0; j < BOARD_SIZE - j_shift[1]; j++) {
            if (t[GET_INDEX(i, j)] != ' ' &&
                t[GET_INDEX(i, j)] ==
                    t[GET_INDEX(i + i_shift[0], j + j_shift[0])] &&
                t[GET_INDEX(i + i_shift[0], j + j_shift[0])] ==
                    t[GET_INDEX(i + i_shift[1], j + j_shift[1])]) {
                return t[GET_INDEX(i, j)];
            }
        }
    }
    return ' ';
}

char check_diagonal(const char *t, int flag)
{
    int i_shift[2], j_shift[2];
    if (flag == PRIMARY) {
        memcpy(i_shift, step_forward, sizeof(i_shift));
        memcpy(j_shift, step_forward, sizeof(j_shift));
    } else if (flag == SECONDARY) {
        memcpy(i_shift, step_backward, sizeof(i_shift));
        memcpy(j_shift, step_forward, sizeof(j_shift));
    }
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (i + i_shift[1] < 0 || i + i_shift[1] >= BOARD_SIZE ||
                j + j_shift[1] < 0 || j + j_shift[1] >= BOARD_SIZE)
                continue;
            if (t[GET_INDEX(i, j)] != ' ' &&
                t[GET_INDEX(i, j)] ==
                    t[GET_INDEX(i + i_shift[0], j + j_shift[0])] &&
                t[GET_INDEX(i + i_shift[0], j + j_shift[0])] ==
                    t[GET_INDEX(i + i_shift[1], j + j_shift[1])]) {
                return t[GET_INDEX(i, j)];
            }
        }
    }
    return ' ';
}

char check_win(char *t)
{
    char r = check_row_col(t, ROW);
    if (r != ' ')
        return r;
    char c = check_row_col(t, COL);
    if (c != ' ')
        return c;
    char md = check_diagonal(t, PRIMARY);
    if (md != ' ')
        return md;
    char bd = check_diagonal(t, SECONDARY);
    if (bd != ' ')
        return bd;
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

int eval_line_score(const char *table, char player, int a, int b, int c)
{
    int score = 0;

    if (table[a] == player)
        score = 1;
    else if (table[a] != ' ')
        score = -1;

    if (table[b] == player) {
        if (score == -1)
            return 0;
        if (score == 1)
            score = 10;
        else
            score = 1;
    } else if (table[b] != ' ') {
        if (score == 1)
            return 0;
        if (score == -1)
            score = -10;
        else
            score = -1;
    }

    if (table[c] == player) {
        if (score < 0)
            return 0;
        if (score > 0)
            score *= 10;
        else
            score = 1;
    } else if (table[c] != ' ') {
        if (score > 0)
            return 0;
        if (score < 0)
            score *= 10;
        else
            score = -1;
    }

    return score;
}

int get_row_col_score(const char *table, char player, int flag)
{
    int i_shift[2], j_shift[2];
    if (flag == ROW) {
        memcpy(i_shift, no_step, sizeof(i_shift));
        memcpy(j_shift, step_forward, sizeof(j_shift));
    } else if (flag == COL) {
        memcpy(i_shift, step_forward, sizeof(i_shift));
        memcpy(j_shift, no_step, sizeof(j_shift));
    }
    int score = 0;
    for (int i = 0; i < BOARD_SIZE - i_shift[1]; i++) {
        for (int j = 0; j < BOARD_SIZE - j_shift[1]; j++) {
            score += eval_line_score(table, player, GET_INDEX(i, j),
                                     GET_INDEX(i + i_shift[0], j + j_shift[0]),
                                     GET_INDEX(i + i_shift[1], j + j_shift[1]));
        }
    }
    return score;
}

int get_diagonal_score(const char *table, char player, int flag)
{
    int i_shift[2], j_shift[2];
    int score = 0;
    if (flag == PRIMARY) {
        memcpy(i_shift, step_forward, sizeof(i_shift));
        memcpy(j_shift, step_forward, sizeof(j_shift));
    } else if (flag == SECONDARY) {
        memcpy(i_shift, step_backward, sizeof(i_shift));
        memcpy(j_shift, step_forward, sizeof(j_shift));
    }
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (i + i_shift[1] < 0 || i + i_shift[1] >= BOARD_SIZE ||
                j + j_shift[1] < 0 || j + j_shift[1] >= BOARD_SIZE)
                continue;
            score += eval_line_score(table, player, GET_INDEX(i, j),
                                     GET_INDEX(i + i_shift[0], j + j_shift[0]),
                                     GET_INDEX(i + i_shift[1], j + j_shift[1]));
        }
    }
    return score;
}

int get_score(const char *table, char player)
{
    int score = 0;
    score += get_row_col_score(table, player, ROW);
    score += get_row_col_score(table, player, COL);

    score += get_diagonal_score(table, player, PRIMARY);
    score += get_diagonal_score(table, player, SECONDARY);
    return score;
}

int negamax(char *table, int depth, char player, int alpha, int beta)
{
    if (check_win(table) != ' ')
        return get_score(table, player);
    if (depth == MAX_SEARCH_DEPTH)
        return get_score(table, player);

    int best_score = -10000;
    int best_move = -1;
    const int *moves = available_moves(table);
    for (int i = 0; i < N_GRIDS; i++) {
        if (moves[i] == -1)
            break;
        table[moves[i]] = player;
        int score = -negamax(table, depth + 1, player == 'X' ? 'O' : 'X', -beta,
                             -alpha);
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

    if (depth == 0 && best_move != -1) {
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
        for (int i = 0; i < r - 1; i++) {
            if (isalpha(line[i]) && parseX) {
                x = x * 26 + (tolower(line[i]) - 'a' + 1);
                if (x > BOARD_SIZE) {
                    x = BOARD_SIZE + 1;  // x could be assigned with any value
                                         // in [BOARD_SIZE + 1, INT_MAX]
                    printf("Invalid operation: index exceeds board size\n");
                    break;
                }
                continue;
            }
            // input does not have leading alpabets
            if (x == 0) {
                printf("Invalid operation: No leading alphabet\n");
                y = 0;
                break;
            }
            parseX = 0;
            if (isdigit(line[i])) {
                y = y * 10 + line[i] - '0';
                if (y > BOARD_SIZE) {
                    y = BOARD_SIZE + 1;  // y could be assigned with any value
                                         // in [BOARD_SIZE + 1, INT_MAX]
                    printf("Invalid operation: index exceeds board size\n");
                    break;
                }
                continue;
            }
            // any other char is invalid
            // ant non-digit char during digit parsing is invalid
            // TODO: Error message could be better by separating these two cases
            if (i < r - 1) {
                printf("invalid operation\n");
                x = 0;
                y = 0;
                break;
            }
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
            negamax(table, 0, ai, -100000, 100000);
        } else {
            draw_board(table);
            int move;
            do {
                move = get_input(turn);
            } while (table[move] != ' ');
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
