#include "decompressing.h"

bool read_buffer_secure(FILE *SRC, char *read_buffer, int *rbuf_i) {

    bool result = true;
    if (*rbuf_i == -1) {

        fgets(read_buffer, MAX_WINDOW_LENGTH, SRC);
        *rbuf_i = 0;
    }
    else {

        if (*(read_buffer + *rbuf_i) == '\0') {

            if (!feof(SRC)) {

                fgets(read_buffer, MAX_WINDOW_LENGTH, SRC);
                *rbuf_i = 0;
            }
            else
                result = false;
        }
    }
    return result;
}

void write_buffer_secure(FILE *DST, char *write_buffer, char* copy_buffer, int *wbuf_i, int *cbuf_last, const char *word, int word_length) {

    if (word && word_length) {

        if ((*wbuf_i + word_length) >= MAX_WINDOW_LENGTH) {

            *(write_buffer + *wbuf_i) = '\0';
            strcpy(copy_buffer, (const char*)write_buffer);
            *cbuf_last = strlen(write_buffer);
            fprintf(DST, "%s", write_buffer);
            *wbuf_i = 0;
        }
        write_string(word, write_buffer + *wbuf_i, word_length);
        write_string(word, (copy_buffer + *cbuf_last + *wbuf_i), word_length);
        *wbuf_i += word_length;
    }
    else
    {
        *(write_buffer + *wbuf_i) = '\0';
        fprintf(DST, "%s", write_buffer);
    }
}

bool is_number(char c) {

    return (c >= '0' && c <= '9');
}

int convert_char_to_int(FILE *ARC, char *read_buffer, int *buf_i) {

    int result = 0;

    while (read_buffer_secure(ARC, read_buffer, buf_i) && is_number(*(read_buffer + *buf_i))) {

        result *= 10;
        result += *(read_buffer + *buf_i) - '0';
        (*buf_i)++;
    }
    return result;
}

void decompress_file(FILE *ARC, FILE *OUT) {

    int rbuf_i, wbuf_i = 0, offset, match_length, cbuf_last = 0, temp_i = 0;

    char *read_buffer = (char*)malloc(MAX_BUFFER_LENGTH * sizeof(char));
    char *write_buffer = (char*)malloc(MAX_BUFFER_LENGTH * sizeof(char));
    char *copy_buffer = (char*)malloc(2 * MAX_BUFFER_LENGTH * sizeof(char));

    char *temp_word = (char*)malloc(MAX_MATCH_LENGTH * sizeof(char));

    if (!feof(ARC)) {

        rbuf_i = -1;

        while (read_buffer_secure(ARC, read_buffer, &rbuf_i)) {

            offset = 0;
            match_length = 0;

            while (read_buffer_secure(ARC, read_buffer, &rbuf_i) && read_buffer[rbuf_i] != PREFIX) {

                write_buffer_secure(OUT, write_buffer, copy_buffer, &wbuf_i, &cbuf_last, (const char*)(read_buffer + rbuf_i), 1);
                rbuf_i++;
            }
            if (read_buffer_secure(ARC, read_buffer, &rbuf_i) && read_buffer[rbuf_i] == PREFIX) {

                rbuf_i++;
            }
            offset = convert_char_to_int(ARC, read_buffer, &rbuf_i);
            if (read_buffer_secure(ARC, read_buffer, &rbuf_i) && read_buffer[rbuf_i] == DELIMITER) {

                rbuf_i++;
            }
            match_length = convert_char_to_int(ARC, read_buffer, &rbuf_i);
            if (read_buffer_secure(ARC, read_buffer, &rbuf_i) && read_buffer[rbuf_i] == SUFFIX) {

                rbuf_i++;
            }

            if (read_buffer_secure(ARC, read_buffer, &rbuf_i)) {

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

                write_buffer_secure(OUT, write_buffer, copy_buffer, &wbuf_i, &cbuf_last, temp_word, match_length);
            }
        }

        write_buffer_secure(OUT, write_buffer, copy_buffer, &wbuf_i, &cbuf_last, NULL, 0);
    }

}
