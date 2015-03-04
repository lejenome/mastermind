all: mastermindcli
mastermindcli: core.o cli.o
	$(CC) -o $@ $^
%.o: src/%.c
	$(CC) -c $^
