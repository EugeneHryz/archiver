#include "coding.h"

int get_characters_count(FILE *compressed_file, int *characters) {

    char *buffer = (char*)malloc(MAX_BUFFER_SIZE * sizeof(char));

    int buf_length = 0;
    int file_length = 0;
    int i;

    while (!feof(compressed_file)) {

        buf_length = fread(buffer, sizeof(char), MAX_BUFFER_SIZE, compressed_file);

        for (i = 0; i < buf_length; i++) {

            characters[(unsigned char)(buffer[i])] = characters[(unsigned char)(buffer[i])] + 1;
            file_length++;
        }
    }
    return file_length;
}

void write_bit_to_buffer(FILE *encoded_file, char *buffer, int *buf_i, int *length, char *accum, char *mask, int bit) {

    if (bit == 1) {

        *accum |= *mask;
    }
    *mask = *mask << 1;

    if (*mask == 0) {

        *(buffer + *buf_i) = *accum;
        (*buf_i)++;
        (*length)++;

        *accum = 0;
        *mask = 1;

        if (*buf_i == MAX_BUFFER_SIZE) {

            fwrite((const char*)buffer, sizeof(char), *buf_i, encoded_file);
            *buf_i = 0;
        }
    }
}

void flush_buffer_to_file(FILE *encoded_file, char *buffer, int *buf_i, int *length, char *accum, char *mask) {

    if (*mask != 1) {

        *(buffer + *buf_i) = *accum;
        (*buf_i)++;
        (*length)++;
    }
    fwrite(buffer, sizeof(char), *buf_i, encoded_file);
}

void write_byte_to_buffer(FILE *encoded_file, char *buffer, int *buf_i, int *length, char *accum, char *mask, char byte) {

    char mask_byte = 1;

    while (mask_byte) {

        if ((byte & mask_byte) != 0) {

            *accum |= *mask;
        }
        *mask = *mask << 1;

        if (*mask == 0) {

            *(buffer + *buf_i) = *accum;
            (*buf_i)++;
            (*length)++;

            *accum = 0;
            *mask = 1;

            if (*buf_i == MAX_BUFFER_SIZE) {

                fwrite((const char*)buffer, sizeof(char), *buf_i, encoded_file);
                *buf_i = 0;
            }
        }
        mask_byte = mask_byte << 1;
    }
}

int write_tree(FILE *encoded_file, char *buffer, int *buf_i, int *length, char *accum, char *mask, Node *root) {

    static int node_count;

    if (root->left && root->right) {

        write_bit_to_buffer(encoded_file, buffer, buf_i, length, accum, mask, 0);

        write_tree(encoded_file, buffer, buf_i, length, accum, mask, root->left);
        write_tree(encoded_file, buffer, buf_i, length, accum, mask, root->right);

        node_count++;
    }
    else {

        write_bit_to_buffer(encoded_file, buffer, buf_i, length, accum, mask, 1);
        write_byte_to_buffer(encoded_file, buffer, buf_i, length, accum, mask, root->c);

        node_count++;
    }
    return node_count;
}

void encode_file(FILE *compressed_file, FILE *encoded_file, const char *file_name, Node *root, char **codes, int file_length, int offset) {

    char *write_buffer = (char*)malloc(MAX_BUFFER_SIZE * sizeof(char));
    char *read_buffer = (char*)malloc(MAX_BUFFER_SIZE * sizeof(char));
    char *archive_info_buffer = (char*)malloc(MAX_BUFFER_LENGTH * sizeof(char));

    int buf_i = 0, i = 0, j = 0, rbuf_length = 0, node_count = 0, length = 0, info_block_l = 0;
    char accum = 0, mask = 1;

    rewind(compressed_file);
    fseek(encoded_file, offset, SEEK_SET);

    if (offset > 0) {

        info_block_l = fread(archive_info_buffer, sizeof(char), MAX_BUFFER_LENGTH, encoded_file);
        fseek(encoded_file, offset, SEEK_SET);
    }

    length += fprintf(encoded_file, "#ARC#");
    length += fwrite(&file_length, sizeof(int), 1, encoded_file) * sizeof(int);

    node_count = write_tree(encoded_file, write_buffer, &buf_i, &length, &accum, &mask, root);

    while (!feof(compressed_file)) {

        rbuf_length = fread(read_buffer, sizeof(char), MAX_BUFFER_SIZE, compressed_file);

        for (i = 0; i < rbuf_length; i++) {

            for (j = 0; codes[(unsigned char)(read_buffer[i])][j] != '\0'; j++) {

                if (codes[(unsigned char)(read_buffer[i])][j] == '0') {

                    write_bit_to_buffer(encoded_file, write_buffer, &buf_i, &length, &accum, &mask, 0);
                }
                else {

                    write_bit_to_buffer(encoded_file, write_buffer, &buf_i, &length, &accum, &mask, 1);
                }

            }
        }
    }
    flush_buffer_to_file(encoded_file, write_buffer, &buf_i, &length, &accum, &mask);

    write_string(file_name, archive_info_buffer + info_block_l, strlen(file_name));
    info_block_l += strlen(file_name);

    fwrite(archive_info_buffer, sizeof(char), info_block_l, encoded_file);
    fprintf(encoded_file, "\r\n");
    fwrite(&offset, sizeof(int), 1, encoded_file);
    fwrite(&length, sizeof(int), 1, encoded_file);
    fprintf(encoded_file, "\r\n");

    free(write_buffer);
    free(read_buffer);
    free(archive_info_buffer);
}
