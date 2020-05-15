#include "tree.h"

void add_character_to_list(char c, int count, Node *node, Character **head) {

    Character *new_character, *previous, *cursor;
    previous = NULL;
    cursor = *head;

    new_character = (Character*)malloc(sizeof(Character));

    new_character->c = c;
    new_character->count = count;
    new_character->node = node;
    new_character->next = NULL;

    while (cursor && count > cursor->count) {

        previous = cursor;
        cursor = cursor->next;
    }
    if (previous == NULL) {

        *head = new_character;
        new_character->next = cursor;
    }
    else
    {
        new_character->next = previous->next;
        previous->next = new_character;
    }
}

Character* make_characters_list(int *characters) {

    Character *head = NULL;
    int i;

    for (i = 0; i < CHARACTERS_NUMBER; i++) {

        if (characters[i] != 0) {

            add_character_to_list(char(i), characters[i], NULL, &head);
        }
    }
    return head;
}

Node* make_tree(Character **head) {

    Character *cursor, *elmnt1, *elmnt2;
    Node *parent, *node1, *node2;

    parent = NULL;
    cursor = *head;

    while (cursor) {

        elmnt1 = cursor;
        if (cursor->next) {

            elmnt2 = elmnt1->next;
            cursor = elmnt2->next;

            parent = (Node*)malloc(sizeof(Node));

            if (elmnt1->node) {

                parent->left = elmnt1->node;
            }
            else {

                node1 = (Node*)malloc(sizeof(Node));

                node1->c = elmnt1->c;
                node1->count = elmnt1->count;
                node1->left = NULL;
                node1->right = NULL;
                parent->left = node1;
            }

            if (elmnt2->node) {

                parent->right = elmnt2->node;
            }
            else {

                node2 = (Node*)malloc(sizeof(Node));

                node2->c = elmnt2->c;
                node2->count = elmnt2->count;
                node2->left = NULL;
                node2->right = NULL;
                parent->right = node2;
            }

            parent->count = parent->left->count + parent->right->count;

            if (cursor) {

                add_character_to_list(0, parent->count, parent, &cursor);
            }
        }
        else {

            parent = (Node*)malloc(sizeof(Node));

            parent->c = cursor->c;
            parent->count = cursor->count;
            parent->left = NULL;
            parent->right = NULL;

            return parent;
        }
    }

    return parent;
}

void make_codes(Node *node, int level, char **codes) {

    static char code[CHARACTERS_NUMBER];

    if (node->left && node->right) {

        code[level] = '0';
        make_codes(node->left, level + 1, codes);

        code[level] = '1';
        make_codes(node->right, level + 1, codes);
    }
    else {

        code[level] = '\0';

        codes[(unsigned char)(node->c)] = (char*)malloc(sizeof(char) * (strlen(code) + 1));
        strcpy_s(codes[(unsigned char)(node->c)], strlen(code) + 1, code);
    }
}

void free_tree(Node *node) {

    if (node->left && node->right) {

        free_tree(node->left);
        free_tree(node->right);

        free(node);

    } else {

        free(node);
    }
}
