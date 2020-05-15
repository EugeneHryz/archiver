#define _CRT_SECURE_NO_WARNINGS
#pragma once

#include "buffer.h"

#define DELIMITER ','
#define PREFIX '('
#define SUFFIX ')'

#define MAX_MATCH_LENGTH 500

bool is_number(char c);

int convert_char_to_int(FILE *compressed_file, char *read_buffer, int *buf_i);

void decompress_file(FILE *compressed_file, FILE *decompressed_file);
