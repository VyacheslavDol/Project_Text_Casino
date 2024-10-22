#ifndef DB
#define DB

#include <postgresql/libpq-fe.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "../error/error.h"
#include "../lib/libdotenv/dot_env.h"

#define DB_SIZE 1024
#define USERS "users"
#define TOKEN "token"
#define MAIL "mail"
#define PROFILE "profile"

void db_connetc(PGconn **conn);
PGresult *db_get(PGconn **conn, const char *table_name, const char *src,
                 int num_args, ...);
int db_add(PGconn **conn, const char *table_name, int num_args, ...);
int db_update(PGconn **conn, const char *table_name, const char *src,
              int num_args, ...);

#endif
