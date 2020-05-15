#include "archiveview.h"

int get_info_block_pos(FILE *encoded_file) {

    int pos = 0, length = 0;
    rewind(encoded_file);

    if (!feof(encoded_file)) {

        fseek(encoded_file, -(sizeof(int) * 2 + 2), SEEK_END);

        fread(&pos, sizeof(int), 1, encoded_file);
        fread(&length, sizeof(int), 1, encoded_file);
        pos += length;
    }
    return pos;
}

void add_new_string(const char *new_str, Strings **head) {

    Strings *cursor = *head;

    Strings *new_item = (Strings*)malloc(sizeof(Strings));
    new_item->string = (char*)malloc(MAX_PATH * sizeof(char));
    strcpy(new_item->string, new_str);
    new_item->next = NULL;

    if (cursor == NULL) {
        *head = new_item;
    } else {

        while (cursor->next != NULL) {
            cursor = cursor->next;
        }
        cursor->next = new_item;
    }
}

Strings* get_archived_file_names(const char *archive_name) {

    FILE *archived_file = NULL;
    fopen_s(&archived_file, archive_name, "rb");

    Strings *head = NULL;
    char *file_name = (char*)malloc(MAX_PATH * sizeof(char));
    int pos = get_info_block_pos(archived_file);

    if (pos > 0) {

        fseek(archived_file, pos, SEEK_SET);
        while (fgets(file_name, MAX_PATH, archived_file)) {

            file_name[strlen(file_name) - 2] = '\0';
            add_new_string((const char*)file_name, &head);

            fseek(archived_file, 2 * sizeof(int) + 2, SEEK_CUR);
        }
    }
    fclose(archived_file);

    return head;
}

void display_list_contents(struct Strings *head) {

    Strings *cursor = head;
    while (cursor != NULL) {

        printf("\n%s\n", cursor->string);
        cursor = cursor->next;
    }
}
