CFLAGS = -Wall -Wextra -pedantic -Isrc/include

SRCFILES = $(wildcard *.c)
OBJFILES = $(patsubst %.c,%.o,$(SRCFILES))

TARGET = ../../admin_monitor

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) $^ -o $@

$(BINDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET)
