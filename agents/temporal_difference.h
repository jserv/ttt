#define LEARNING_RATE 0.02
#define GAMMA 0.99
#define MODEL_NAME "state_value.bin"

#define CALC_STATE_NUM(x)                 \
    {                                     \
        x = 1;                            \
        for (int i = 0; i < N_GRIDS; i++) \
            x *= 3;                       \
    }

#define for_each_empty_grid(i, table) \
    for (int i = 0; i < N_GRIDS; i++) \
        if (table[i] == ' ')

typedef struct td_agent {
    char player;
    float *state_value;
} td_agent_t;

void init_td_agent(td_agent_t *agent, unsigned int state_num, char player);
void load_model(td_agent_t *agent,
                unsigned int state_num,
                const char *model_path);
int play_td(char *table, td_agent_t *agent);
int get_action_exploit(char *table, td_agent_t *agent);
void update_state_value(char *cur_state, char *nxt_state, td_agent_t *agent);
