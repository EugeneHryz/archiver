#define _CRT_SECURE_NO_WARNINGS
#pragma once

#include "buffer.h"

#define DELIMITER ','
#define PREFIX '('
#define SUFFIX ')'

#define MAX_MATCH_LENGTH 500

bool read_buffer_secure(FILE *SRC, char *read_buffer, int *rbuf_i);

void write_buffer_secure(FILE *DST, char *write_buffer, char *copy_buffer, int *wbuf_i, int *cbuf_last, const char *word, int word_length);

bool is_number(char c);

int convert_char_to_int(FILE *ARC, char *read_buffer, int *buf_i);

void decompress_file(FILE *ARC, FILE *OUT);
