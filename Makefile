PROG = ttt
CFLAGS := -Wall -Wextra -std=c11
CFLAGS += -I. -MMD
LDFLAGS :=
TRAIN = train
TD = td
MCTS = mcts
TD_CFLAGS := $(CFLAGS) -D USE_TD
MCTS_CFLAGS := $(CFLAGS) -D USE_MCTS
MCTS_LDFLAGS := $(LDFLAGS) -lm

GIT_HOOKS := .git/hooks/applied

all: $(GIT_HOOKS) $(PROG)

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

OBJS := \
	game.o \
	mt19937-64.o \
	zobrist.o \
	agents/negamax.o \
	main.o
deps := $(OBJS:%.o=%.d)
deps += $(TRAIN).d $(TD).d
deps += $(MCTS).d

$(PROG): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(TD): main.c agents/temporal_difference.c game.c
	$(CC) -o $@ $^ $(TD_CFLAGS)

$(TRAIN): $(TRAIN).c agents/temporal_difference.c game.c
	$(CC) $(CFLAGS) -o $@ $^

$(MCTS): main.c agents/mcts.c game.c
	$(CC) -o $@ $^ $(MCTS_CFLAGS) $(MCTS_LDFLAGS)

clean:
	-$(RM) $(PROG) $(OBJS) $(deps) $(TRAIN) $(TD) $(MCTS)
	-$(RM) *.bin
