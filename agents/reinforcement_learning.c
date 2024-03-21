#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "reinforcement_learning.h"
#include "util.h"

// Uncomment it if you want to see the log output.
// #define VERBOSE

// TODO: Find a more efficient hash, we could not store 5x5 or larger board,
// Since we could have 3^25 states and it might overflow.
int table_to_hash(char *table)
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

char *hash_to_table(int hash)
{
    char *table = malloc(sizeof(char) * N_GRIDS);
    for (int i = N_GRIDS - 1; i >= 0; i--) {
        table[i] = " OX"[hash % 3];
        hash /= 3;
    }
    return table;
}

void load_model(rl_agent_t *agent,
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

int get_action_exploit(char *table, rl_agent_t *agent)
{
    int max_act = -1;
    float max_q = -FLT_MAX;
    float *state_value = agent->state_value;
    int candidate_count = 1;
#ifdef VERBOSE
    printf("[ ");
#endif
    for_each_empty_grid (i, table) {
        table[i] = agent->player;
        float new_q = state_value[table_to_hash(table)];
#ifdef VERBOSE
        printf("%f ", new_q);
#endif
        if (new_q == max_q) {
            ++candidate_count;
            if (rand() % candidate_count == 0) {
                max_act = i;
            }
        } else if (new_q > max_q) {
            candidate_count = 1;
            max_q = new_q;
            max_act = i;
        }
        table[i] = ' ';
    }
#ifdef VERBOSE
    printf(" ]\n");
    printf("exploit %d\n", max_act);
#endif
    return max_act;
}

int play_rl(char *table, rl_agent_t *agent)
{
    int move = get_action_exploit(table, agent);
    table[move] = agent->player;
    return move;
}

void store_state_value(rl_agent_t *agent, unsigned int N_STATES)
{
    FILE *fptr = NULL;
    if ((fptr = fopen(MODEL_NAME, "wb")) == NULL) {
        perror("Failed to open file");
        exit(1);
    }
    if (fwrite(agent[0].state_value, N_STATES * sizeof(float), 1, fptr) != 1) {
        perror("Failed to write file");
        exit(1);
    }
    if (fwrite(agent[1].state_value, N_STATES * sizeof(float), 1, fptr) != 1) {
        perror("Failed to write file");
        exit(1);
    }
    fclose(fptr);
}

void init_rl_agent(rl_agent_t *agent, unsigned int state_num, char player)
{
    agent->player = player;
    agent->state_value = malloc(sizeof(float) * state_num);
    if (!(agent->state_value)) {
        perror("Failed to allocate memory");
        exit(1);
    }
}
