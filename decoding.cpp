#include "decoding.h"

bool is_archive(FILE *encoded_file) {

    bool result = false;
    char header[6];

    if (!feof(encoded_file)) {

        fgets(header, 6, encoded_file);
        if (strcmp(header, "#ARC#") == 0) {

            result = true;
        }
    }
    return result;
}

int get_buffer_pos(FILE *encoded_file, char *buffer) {

    static int length = 0;
    static int buf_pos = 0;

    buf_pos++;

    if (buffer == NULL) {

        length = 0;
        buf_pos = 0;
        return -1;
    }
    if (buf_pos < length) {

        return buf_pos;
    }
    else {

        if (!feof(encoded_file)) {

            length = fread(buffer, sizeof(char), MAX_BUFFER_LENGTH, encoded_file);
            buf_pos = 0;

            return buf_pos;
        }
        else
            return -1;
    }
}

int get_bit_from_buffer(FILE *encoded_file, char *buffer, char *accum, char *mask) {

    int bit = 0, pos;

    if (*mask == 0) {

        pos = get_buffer_pos(encoded_file, buffer);

        if (pos != -1) {

            *accum = *(buffer + pos);
            *mask = 1;
        }
    }
    if (*mask) {

        if ((*accum & *mask) != 0) {

            bit = 1;
        }
        *mask = *mask << 1;
    }
    return bit;
}

char get_byte_from_buffer(FILE *encoded_file, char *buffer, char *accum, char *mask) {

    char byte = 0, mask_byte = 1;
    int pos;

    while (mask_byte) {

        if (*mask == 0) {

            pos = get_buffer_pos(encoded_file, buffer);

            if (pos != -1) {

                *accum = *(buffer + pos);
                *mask = 1;
            }
        }

        if (*mask) {

            if ((*accum & *mask) != 0) {

                byte |= mask_byte;
            }
            *mask = *mask << 1;
        }
        mask_byte = mask_byte << 1;
    }
    return byte;
}

void create_tree(FILE *encoded_file, char *buffer, char *accum, char *mask, Node *node) {

    if (get_bit_from_buffer(encoded_file, buffer, accum, mask) == 0) {

        node->left = (Node*)malloc(sizeof(Node));
        create_tree(encoded_file, buffer, accum, mask, node->left);

        node->right = (Node*)malloc(sizeof(Node));
        create_tree(encoded_file, buffer, accum, mask, node->right);
    }
    else {

        node->c = get_byte_from_buffer(encoded_file, buffer, accum, mask);
        node->left = NULL;
        node->right = NULL;
    }
}

char get_decoded_character(FILE *encoded_file, char *buffer, Node *node, char *accum, char *mask) {

    static unsigned char c;

    if (node->left != NULL && node->right != NULL) {

        if (get_bit_from_buffer(encoded_file, buffer, accum, mask) == 1) {
            get_decoded_character(encoded_file, buffer, node->right, accum, mask);
        }
        else {
            get_decoded_character(encoded_file, buffer, node->left, accum, mask);
        }
    }
    else {

        c = node->c;
    }
    return c;
}

bool decode_file(FILE **encoded_file, FILE *decoded_file, const char *encoded_file_name, const char *file_to_decode) {

    char *read_buffer = (char*)malloc(MAX_BUFFER_LENGTH * sizeof(char));
    char *write_buffer = (char*)malloc(MAX_BUFFER_LENGTH * sizeof(char));

    Node *root = (Node*)malloc(sizeof(Node));
    int buf_i = 0, file_length = 0, info_block_pos = 0, position, length;
    char accum = 0, mask = 0, c;
    char *relative_path = (char*)malloc(MAX_PATH * sizeof(char));

    info_block_pos = get_archived_file_info_pos(*encoded_file, file_to_decode);
    fseek(*encoded_file, info_block_pos, SEEK_SET);
    fgets(relative_path, MAX_PATH, *encoded_file);
    fread(&position, sizeof(int), 1, *encoded_file);
    fread(&length, sizeof(int), 1, *encoded_file);

    fseek(*encoded_file, position, SEEK_SET);

    if (!is_archive(*encoded_file)) {
        return false;
    }
    fread(&file_length, sizeof(int), 1, *encoded_file);
    create_tree(*encoded_file, read_buffer, &accum, &mask, root);

    while (file_length > 0) {

        c = get_decoded_character(*encoded_file, read_buffer, root, &accum, &mask);
        write_character_to_buffer(decoded_file, write_buffer, &buf_i, c);

        file_length--;
    }
    fwrite(write_buffer, sizeof(char), buf_i, decoded_file);
    get_buffer_pos(*encoded_file, NULL);

    remove_archived_file(encoded_file, encoded_file_name, file_to_decode);

    free_tree(root);
    free(read_buffer);
    free(write_buffer);
    free(relative_path);

    return true;
}

int get_archived_file_info_pos(FILE *encoded_file, const char *file_name) {

    char *buffer = (char*)malloc(MAX_PATH * sizeof(char));
    int start_pos = get_info_block_pos(encoded_file), pos = 0;

    if (start_pos > 0) {

        fseek(encoded_file, start_pos, SEEK_SET);
        while (fgets(buffer, MAX_PATH, encoded_file)) {

            buffer[strlen(buffer) - 2] = '\0';
            if (strcmp(file_name, buffer) == 0) {
                pos = start_pos;
                break;
            }
            fseek(encoded_file, 2 * sizeof(int) + 2, SEEK_CUR);
            start_pos += strlen(buffer) + 12;
        }
    }
    return pos;
}

void remove_archived_file(FILE **encoded_file, const char *encoded_file_name, const char *file_to_remove) {

    FILE *temp_file = tmpfile();

    int info_pos, info_length = 0, position, length, temp, bytes_to_copy, bytes_read;
    char *relative_path = (char*)malloc(MAX_PATH * sizeof(char));
    char *buffer = (char*)malloc(MAX_BUFFER_LENGTH * sizeof(char));

    info_pos = get_archived_file_info_pos(*encoded_file, file_to_remove);
    fseek(*encoded_file, info_pos, SEEK_SET);
    info_length += strlen(fgets(relative_path, MAX_PATH, *encoded_file)) + 2 * sizeof(int) + 2;

    fread(&position, sizeof(int), 1, *encoded_file);
    fread(&length, sizeof(int), 1, *encoded_file);

    fseek(*encoded_file, info_pos + info_length, SEEK_SET);
    while (fgets(relative_path, MAX_PATH, *encoded_file)) {

        fread(&temp, sizeof(int), 1, *encoded_file);
        temp -= length;

        fseek(*encoded_file, -4, SEEK_CUR);
        fwrite(&temp, sizeof(int), 1, *encoded_file);
        fseek(*encoded_file, 6, SEEK_CUR);
    }

    rewind(*encoded_file);
    while (!feof(*encoded_file)) {

        bytes_read = fread(buffer, sizeof(char), MAX_CODE_LENGTH, *encoded_file);
        fwrite(buffer, sizeof(char), bytes_read, temp_file);
    }
    fclose(*encoded_file);
    remove(encoded_file_name);
    fopen_s(encoded_file, encoded_file_name, "wb+");

    bytes_to_copy = position;
    if (bytes_to_copy > 0) {

        rewind(temp_file);
        while (bytes_to_copy > 0) {

            fputc(fgetc(temp_file), *encoded_file);
            bytes_to_copy--;
        }
    }
    fseek(temp_file, position + length, SEEK_SET);
    bytes_to_copy = position + length;

    while (bytes_to_copy < info_pos) {

        fputc(fgetc(temp_file), *encoded_file);
        bytes_to_copy++;
    }
    fseek(temp_file, info_length, SEEK_CUR);

    while (!feof(temp_file)) {

        bytes_read = fread(buffer, sizeof(char), MAX_CODE_LENGTH, temp_file);
        fwrite(buffer, sizeof(char), bytes_read, *encoded_file);
    }

    fseek(*encoded_file, 0, SEEK_END);
    if (ftell(*encoded_file) == 0) {

        fclose(*encoded_file);
        remove(encoded_file_name);
    }
    fclose(temp_file);
}

void copy_file_contents(FILE *src_file, FILE *dst_file) {

    char *buffer = (char*)malloc(MAX_BUFFER_LENGTH * sizeof(char));
    int buf_l = 0;

    rewind(src_file);
    rewind(dst_file);

    while (!feof(src_file)) {

        buf_l = fread(buffer, sizeof(char), MAX_BUFFER_LENGTH, src_file);
        fwrite(buffer, sizeof(char), buf_l, dst_file);
    }

    free(buffer);
}
