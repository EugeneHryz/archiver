#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER_LENGTH 10240

#define MAX_WINDOW_LENGTH 6144
#define MAX_SEARCH_BUFFER_LENGTH 250

#define MAX_CODE_LENGTH 50
#define MAX_NUMBER_LENGTH 20

#define MIN_MATCH_LENGTH 7

struct Code {

    int offset;
    int length;
};

bool read_buffer_safe(FILE *SRC, char *buffer, int *buf_i);

bool set_pointers(const char* buffer, int *begin, int *buf_i, int *end, int offset);

void write_buffer_safe(FILE *DST, char *buffer, int *buf_i, const char *word, int word_length);

void write_string(const char *src, char *dst, int word_length);
