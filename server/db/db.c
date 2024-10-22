#include "db.h"

void del_newline(const char *val) {
  char *newline = strchr(val, '\n');
  if (newline) {
    *newline = '\0';
  }
}

void db_connetc(PGconn **conn) {
  const char *db = getenv("DB");
  const char *dbuser = getenv("DB_USER");
  const char *dbpassword = getenv("DB_PASSWORD");

  char data[DB_SIZE];

  snprintf(data, sizeof(data), "dbname=%s user=%s password=%s", db, dbuser,
           dbpassword);

  *conn = PQconnectdb(data);

  if (PQstatus(*conn) != CONNECTION_OK) {
    fprintf(stderr, "Connection to db failed: %s", PQerrorMessage(*conn));
    PQfinish(*conn);
    error("FAILED TO CONNECT TO THE DATABASE");
    exit(EXIT_FAILURE);
  }

  printf("Connected to db\n");
}

PGresult *db_get(PGconn **conn, const char *table_name, const char *src,
                 int num_args, ...) {
  if (num_args <= 0) {
    fprintf(stderr, "Error: num_args must be greater than 0\n");
  }

  va_list args;
  va_start(args, num_args);

  char query[DB_SIZE];
  snprintf(query, sizeof(query), "SELECT ");

  const char *value = NULL;

  for (int i = 0; i < num_args - 1; i++) {
    value = va_arg(args, char *);
    del_newline(value);
    snprintf(query + strlen(query), sizeof(query) - strlen(query), "%s%s ",
             value, (i < num_args - 2) ? "," : "");
  }

  value = va_arg(args, char *);
  va_end(args);

  del_newline(value);
  del_newline(src);

  snprintf(query + strlen(query), sizeof(query) - strlen(query),
           "FROM %s WHERE %s = '%s'", table_name, value, src);

  PGresult *res = PQexec(*conn, query);

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    fprintf(stderr, "Get failed: %s", PQerrorMessage(*conn));
    PQclear(res);
  }

  return res;
}

int db_add(PGconn **conn, const char *table_name, int num_args, ...) {
  if (num_args <= 0) {
    fprintf(stderr, "Error: num_args must be greater than 0\n");
    return EXIT_FAILURE;
  }

  va_list args;
  va_start(args, num_args);

  char query[DB_SIZE];
  snprintf(query, sizeof(query), "INSERT INTO %s VALUES ( DEFAULT,",
           table_name);

  for (int i = 0; i < num_args; i++) {
    const char *value = va_arg(args, char *);
    del_newline(value);
    snprintf(query + strlen(query), sizeof(query) - strlen(query), "'%s'%s",
             value, (i < num_args - 1) ? "," : ")");
  }

  PGresult *res = PQexec(*conn, query);

  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    fprintf(stderr, "Insert failed: %s", PQerrorMessage(*conn));
    PQclear(res);
    va_end(args);
    return EXIT_FAILURE;
  }

  PQclear(res);
  va_end(args);

  return EXIT_SUCCESS;
}

int db_update(PGconn **conn, const char *table_name, const char *src,
              int num_args, ...) {
  if (num_args <= 0) {
    fprintf(stderr, "Error: num_args must be greater than 0\n");
    return EXIT_FAILURE;
  }

  va_list args;
  va_start(args, num_args);

  char query[DB_SIZE];
  snprintf(query, sizeof(query), "UPDATE %s SET ", table_name);

  const char *value = NULL;
  for (int i = 0; i < num_args - 1; i++) {
    value = va_arg(args, char *);
    del_newline(value);
    snprintf(query + strlen(query), sizeof(query) - strlen(query), "%s%s",
             value,
             (i + 1 == num_args - 1 ? "' " : ((i + 1) % 2 ? " = '" : "', ")));
  }
  value = va_arg(args, char *);
  va_end(args);

  del_newline(value);
  del_newline(src);

  snprintf(query + strlen(query), sizeof(query) - strlen(query),
           "WHERE %s = '%s'", value, src);

  PGresult *res = PQexec(*conn, query);

  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    fprintf(stderr, "Update failed: %s", PQerrorMessage(*conn));
    PQclear(res);
    return EXIT_FAILURE;
  }

  if (!atoi(PQcmdTuples(res))) {
    printf("Not found\n");
    PQclear(res);
    return EXIT_FAILURE;
  }

  PQclear(res);

  return EXIT_SUCCESS;
}