#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pool.h"

#define MAX_CHARS 128

static int ret_code = 0;

int echo(int argc, char *argv[]) {
    for (int i = 1; i < argc; ++i) {
        printf("%s%c", argv[i], i == argc - 1 ? '\n' : ' ');
    }
    return argc - 1;


static int g_retcode;

#define APPS_X(X) \
        X(echo) \
        X(retcode) \
        X(pooltest) \


#define DECLARE(X) static int X(int, char *[]);
APPS_X(DECLARE)
#undef DECLARE

static const struct app {
        const char *name;
        int (*fn)(int, char *[]);
} app_list[] = {
#define ELEM(X) { # X, X },
        APPS_X(ELEM)
#undef ELEM
};

static int echo(int argc, char *argv[]) {
	for (int i = 1; i < argc; ++i) {
		printf("%s%c", argv[i], i == argc - 1 ? '\n' : ' ');
	}
	return argc - 1;
}

int retcode(int argc, char *argv[]) {
    printf("%d\n", ret_code);
    return 0;
}

int command_exec(char **words, int count_words) {
    if (!strcmp(words[0], "echo")) {
        return echo(count_words, words);
    } else if (!strcmp(words[0], "retcode")) {
        return retcode(count_words, words);
    } else {
        return -1;
    }
}

void parse_string(char **command, int count_strings) {
    char **words = NULL;
    for (int i = 0; i < count_strings; i++) {
        char *word = strtok(command[i], " ");
        int count_words = 0;
        while (word != NULL) {
            words = realloc(words, (count_words + 1) * sizeof(char *));
            words[count_words] = word;
            word = strtok(NULL, " ");
            count_words++;
        }
        ret_code = command_exec(words, count_words);
    }
    free(words);
}

int input_string() {
    while (1) {
        char *string = calloc(MAX_CHARS, sizeof(char));
        char **buffer = NULL;
        if (fgets(string, MAX_CHARS, stdin) == NULL) {
            return 0;
        }
        char *command = strtok(string, ";\n");
        int count_strings = 0;
        while (command != NULL) {
            buffer = realloc(buffer, (count_strings + 1) * sizeof(char *));
            buffer[count_strings] = command;
            count_strings++;
            command = strtok(NULL, ";\n");
        }
        parse_string(buffer, count_strings);
        free(string);
        free(buffer);
    }
}

int main(int argc, char *argv[]) {
    input_string();
    return 0;
}
