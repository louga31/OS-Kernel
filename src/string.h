#pragma once
#include <types.h>

size_t strlen(const char* str);

char* strcpy(char* s1, const char* s2);
char* strncpy(char* s1, const char* s2, size_t n);

int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, size_t n);

char* strtok(char* s1, const char* s2);