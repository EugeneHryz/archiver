#pragma once

#include "buffer.h"
#include "tree.h"
#include "archiveview.h"

bool is_archive(FILE *encoded_file);

int get_buffer_pos(FILE *encoded_file, char *buffer);

int get_bit_from_buffer(FILE *encoded_file, char *buffer, char *accum, char *mask);

char get_byte_from_buffer(FILE *encoded_file, char *buffer, char *accum, char *mask);

void create_tree(FILE *encoded_file, char *buffer, char *accum, char *mask, Node *node);

char get_decoded_character(FILE *encoded_file, char *buffer, Node *node, char *accum, char *mask);

bool decode_file(FILE **encoded_file, FILE *decoded_file, const char *encoded_file_name, const char *file_to_decode);

int get_archived_file_info_pos(FILE *encoded_file, const char *file_name);

void remove_archived_file(FILE **encoded_file, const char* encoded_file_name, const char *file_to_remove);

void copy_file_contents(FILE *src_file, FILE *dst_file);
