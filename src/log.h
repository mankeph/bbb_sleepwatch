#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* logfile;

int init_log(char* str_logfile);

void logm(char* message);
void logn(char* message, int nr);
void logc(char* message, char* code);
void logcn(char* message, char* code, int nr);