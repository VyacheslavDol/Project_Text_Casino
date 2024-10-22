CC = gcc

CFLAGS = -Wall -Werror -Wextra -Wpedantic -O2 -g

LDFLAGS = -lpq -lcrypto -lcurl -lmicrohttpd -luuid -lssl

SRC = main.c server/*.c db/db.c lib/libdotenv/dot_env.c router/router.c error/error.c controller/auth_controller.c service/*.c lib/bcrypt/bcrypt.a ssl/ssl.c roulette/*.c

TARGET = start_server

SRC_BL = black_jack

SRC_R = roulette

LIB_NAME = black_jack.a

LIB_NAME_R = roulette.a

all: clean bcrypt $(LIB_NAME) $(LIB_NAME_R) $(TARGET)

bcrypt:
	$(MAKE) -C lib/bcrypt

$(LIB_NAME):
	cd $(SRC_BL) && $(CC) $(CFLAGS) -c *.c && ar rcs $(LIB_NAME) *.o

$(LIB_NAME_R):
	cd $(SRC_R) && $(CC) $(CFLAGS) -c *.c && ar rcs $(LIB_NAME_R) *.o

$(TARGET):
	$(CC) $(CFLAGS) $(SRC) $(SRC_BL)/$(LIB_NAME) $(SRC_R)/$(LIB_NAME_R) -o $(TARGET) $(LDFLAGS)

clean: clean_bcrypt clean_bl clean_r
	rm -rf $(TARGET) *.o *.a
	
clean_bcrypt:
	$(MAKE) clean -C lib/bcrypt

clean_bl:
	cd $(SRC_BL) && rm -rf $(LIB_NAME) *.o

clean_r:
	cd $(SRC_R) && rm -rf $(LIB_NAME_R) *.o