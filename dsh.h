#define MAXBUF 256  // max number of characteres allowed on command line
#define true 1
#define false 0

// TODO: Any global variables go below

// TODO: Function declarations go below

void execPath(char*, char**);

char** split(char*, char*);

int arrlen(char**);

int maxlen(char**);

int startsWith(char*, char*);

void strtrim(char*, char);