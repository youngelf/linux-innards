threadStorage: threadStorage.c
	$(CC) -pthread $< -o $@

%.s: %.c
	$(CC) -S $< -o $@

clean:
	$(RM) .o
