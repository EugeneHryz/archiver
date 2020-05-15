#pragma once

#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER_SIZE 1024
#define CHARACTERS_NUMBER 256

struct Node {

    int count;
    char c;

    Node *left;
    Node *right;
};

struct Character {

    char c;
    int count;

    Character *next;
    Node *node;
};

void add_character_to_list(char c, int count, Node *node, Character **head);

Character* make_characters_list(int *characters);

Node* make_tree(Character **head);

void make_codes(Node *root, int level, char **codes);

void free_tree(Node *node);
