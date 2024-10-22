CC = gcc

CFLAGS = -Wall -Werror -Wextra -Wpedantic -O2 -g

SERV = server_black_jack
CLI = client_black_jack
SRC_DIR = ./source
SRCS = $(wildcard $(SRC_DIR)/*.c)

.PHONY: all clean

all: $(SERV) $(CLI)

$(SERV): 
	$(CC) $(CFLAGS) $(filter-out $(SRC_DIR)/$(CLI).c bl_j_basic_client.c, $(SRCS)) -o server_bj_test

$(CLI):
	$(CC) $(CFLAGS) $(filter-out $(SRC_DIR)/$(SERV).c bl_j_basic_server.c, $(SRCS)) -o client_bj_test 

clean:
	rm -f server_bj_test client_bj_test 