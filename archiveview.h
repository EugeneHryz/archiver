#define _CRT_SECURE_NO_WARNINGS
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH 260

struct Strings {

    char *string;
    Strings *next;
};

int get_info_block_pos(FILE *encoded_file);

void add_new_string(const char *new_str, Strings **head);

Strings* get_archived_file_names(const char *archive_name);

void display_list_contents(Strings *head);

