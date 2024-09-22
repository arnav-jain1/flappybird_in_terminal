CC = gcc
CFLAGS = -Wall -Wextra -pedantic

TARGET = flappy_bird
SRC = flappy_bird.c

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)
