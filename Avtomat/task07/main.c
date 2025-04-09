#include <corecrt_wctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <wchar.h>
#include <locale.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#define CHAR_T wchar_t

typedef struct {
    CHAR_T* word;
    size_t count;
} Word;

long long getline(CHAR_T** line, size_t* line_cap, FILE* file) {
    CHAR_T buffer[128];

    if (*line == NULL || *line_cap == 0) {
        *line_cap = 128;
        *line = malloc(*line_cap * sizeof(CHAR_T));
    }

    size_t line_len = 0;
    while (fgetws(buffer, 128, file)) {
        size_t buf_len = wcslen(buffer);

        if (line_len + buf_len >= *line_cap) {
            *line_cap *= 2;
            *line = realloc(*line, (*line_cap) * sizeof(CHAR_T));
        }

        size_t bytes_to_copy = buf_len;
        if (buffer[buf_len - 1] == L'\n') {
            bytes_to_copy -= 1;
        }

        memcpy((*line) + line_len, buffer, bytes_to_copy * sizeof(CHAR_T));
        line_len += bytes_to_copy;

        if (buffer[buf_len - 1] == L'\n') {
            (*line)[line_len] = L'\0';
            return line_len;
        }
    }

    return -1;
}

void lowercase(CHAR_T* str) {
    size_t len = wcslen(str);
    for (size_t i = 0; i < len; ++i) {
        str[i] = towlower(str[i]);
    }
}

void sort_words(Word* words, size_t count) {
    for (size_t j = 1; j < count; ++j) {
        for (size_t i = 0; i < count - j; ++i) {
            Word* w1 = &words[i];
            Word* w2 = &words[i + 1];

            if (wcscmp(w1->word, w2->word) > 0) {
                Word tmp = *w1;
                words[i] = *w2;
                words[i + 1] = tmp;
            }
        }
    }
}

#define DELIMETERS L" ,.:;()[]{}!?"

int main(int argc, char** argv) {
    setlocale(LC_ALL, "ru_RU.UTF-8");

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    if (argc < 2) {
        printf("You didn't provide a path to a file.\n");
        return 1;
    }

    FILE* f = fopen(argv[1], "r");
    if (f == NULL) {
        printf("Couldn't open the file.\n");
        return 1;
    }

    size_t words_cap = 10;
    size_t word_count = 0;
    Word* words = malloc(words_cap * sizeof(Word));

    CHAR_T* line = NULL;
    size_t line_cap = 0;

    CHAR_T* next_token = NULL;

    while (getline(&line, &line_cap, f) >= 0) {
        CHAR_T* word = wcstok(line, DELIMETERS, &next_token);
        while (word != NULL) {
            lowercase(word);

            if (word_count >= words_cap) {
                words_cap *= 2;
                words = realloc(words, words_cap * sizeof(Word));
            }

            bool found = false;
            for (size_t i = 0; i < word_count; ++i) {
                Word* w = &words[i];
                if (wcscmp(w->word, word) == 0) {
                    w->count++;
                    found = true;
                    break;
                }
            }

            if (!found) {
                size_t wsize = wcslen(word) + 1;
                CHAR_T* p = malloc(wsize * sizeof(CHAR_T));
                memcpy(p, word, wsize * sizeof(CHAR_T));

                words[word_count++] = (Word) {
                    .word = p,
                    .count = 1
                };
            }

            word = wcstok(NULL, DELIMETERS, &next_token);
        }
    }

    fclose(f);

    free(line);

    sort_words(words, word_count);

    for (size_t i = 0; i < word_count; ++i) {
        Word* word = &words[i];

        wprintf(L"%s : %zu\n", word->word, word->count);

        free(word->word);
    }

    free(words);

    return 0;
}