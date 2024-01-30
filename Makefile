PROG = ttt
CFLAGS = -Wall -Wextra -std=c11

GIT_HOOKS := .git/hooks/applied

all: $(GIT_HOOKS) $(PROG)

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

$(PROG): $(PROG).c
	gcc $(CFLAGS) -o $@ $^

clean:
	-$(RM) $(PROG)
