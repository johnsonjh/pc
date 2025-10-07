RM=rm -f

.PHONY: all
all: pc

pc: pc.c
	$(CC) $(CFLAGS) $(LDFLAGS) pc.c -o pc

.PHONY: clean
clean:
	$(RM) pc
