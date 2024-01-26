#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MOVES (3 * 3)

int move_record[MAX_MOVES];
int move_count = 0;

void record_move(int move)
{
    if (move_count < MAX_MOVES) {
        move_record[move_count++] = move;
    }
}

void print_moves()
{
    printf("Moves: ");
    for (int i = 0; i < move_count; i++) {
        int col = move_record[i] % 3;
        int row = move_record[i] / 3;
        printf("%c%d", 'A' + col, row + 1);
        if (i < move_count - 1) {
            printf(" -> ");
        }
    }
    printf("\n");
}

void draw_board(const char *t)
{
    const int M = 3, N = 3;
    for (int i = 0; i < M; i++) {
        printf("%2c | ", '1' + i);
        for (int j = 0; j < N; j++) {
            printf("\x1b[47m");
            switch (t[i * M + j]) {
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
    printf("---+-");
    for (int i = 0; i < N; i++)
        printf("---");
    printf("\n");
    printf("    ");
    for (int i = 0; i < N; i++)
        printf(" %2c", 'A' + i);
    printf("\n");
}

char check_win(char *t)
{
    for (int i = 0; i < 9; i += 3) {
        if (t[i] != ' ' && t[i] == t[i + 1] && t[i + 1] == t[i + 2])
            return t[i];
    }

    for (int i = 0; i < 3; i++) {
        if (t[i] != ' ' && t[i] == t[i + 3] && t[i + 3] == t[i + 6])
            return t[i];
    }

    if (t[0] != ' ' && t[0] == t[4] && t[4] == t[8])
        return t[0];
    if (t[2] != ' ' && t[2] == t[4] && t[4] == t[6])
        return t[2];

    for (int i = 0; i < 9; i++)
        if (t[i] == ' ')
            return ' ';
    return 'D';
}

int *available_moves(char *table)
{
    int *moves = malloc(9 * sizeof(int));
    int m = 0;
    for (int i = 0; i < 9; i++)
        if (table[i] == ' ')
            moves[m++] = i;
    if (m < 9)
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

int get_score(const char *table, char player)
{
    int score = 0;

    score += eval_line_score(table, player, 0, 1, 2);
    score += eval_line_score(table, player, 3, 4, 5);
    score += eval_line_score(table, player, 6, 7, 8);

    score += eval_line_score(table, player, 0, 3, 6);
    score += eval_line_score(table, player, 1, 4, 7);
    score += eval_line_score(table, player, 2, 5, 8);

    score += eval_line_score(table, player, 0, 4, 8);
    score += eval_line_score(table, player, 2, 4, 6);

    return score;
}

int negamax(char *table, int depth, char player, int alpha, int beta)
{
    if (check_win(table) != ' ')
        return get_score(table, player);

    int best_score = -10000;
    int best_move = -1;
    const int *moves = available_moves(table);
    for (int i = 0; i < 9; i++) {
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

    char x = -1, y = -1;
    while (x < 0 || x > 2 || y < 0 || y > 2) {
        printf("%c> ", player);
        int r = getline(&line, &line_length, stdin);
        if (r == -1)
            exit(1);
        if (r < 2)
            continue;
        x = tolower(line[0]) - 'a';
        y = tolower(line[1]) - '1';
    }
    free(line);
    return x + 3 * y;
}

int main()
{
    char table[] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
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
    return 0;
}
