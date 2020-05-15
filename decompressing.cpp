#include "decompressing.h"


bool is_number(char c) {

    return (c >= '0' && c <= '9');
}

int convert_char_to_int(FILE *compressed_file, char *read_buffer, int *buf_i) {

    int result = 0;

    while (read_buffer_secure(compressed_file, read_buffer, buf_i) && is_number(*(read_buffer + *buf_i))) {

        result *= 10;
        result += *(read_buffer + *buf_i) - '0';
        (*buf_i)++;
    }
    return result;
}

void decompress_file(FILE *compressed_file, FILE *decompressed_file) {

    int rbuf_i, wbuf_i = 0, offset, match_length, cbuf_last = 0, temp_i = 0;

    char *read_buffer = (char*)malloc(MAX_BUFFER_LENGTH * sizeof(char));
    char *write_buffer = (char*)malloc(MAX_BUFFER_LENGTH * sizeof(char));
    char *copy_buffer = (char*)malloc(2 * MAX_BUFFER_LENGTH * sizeof(char));

    char *temp_word = (char*)malloc(MAX_MATCH_LENGTH * sizeof(char));

    rewind(compressed_file);
    if (!feof(compressed_file)) {

        rbuf_i = -1;

        while (read_buffer_secure(compressed_file, read_buffer, &rbuf_i)) {

            offset = 0;
            match_length = 0;

            while (read_buffer_secure(compressed_file, read_buffer, &rbuf_i) && read_buffer[rbuf_i] != PREFIX) {

                write_buffer_secure(decompressed_file, write_buffer, copy_buffer, &wbuf_i, &cbuf_last, (const char*)(read_buffer + rbuf_i), 1);
                rbuf_i++;
            }
            if (read_buffer_secure(compressed_file, read_buffer, &rbuf_i) && read_buffer[rbuf_i] == PREFIX) {

                rbuf_i++;
            }
            offset = convert_char_to_int(compressed_file, read_buffer, &rbuf_i);
            if (read_buffer_secure(compressed_file, read_buffer, &rbuf_i) && read_buffer[rbuf_i] == DELIMITER) {

                rbuf_i++;
            }
            match_length = convert_char_to_int(compressed_file, read_buffer, &rbuf_i);
            if (read_buffer_secure(compressed_file, read_buffer, &rbuf_i) && read_buffer[rbuf_i] == SUFFIX) {

                rbuf_i++;
            }

            if (read_buffer_secure(compressed_file, read_buffer, &rbuf_i) || (offset != 0 && match_length != 0)) {

                if (offset == 0 && match_length == 0) {

                    temp_i = rbuf_i;
                    while (read_buffer[rbuf_i] != SUFFIX) {

                        rbuf_i++;
                        match_length++;
                    }
                    write_string((const char*)(read_buffer + temp_i), temp_word, match_length);
                    temp_word[match_length] = '\0';
                }
                else {

                    write_string((const char*)(copy_buffer + cbuf_last + wbuf_i - offset), temp_word, match_length);
                    temp_word[match_length] = '\0';
                }

                write_buffer_secure(decompressed_file, write_buffer, copy_buffer, &wbuf_i, &cbuf_last, temp_word, match_length);
            }
        }

        write_buffer_secure(decompressed_file, write_buffer, copy_buffer, &wbuf_i, &cbuf_last, NULL, 0);
    }

}
