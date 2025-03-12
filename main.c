#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game.h"
#ifdef USE_RL
#include "agents/reinforcement_learning.h"
#elif defined(USE_MCTS)
#include "agents/mcts.h"
#else
#include "agents/negamax.h"
#endif

static int move_record[N_GRIDS];
static int move_count = 0;

static void record_move(int move)
{
    move_record[move_count++] = move;
}

static void print_moves()
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

static int get_input(char player)
{
    char *line = NULL;
    size_t line_length = 0;
    int parseX = 1;

    int x = -1, y = -1;
    while (x < 0 || x > (BOARD_SIZE - 1) || y < 0 || y > (BOARD_SIZE - 1)) {
        printf("%c> ", player);
        int r = getline(&line, &line_length, stdin);
        if (r == -1) {
            free(line);
            exit(1);
        }
        if (r < 2)
            continue;
        x = 0;
        y = 0;
        parseX = 1;
        for (int i = 0; i < (r - 1); i++) {
            if (isalpha(line[i]) && parseX) {
                // input has multiple column alphabets
                if (i > 0 && isalpha(line[i])) {
                    printf(
                        "Invalid operation: multiple column alphabets "
                        "detected\n");
                    x = y = 0;
                    break;
                }
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
        // input does not have row numbers
        if (x > 0 && x <= BOARD_SIZE && parseX == 1)
            printf("Invalid operation: No row number\n");
        x -= 1;
        y -= 1;
    }
    free(line);
    return GET_INDEX(y, x);
}

int main()
{
    srand(time(NULL));
    char table[N_GRIDS];
    memset(table, ' ', N_GRIDS);
    char turn = 'X';
    char ai = 'O';

#ifdef USE_RL
    rl_agent_t agent;
    unsigned int state_num = 1;
    CALC_STATE_NUM(state_num);
    init_rl_agent(&agent, state_num, 'O');
    load_model(&agent, state_num, MODEL_NAME);
#elif defined(USE_MCTS)
    // A routine for initializing MCTS is not required.
#else
    negamax_init();
#endif
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
#ifdef USE_RL
            int move = play_rl(table, &agent);
            record_move(move);
#elif defined(USE_MCTS)
            int move = mcts(table, ai);
            if (move != -1) {
                table[move] = ai;
                record_move(move);
            }
#else
            int move = negamax_predict(table, ai).move;
            if (move != -1) {
                table[move] = ai;
                record_move(move);
            }
#endif
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

    return 0;
}
