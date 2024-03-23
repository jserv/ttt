#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "agents/mcts.h"
#include "agents/negamax.h"
#include "agents/reinforcement_learning.h"
#include "game.h"
#include "zobrist.h"

#define N_GAMES 100
#define ELO_INIT 1500
#define ELO_K 32

static const char *agent_name[] = {"Negamax", "MCTS", "RL"};

static long long int win[] = {0, 0, 0};
static long long int draw[] = {0, 0, 0};
static long long int lose[] = {0, 0, 0};

static double elo_rating[] = {ELO_INIT, ELO_INIT, ELO_INIT};

static int play_game(int player1, int player2)
{
    char table[N_GRIDS];
    int player = player1;

    memset(table, ' ', N_GRIDS);

    printf("Start a game: %s v.s. %s\n", agent_name[player1],
           agent_name[player2]);

    rl_agent_t agent;
    unsigned int state_num = 1;
    CALC_STATE_NUM(state_num);
    init_rl_agent(&agent, state_num,
                  'X' * (player1 == 2) + 'O' * (player2 == 2));
    load_model(&agent, state_num, MODEL_NAME);

    negamax_init();

    while (1) {
        char win = check_win(table);

        if (win != ' ') {
            zobrist_destroy_table();
            free(agent.state_value);
            if (win == 'D')
                return 0;
            return win == 'X' ? 1 : -1;
        }

        int move;

        if (player == 0)
            move = negamax_predict(table, player == player1 ? 'X' : 'O').move;
        else if (player == 1)
            move = mcts(table, player == player1 ? 'X' : 'O');
        else
            move = play_rl(table, &agent);

        table[move] = player == player1 ? 'X' : 'O';
        player ^= player1 ^ player2;
    }

    assert(false);  // Unreachable
    return 0;
}

static void dump_elo()
{
    printf("%-10s | %-10s | %-10s | %-10s | %-10s\n", "Agent Name",
           "Elo Rating", "Win", "Draw", "Lose");
    printf("---------------------------------------------------------\n");

    for (int i = 0; i < 3; ++i)
        printf("%-10s | %-10.2f | %-10lld | %-10lld | %-10lld\n", agent_name[i],
               elo_rating[i], win[i], draw[i], lose[i]);
}

int main()
{
    srand(time(NULL));

    for (int i = 0; i < N_GAMES; i++) {
        printf("Running Game #%d\n", i + 1);
        double sa, sb, ra, rb, ea, eb;
        int player1 = rand() % 3;
        int player2 = rand() % 3;

        while (player1 == player2) {
            player1 = rand() % 3;
            player2 = rand() % 3;
        }

        ra = elo_rating[player1];
        rb = elo_rating[player2];

        int result = play_game(player1, player2);

        if (result == 1) {
            win[player1]++;
            lose[player2]++;
            sa = 1;
            sb = 0;
        } else if (result == 0) {
            draw[player1]++;
            draw[player2]++;
            sa = 0.5;
            sb = 0.5;
        } else {
            win[player2]++;
            lose[player1]++;
            sa = 0;
            sb = 1;
        }

        ea = 1 / (1 + pow(10, (rb - ra) / 400));
        eb = 1 / (1 + pow(10, (ra - rb) / 400));

        elo_rating[player1] = ra + ELO_K * (sa - ea);
        elo_rating[player2] = rb + ELO_K * (sb - eb);
    }

    dump_elo();

    return 0;
}
