CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic
TARGET = isola

SRCS = main.c game.c map.c squad.c utils.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
# Gestione pulizia cross-platform
ifeq ($(OS),Windows_NT)
	del /f /q $(OBJS) $(TARGET).exe 2>NUL || true
else
	rm -f $(OBJS) $(TARGET)
endif

.PHONY: all clean