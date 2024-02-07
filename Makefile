PROG = ttt
CFLAGS = -Wall -Wextra -std=c11 -I .

GIT_HOOKS := .git/hooks/applied

all: $(GIT_HOOKS) $(PROG)

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

$(PROG): $(PROG).c game.c agents/negamax.c
	gcc $(CFLAGS) -o $@ $^

clean:
	-$(RM) $(PROG)
