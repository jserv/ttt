PROG = ttt
CFLAGS := -Wall -Wextra -std=c11
CFLAGS += -I. -MMD
LDFLAGS :=
TRAIN = train
TD = td
TD_CFLAGS := $(CFLAGS) -D USE_TD

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

$(PROG): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(TD): main.c agents/temporal_difference.c game.c
	$(CC) -o $@ $^ $(TD_CFLAGS)

$(TRAIN): $(TRAIN).c agents/temporal_difference.c game.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	-$(RM) $(PROG) $(OBJS) $(deps) $(TRAIN) $(TD)
	-$(RM) *.bin
