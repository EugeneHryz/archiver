#define _CRT_SECURE_NO_WARNINGS
#pragma once

#include "buffer.h"

#define SENTINEL '$'

#define PREFIX '('
#define SUFFIX ')'

#define DELIMITER ','

int* get_prefix_function(const char *string);

int find_max_element(const int *array, int size, int start);

Code* get_next_code(const char *buffer, int begin, int buf_i, int word_length);

char* code_to_string(Code *code);

char* get_wrapped_string(const char *string, int length);

bool check_string(const char* string, int length);

void compress_file(FILE *SRC, FILE *DST);
