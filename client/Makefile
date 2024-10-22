CC = gcc

#CFLAGS = -Wall -Werror -Wextra -Wpedantic -O2 -g -Wshadow -Wconversion -fsanitize=address,undefined -fanalyzer -Wstack-protector -Wunused -Wformat-security -flto -funroll-loops -fstack-protector-all

CFLAGS = -Wall -Werror -Wextra -Wpedantic -O2 -g

LDFLAGS = -lssl -lcrypto -lcjson

SRC = main.c client/client.c error/error.c components/*.c components/*/*.c service/*.c black_jack/*.c  ssl/*.c roulette/*.c

TARGET = start_client

all: clean $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -rf $(TARGET)