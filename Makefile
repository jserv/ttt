PROG = ttt
CFLAGS := -Wall -Wextra -std=c11
CFLAGS += -I.

GIT_HOOKS := .git/hooks/applied

all: $(GIT_HOOKS) $(PROG)

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

OBJS := \
	game.o \
	agents/negamax.o \
	main.o

$(PROG): $(OBJS)

clean:
	-$(RM) $(PROG) $(OBJS)
