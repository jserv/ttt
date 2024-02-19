#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "temporal_difference.h"
#include "util.h"

// TODO: Find a more efficient hash, we could not store 5x5 or larger board,
// Since we could have 3^25 states and it might overflow.
static int table_to_hash(char *table)
{
    int ret = 0;
    for (int i = 0; i < N_GRIDS; i++) {
        ret *= 3;
        if (table[i] == ' ') {
            ret += 0;
        } else if (table[i] == 'O') {
            ret += 1;
        } else if (table[i] == 'X') {
            ret += 2;
        }
    }
    return ret;
}

static char *hash_to_table(int hash)
{
    char *table = malloc(sizeof(char) * N_GRIDS);
    for (int i = N_GRIDS - 1; i >= 0; i--) {
        table[i] = " OX"[hash % 3];
        hash /= 3;
    }
    return table;
}

void init_td_agent(td_agent_t *agent, unsigned int state_num, char player)
{
    agent->player = player;
    agent->state_value = malloc(sizeof(float) * state_num);
    if (!(agent->state_value)) {
        perror("Failed to allocate memory");
        exit(1);
    }
    for (unsigned int i = 0; i < state_num; i++)
        agent->state_value[i] = get_score(hash_to_table(i), player);
}

void load_model(td_agent_t *agent,
                unsigned int state_num,
                const char *model_path)
{
    FILE *fptr = fopen(model_path, "rb");
    if (!fptr) {
        perror("Failed to open state value table, train first");
        exit(1);
    }
    long offset = (agent->player == 'O') ? 0 : state_num * sizeof(float);
    if (fseek(fptr, offset, SEEK_SET) != 0) {
        perror("Failed to seek file pointer to given player");
        exit(1);
    }
    if (fread(agent->state_value, state_num * sizeof(float), 1, fptr) != 1) {
        perror("Failed to load the model");
        exit(1);
    }
    fclose(fptr);
}

int get_action_exploit(char *table, td_agent_t *agent)
{
    int max_act = -1;
    float max_q = -FLT_MAX;
    float *state_value = agent->state_value;
    char *try_table = malloc(sizeof(char) * N_GRIDS);
    if (!try_table) {
        perror("Failed to allocate memory");
        exit(1);
    }
    printf("[ ");
    for_each_empty_grid(i, table)
    {
        memcpy(try_table, table, sizeof(char) * N_GRIDS);
        try_table[i] = agent->player;
        printf("%f ", state_value[table_to_hash(try_table)]);
        if (state_value[table_to_hash(try_table)] > max_q) {
            max_q = state_value[table_to_hash(try_table)];
            max_act = i;
        }
    }
    printf(" ]\n");
    printf("exploit %d\n", max_act);
    free(try_table);
    return max_act;
}

int play_td(char *table, td_agent_t *agent)
{
    int move = get_action_exploit(table, agent);
    table[move] = agent->player;
    return move;
}

void update_state_value(char *cur_state, char *nxt_state, td_agent_t *agent)
{
    int cur_state_hash = table_to_hash(cur_state);
    int nxt_state_hash = table_to_hash(nxt_state);
    int nxt_reward = get_score(hash_to_table(nxt_state_hash), agent->player);
    agent->state_value[cur_state_hash] =
        (1 - LEARNING_RATE) * agent->state_value[cur_state_hash] +
        LEARNING_RATE *
            (nxt_reward + GAMMA * agent->state_value[nxt_state_hash]);
}
