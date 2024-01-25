PROG = ttt
CFLAGS = -Wall -Wextra

$(PROG): $(PROG).c
	gcc $(CFLAGS) -o $@ $^

clean:
	-$(RM) $(PROG)
