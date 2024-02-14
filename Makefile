PROG = ttt
CFLAGS := -Wall -Wextra -std=c11
CFLAGS += -I. -MMD
LDFLAGS :=

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

$(PROG): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	-$(RM) $(PROG) $(OBJS) $(deps)
