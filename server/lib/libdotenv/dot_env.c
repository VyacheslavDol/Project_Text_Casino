#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __linux__
#include <sys/types.h>
#endif

#define ENV_FILE_NAME ".env"
#define COMMENT_CHAR  '#'
#define SEPERATOR     "="
#define NEW_LINE      "\n"
#define ENV_SIZE 8192

static char*
trim_whitespace(char *str)
{
    char *end;

    while (isspace((unsigned char)*str)) {
        str++;
    }

    if (*str == 0) {
        return str;
    }

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }

    end[1] = '\0';

    return str;
}

static int
replace_double_quotes(char *value)
{
    if (value == NULL || strlen(value) == 0) {
        return 0;
    }

    int last = strlen(value)-1;
    if (value[0] == '\'' && value[last] == '\'') {
        value[0] = '"';
        value[last] = '"';
    }

    return 0;
}

int
env_load(const char *path, const int overwrite)
{
    char full_path[strlen(path) + strlen(ENV_FILE_NAME) + 1];
    
    if (path == NULL || path[0] == '\0') {
        strcpy(full_path, ENV_FILE_NAME);
    } else {
        strcpy(full_path, path);
    }

    FILE* f = fopen(full_path, "rb");
    if (f == NULL) {
        return 1;
    }

    char line[ENV_SIZE];
    
    while (fgets(line, sizeof(line), f)) {
        // ignore lines starting with # and whitespace
        if (COMMENT_CHAR == line[0] || isspace(line[0])) {
            continue;
        }
        
        char *line_ptr = line;
        char *key = strsep(&line_ptr, SEPERATOR);
        char *value = strsep(&line_ptr, NEW_LINE);

        replace_double_quotes(value);

        if (setenv(key, trim_whitespace(value), overwrite) != 0) {
            fclose(f);
            return 1;
        }
    }

    fclose(f);

    return 0;
}
