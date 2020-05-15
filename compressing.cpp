#include "compressing.h"

int* get_prefix_function(const char *string) {

    int length = strlen(string);
    int current, k;

    int *prefix_f = (int*)malloc(length * sizeof(int));

    prefix_f[0] = 0;

    for (current = 1; current < length; current++) {

        k = prefix_f[current - 1];

        while (k > 0 && string[k] != string[current]) {

            k = prefix_f[k - 1];
        }
        if (string[k] == string[current]) {

            prefix_f[current] = k + 1;
        }
        else
            prefix_f[current] = k;
    }
    return prefix_f;
}

int find_max_element(const int *array, int size, int start) {

    int max = -1, index = -1, i;

    for (i = start; i < size; i++) {

        if (array[i] >= max) {

            max = array[i];
            index = i;
        }
    }
    return index;
}

Code* get_next_code(const char *buffer, int begin, int buf_i, int word_length) {

    int index, win_length = buf_i - begin, length = win_length + word_length + 2;
    int *prefix_func;

    char *modified_str = (char*)malloc(length * sizeof(char));
    Code *code = NULL;

    if (begin < buf_i && word_length > 0) {

        write_string((const char*)(buffer + buf_i), modified_str, word_length);
        modified_str[word_length] = SENTINEL;
        write_string((const char*)(buffer + begin), modified_str + word_length + 1, win_length);
        modified_str[word_length + win_length + 1] = '\0';
        prefix_func = get_prefix_function((const char*)modified_str);

        index = find_max_element((const int*)prefix_func, length - 1, word_length + 1);

        if (index != -1) {

            code = (Code*)malloc(sizeof(Code));

            code->offset = win_length - (index - word_length - prefix_func[index]);
            code->length = prefix_func[index];
        }
        free(prefix_func);
    }

    free(modified_str);

    return code;
}

char* code_to_string(Code *code) {

    char* number = (char*)malloc(MAX_NUMBER_LENGTH * sizeof(char));
    char* code_str = NULL;

    int i = 0, length;

    if (code != NULL && code->length != 0) {

        code_str = (char*)malloc(MAX_CODE_LENGTH * sizeof(char));
        code_str[i] = PREFIX;
        i++;

        number = _itoa(code->offset, number, 10);
        length = strlen(number);
        write_string(number, code_str + i, length);
        i += length;
        code_str[i] = DELIMITER;
        i++;

        number = _itoa(code->length, number, 10);
        length = strlen(number);
        write_string(number, code_str + i, length);
        i += length;

        code_str[i] = SUFFIX;
        code_str[i + 1] = '\0';
    }
    free(number);

    return code_str;
}

char* get_wrapped_string(const char *string, int length) {

    char *wrapped_string = (char*)malloc((length + 3) * sizeof(char));

    wrapped_string[0] = PREFIX;
    write_string(string, wrapped_string + 1, length);
    wrapped_string[length + 1] = SUFFIX;
    wrapped_string[length + 2] = '\0';

    return wrapped_string;
}

bool check_string(const char* string, int length) {

    while (length > 0) {

        if (string[length - 1] == PREFIX) {

            return false;
        }
        length--;
    }
    return true;
}

void compress_file(FILE *source_file, FILE *compressed_file) {

    int rbuf_i, wbuf_i = 0, begin = 0, end = 0, word_length = 0, offset_length = 0;

    char *read_buffer = (char*)malloc(MAX_BUFFER_LENGTH * sizeof(char));
    char *write_buffer = (char*)malloc(MAX_BUFFER_LENGTH * sizeof(char));
    char *word = NULL;

    Code *code = NULL;

    while (!feof(source_file)) {

        rbuf_i = -1;

        while (read_buffer_safe(source_file, read_buffer, &rbuf_i)) {

            code = get_next_code((const char*)read_buffer, begin, rbuf_i, end - rbuf_i);

            if (code != NULL && code->length >= MIN_MATCH_LENGTH) {

                word = read_buffer + rbuf_i - code->offset;
                word_length = code->length;
                offset_length = code->length;

                if (!check_string((const char*)word, word_length)) {

                    word = get_wrapped_string((const char*)word, word_length);
                    word_length += 2;
                }
                else {

                    word = code_to_string(code);
                    word_length = strlen(word);
                }
            }
            else {

                word = read_buffer + rbuf_i;
                word_length = 1;
                offset_length = 1;

                if (!check_string((const char*)word, word_length)) {

                    word = get_wrapped_string((const char*)word, word_length);
                    word_length += 2;
                }
            }

            write_buffer_safe(compressed_file, write_buffer, &wbuf_i, (const char*)word, word_length);

            set_pointers((const char*)read_buffer, &begin, &rbuf_i, &end, offset_length);
        }
    }
    write_buffer_safe(compressed_file, write_buffer, &wbuf_i, NULL, 0);

    free(read_buffer);
    free(write_buffer);
}

