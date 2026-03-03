/*
 * main.c
 *
 *  Created on: Mar 17 2017
 *      Author: david
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <string.h>
#include "dsh.h"
#include <sys/types.h>

#define MAX_PROC 1024

int main(int argc, char *argv[]) {

	// DO NOT REMOVE THE BLOCK BELOW (FORK BOMB PREVENTION) //
	struct rlimit limit;
	limit.rlim_cur = MAX_PROC;
	limit.rlim_max = MAX_PROC;
	setrlimit(RLIMIT_NPROC, &limit);
	// DO NOT REMOVE THE BLOCK ABOVE THIS LINE //

	char *line = malloc(MAXBUF);

	char **environPaths = split(getenv("PATH"), ":");

	char *home = malloc(MAXBUF);
	strcpy(home, getenv("HOME"));

	int len = arrlen(environPaths) + 2;

	char **paths = malloc(len * sizeof(char*));

	paths[0] = malloc(MAXBUF);
	strcpy(paths[0], "DUMMY PATH"); // i dont think this is necessary but it couldnt hurt

	for (int i = 1; i < len - 1; i++) { // index 0 of paths will store path of cwd
		paths[i] = malloc(MAXBUF);
		strcpy(paths[i], environPaths[i-1]);
	}

	paths[len - 1] = NULL;

	int pathInd = 0;
	while (environPaths[pathInd] != NULL) {
		free(environPaths[pathInd]);
		pathInd++;
	}
	free(environPaths);

	char *cwd = malloc(MAXBUF);
	getcwd(cwd, MAXBUF);
	char *interpretedPath = malloc(MAXBUF);

	while (true) {

		if (startsWith(cwd, home)) {
			strcpy(interpretedPath, "~");
			strcat(interpretedPath, cwd + strlen(home));
		} else {
			strcpy(interpretedPath, cwd);
		}

		printf("dsh %s > ", interpretedPath);

		if (fgets(line, 256, stdin) == NULL) {
			printf("\n");
			free(line);
			int i = 0;
			while (paths[i] != NULL) {
				free(paths[i]);
				i++;
			}
			free(paths);
			free(cwd);
			free(home);
			free(interpretedPath);
			exit(0);
		}

		line[strlen(line) - 1] = '\0';
		strtrim(line, ' ');

		if (line[0] == '/') { // handles absolute paths
			char **args = split(line, " ");
			char *path = args[0];
			execPath(path, args);
			int index = 0;
			while (args[index] != NULL) {
				free(args[index]);
				index++;
			}
			free(args);
		} else { // handles anything else
			strcpy(paths[0], cwd); // copies cwd into var that stores paths to check them all at once
			int index = 0;
			char *fullLine = malloc(maxlen(paths) + strlen(line) + 2);
			char **args = split(line, " ");
			char *path = args[0];
			int found = false;

			while (paths[index] != NULL) {
				strcpy(fullLine, paths[index]);
				strcat(fullLine, "/");
				strcat(fullLine, path);
				if (access(fullLine, F_OK | X_OK) == 0) { // found the correct executable path
					found = true;
					strcpy(args[0], fullLine);
					execPath(path, args);
					break;
				}
				index++;
			}

			free(fullLine);

			if (!found) { // check built-in commands
				if (strcmp(path, "exit") == 0) { // exit!
					free(line);
					int i = 0;
					while (paths[i] != NULL) {
						free(paths[i]);
						i++;
					}
					free(paths);
					i = 0;
					while (args[i] != NULL) {
						free(args[i]);
						i++;
					}
					free(args);
					free(cwd);
					free(home);
					free(interpretedPath);
					exit(0);
				} else if (strcmp(path, "cd") == 0) { // cd!
					if (args[1] != NULL) { // cd to path
						if (args[1][0] == '/') { // absolute path
							if (access(args[1], F_OK) == 0) {
								chdir(args[1]);
								getcwd(cwd, MAXBUF);
							} else {
								printf("%s: no such file or directory\n", args[1]);
							}
						} else if (args[1][0] == '~') { // relative to home directory
							char *relative = args[1] + 1;
							char *cdPath = malloc(strlen(home) + strlen(relative) + 1);
							strcpy(cdPath, home);
							strcat(cdPath, relative);
							if (access(cdPath, F_OK) == 0) {
								chdir(cdPath);
								getcwd(cwd, MAXBUF);
							} else {
								printf("%s: no such file or directory\n", args[1]);
							}
							free(cdPath);
						} else { // use relative filepath
							char *cdPath = malloc(strlen(cwd) + strlen(args[1]) + 2);
							strcpy(cdPath, cwd);
							strcat(cdPath, "/");
							strcat(cdPath, args[1]);
							if (access(cdPath, F_OK) == 0) {
								chdir(cdPath);
								getcwd(cwd, MAXBUF);
							} else {
								printf("%s: no such file or directory\n", args[1]);
							}
							free(cdPath);
						}
					} else {
						if (access(home, F_OK) == 0) {
							chdir(home);
							getcwd(cwd, MAXBUF);
						} else {
							printf("%s: no such file or directory\n", home);
						}
					}
					getcwd(cwd, MAXBUF);
				} else {
					printf("Error: %s not found\n", path);
				}
			}

			int i = 0;
			while (args[i] != NULL) {
				free(args[i]);
				i++;
			}
			free(args);
		}
	}

	return 0;
}