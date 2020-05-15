#include "buffer.h"

bool read_buffer_safe(FILE *source_file, char* buffer, int *buf_i) {

    bool result = true;
    if (*buf_i != -1) {

        if (*(buffer + *buf_i) == '\0' || *(buffer + *buf_i - 1) == '\n') {

            result = false;
        }
    }
    else
        if (fgets(buffer, MAX_BUFFER_LENGTH, source_file)) {

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

void write_buffer_safe(FILE *compressed_file, char *buffer, int *buf_i, const char *word, int word_length) {

    if (word && word_length) {

        if ((*buf_i + word_length) >= MAX_BUFFER_LENGTH) {

            *(buffer + *buf_i) = '\0';
            fprintf(compressed_file, "%s", buffer);
            *buf_i = 0;
        }
        write_string(word, buffer + *buf_i, word_length);
        *buf_i += word_length;
    }
    else
    {
        *(buffer + *buf_i) = '\0';
        fprintf(compressed_file, "%s", buffer);
    }
}



bool read_buffer_secure(FILE *compressed_file, char *read_buffer, int *rbuf_i) {

    bool result = true;
    if (*rbuf_i == -1) {

        fgets(read_buffer, MAX_WINDOW_LENGTH, compressed_file);
        *rbuf_i = 0;
    }
    else {

        if (*(read_buffer + *rbuf_i) == '\0') {

            if (!feof(compressed_file)) {

                fgets(read_buffer, MAX_WINDOW_LENGTH, compressed_file);
                *rbuf_i = 0;
            }
            else
                result = false;
        }
    }
    return result;
}

void write_buffer_secure(FILE *decompressed_file, char *write_buffer, char* copy_buffer, int *wbuf_i, int *cbuf_last, const char *word, int word_length) {

    if (word && word_length) {

        if ((*wbuf_i + word_length) >= MAX_WINDOW_LENGTH) {

            *(write_buffer + *wbuf_i) = '\0';
            strcpy(copy_buffer, (const char*)write_buffer);
            *cbuf_last = strlen(write_buffer);
            fprintf(decompressed_file, "%s", write_buffer);
            *wbuf_i = 0;
        }
        write_string(word, write_buffer + *wbuf_i, word_length);
        write_string(word, (copy_buffer + *cbuf_last + *wbuf_i), word_length);
        *wbuf_i += word_length;
    }
    else
    {
        *(write_buffer + *wbuf_i) = '\0';
        fprintf(decompressed_file, "%s", write_buffer);
    }
}

void write_character_to_buffer(FILE *decoded_file, char *buffer, int *buf_i, char c) {

    if (*buf_i == MAX_BUFFER_LENGTH) {

        fwrite(buffer, sizeof(char), *buf_i, decoded_file);
        *buf_i = 0;
    }
    *(buffer + *buf_i) = c;
    (*buf_i)++;
}
