#include "buffer.h"

bool read_buffer_safe(FILE *SRC, char* buffer, int *buf_i) {

    bool result = true;
    if (*buf_i != -1) {

        if (*(buffer + *buf_i) == '\0' || *(buffer + *buf_i - 1) == '\n') {

            result = false;
        }
    }
    else
        if (fgets(buffer, MAX_BUFFER_LENGTH, SRC)) {

            *buf_i = 0;
        }
        else
            result = false;
    return result;
}

bool set_pointers(const char *buffer, int *begin, int *buf_i, int *end, int offset) {

    int length = offset;
    bool result = true;

    while (*(buffer + *buf_i) != '\0' && length > 0) {

        (*buf_i)++;
        length--;
    }
    while (*(buffer + *end) != '\0' && (*end - *buf_i) < MAX_SEARCH_BUFFER_LENGTH) {

        (*end)++;
    }
    if (*buf_i < MAX_WINDOW_LENGTH) {

        *begin = 0;
    }
    else {

        while ((*buf_i - *begin) > MAX_WINDOW_LENGTH) {

            (*begin)++;
        }
    }
    if (*buf_i == *end) {
        *begin = 0;
        *end = 0;
        result = false;
    }
    return result;
}

void write_string(const char *src, char *dst, int length) {

    int i = length;
    while (i > 0) {

        *(dst + i - 1) = *(src + i - 1);
        i--;
    }
}

void write_buffer_safe(FILE *DST, char *buffer, int *buf_i, const char *word, int word_length) {

    if (word && word_length) {

        if ((*buf_i + word_length) >= MAX_BUFFER_LENGTH) {

            *(buffer + *buf_i) = '\0';
            fprintf(DST, "%s", buffer);
            *buf_i = 0;
        }
        write_string(word, buffer + *buf_i, word_length);
        *buf_i += word_length;
    }
    else
    {
        *(buffer + *buf_i) = '\0';
        fprintf(DST, "%s", buffer);
    }
}
