#pragma once

#define MODEL_NAME "state_value.bin"

#define CALC_STATE_NUM(x)                 \
    {                                     \
        x = 1;                            \
        for (int i = 0; i < N_GRIDS; i++) \
            x *= 3;                       \
    }

typedef struct td_agent {
    char player;
    float *state_value;
} rl_agent_t;

int table_to_hash(char *table);
char *hash_to_table(int hash);
void load_model(rl_agent_t *agent,
                unsigned int state_num,
                const char *model_path);
int get_action_exploit(char *table, rl_agent_t *agent);
int play_rl(char *table, rl_agent_t *agent);
void store_state_value(rl_agent_t *agent, unsigned int N_STATES);
void init_rl_agent(rl_agent_t *agent, unsigned int state_num, char player);
