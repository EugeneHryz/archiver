#define _CRT_SECURE_NO_WARNINGS
#pragma once

#include "tree.h"
#include "buffer.h"

int get_characters_count(FILE *compressed_file, int *characters);

void write_bit_to_buffer(FILE *encoded_file, char *buffer, int *buf_i, int *length, char *accum, char *mask, int bit);

void flush_buffer_to_file(FILE *encoded_file, char *buffer, int *buf_i, int *length, char *accum, char *mask);

void write_byte_to_buffer(FILE *encoded_file, char *buffer, int *buf_i, int *length, char *accum, char *mask, char byte);

int write_tree(FILE *encoded_file, char *buffer, int *buf_i, int *length, char *accum, char *mask, Node *root);

void encode_file(FILE *compressed_file, FILE *encoded_file, const char *file_name, Node *root, char **codes, int file_length, int offset);
