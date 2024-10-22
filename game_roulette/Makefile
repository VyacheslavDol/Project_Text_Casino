CC = gcc

CFLAGS = -Wall -Werror -Wextra -Wpedantic -O2 -g

SERV = server_roulette
CLI = client_roulette
SRC_DIR = ./source
SRCS = $(wildcard $(SRC_DIR)/*.c)

.PHONY: all clean

all: $(SERV) $(CLI)

$(SERV): 
	$(CC) $(CFLAGS) $(filter-out $(SRC_DIR)/$(CLI).c $(SRC_DIR)/roul_basic_client.c, $(SRCS)) -o server_roulette

$(CLI):
	$(CC) $(CFLAGS) $(filter-out $(SRC_DIR)/$(SERV).c $(SRC_DIR)/roul_basic_server.c, $(SRCS)) -o client_roulette 

clean:
	rm -f server_roulette client_roulette