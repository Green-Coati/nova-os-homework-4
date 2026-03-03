/*
 * dsh.c
 *
 *  Created on: Aug 2, 2013
 *      Author: chiu
 */
#include "dsh.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <errno.h>
#include <err.h>
#include <sys/stat.h>
#include <string.h>


// TODO: Your function definitions below (declarations in dsh.h)

void execPath(char *path, char **args) {

    int background = 0;

    if (strcmp(args[arrlen(args) - 1], "&") == 0) {
        background = 1;
        args[arrlen(args) - 1] = NULL;
    }

    if (access(path, F_OK | X_OK) == 0) {
        pid_t pid = fork();
        if (pid == -1) {
            printf("Error: Unable to fork\n");
        } else if (pid == 0) {
            // we the child now. goodness gracious!
            if (execv(path, args) == -1) {
                printf("Error: Permission denied\n");
            }
        } else {
            if (background == true) {
                // uhhhhhh dont wait ig
                return;
            } else {
                wait(NULL); // we dont care about the status code right???
            }
        }
    } else {
        printf("Error: Executable not found\n");
    }
}

char** split(char *str, char *delim) {
    int len = strlen(str);
    int delimLen = strlen(delim);
    int numTokens = 1;
    for (int i = 0; i < len; i++) {
        int match = 1;
        for (int j = 0; j < delimLen; j++) {
            if (str[i+j] != delim[j]){
                match = 0;
            }
        }
        if (match == true) {
            numTokens++;
        }
    }

    char **tokens = (char**) malloc((numTokens + 1) * sizeof(char*));
    for (int i = 0; i < numTokens; i++) {
        tokens[i] = malloc(MAXBUF);
    }

    char* token = strtok(str, delim);

    for (int i = 0; i < numTokens; i++) {
        strcpy(tokens[i], token);
        token = strtok(NULL, delim);
    }

    tokens[numTokens] = NULL;

    return tokens;

}

int arrlen(char** array) {
    int len = 0;
    while (*array != NULL) {
        array++;
        len++;
    }
    return len;
}

int maxlen(char **array) {
	int max = 0;
	int index = 0;
	while (array[index] != NULL) {
		int len = strlen(array[index]);
		if (len > max) max = len;
		index++;
	}
	return max;
}

int startsWith(char* str1, char* str2) {
	if (strncmp(str1, str2, strlen(str2)) == 0) return 1;
	return 0;
}

void strtrim(char* str, char c) {
	int index = strlen(str) - 1;
	while (str[index] == c) {
		str[index] = '\0';
		index--;
	}

	index = 0;
	while (str[index] == c) {
		str++; // this could be a bad idea..... but its fine.......
	}
}