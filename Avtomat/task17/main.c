#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <curl/curl.h>
#include <curl/easy.h>

#ifdef _WIN32
    #include <io.h>
#else
    #include <unistd.h>
#endif

size_t write_data(void* ptr, size_t size, size_t count, FILE* stream) {
    return fwrite(ptr, size, count, stream);
}

bool file_exists(const char* path) {
#ifdef _WIN32
    return _access(path, 0) == 0;
#else
    return access(path, F_OK) == 0;
#endif
}

typedef struct {
    char* data;
    size_t size;
    size_t capacity;
} String;

String string_create(const char* initial) {
    size_t len = strlen(initial);

    String str = {0};
    str.data = malloc(len);
    str.size = len;
    str.capacity = len;

    memcpy(str.data, initial, len);

    return str;
}

void string_copy(String* string, const char* initial) {
    size_t len = strlen(initial);

    if (len >= string->capacity) {
        free(string->data);
        string->capacity = len;
        string->data = malloc(string->capacity);;
    }
    
    memcpy(string->data, initial, len);
    string->size = len;
}

void string_add(String* string, int item) {
    if (string->size >= string->capacity) {
        string->capacity *= 2;
        string->data = realloc(string->data, string->capacity * sizeof(*string->data));
    }

    string->data[string->size++] = item;
}

void string_add_str(String* string, const char* str) {
    size_t len = strlen(str);

    if (string->size + len >= string->capacity) {
        string->capacity += len;
        string->data = realloc(string->data, string->capacity * sizeof(*string->data));
    }

    for (size_t i = 0; i < len; ++i) {
        string->data[string->size + i] = str[i];
    }

    string->size += len;
}

bool string_get(String* string, size_t index, int* out_result) {
    if (index >= string->size) return false;

    *out_result = string->data[index];

    return true;
}

bool string_remove(String* string, size_t index) {
    if (index >= string->size) return false;

    for (size_t i = index + 1; i < string->size; ++i) {
        string->data[i - 1] = string->data[i];
    }

    string->size--;

    return true;
}

long long string_find_first(String* string, char ch) {
    long long index = 0;
    bool found = false;
    while (index < string->size) {
        if (string->data[index] == ch) {
            found = true;
            break;
        }
        index++;
    }
    return found ? index : -1;
}

bool string_insert(String* string, size_t index, int item) {
    if (index >= string->size) return false;

    if (string->size >= string->capacity) {
        string->capacity *= 2;
        string->data = realloc(string->data, string->capacity * sizeof(*string->data));
    }
    
    string->size++;

    for (size_t i = string->size; i > index; --i) {
        string->data[i] = string->data[i - 1];
    }

    string->data[index] = item;

    return true;
}

char* string_to_cstr(String* string) {
    char* str = malloc(string->size + 1);
    memcpy(str, string->data, string->size);
    str[string->size] = '\0';
    return str;
}

bool string_insert_str(String* string, size_t index, const char* str) {
    if (index >= string->size) return false;

    size_t len = strlen(str);

    if (string->size + len >= string->capacity) {
        string->capacity += len;
        string->data = realloc(string->data, string->capacity * sizeof(*string->data));
    }

    for (size_t i = string->size + len; i >= index; --i) {
        string->data[i] = string->data[i - len];
    }

    for (size_t i = 0; i < len; ++i) {
        string->data[index + i] = str[i];
    }

    string->size += len;

    return true;
}

void string_free(String* string) {
    free(string->data);
    string->data = NULL;
    string->size = 0;
    string->capacity = 0;
}


char* get_filename_from_url(const char* url) {
    char* ptr = strrchr(url, '/');

    const char* name = NULL;

    if (*(ptr + 1) == '\0' || ptr == NULL) {
        name = "index.html";
    } else {
        name = ++ptr;
    }

    ptr = strrchr(name, '?');
    if (ptr != NULL) {
        *ptr = '\0';
    }

    String result = string_create(name);

    char* str = string_to_cstr(&result);

    size_t i = 1;
    while (file_exists(str)) {
        size_t len = snprintf(NULL, 0, " (%zu)", i);
        char* number = malloc(len + 1);
        snprintf(number, len + 1, " (%zu)", i);

        string_copy(&result, name);

        long long index = string_find_first(&result, '.');

        if (index >= 0) {
            string_insert_str(&result, index, number);
        } else {
            string_add_str(&result, number);
        }

        free(str);
        str = string_to_cstr(&result);

        i++;
    }

    return str;
}

long long getline(char** line, size_t* line_cap, FILE* file) {
    char buffer[128];

    if (*line == NULL || *line_cap == 0) {
        *line_cap = sizeof(buffer);
        *line = malloc(*line_cap);
    }

    size_t line_len = 0;
    while (fgets(buffer, sizeof(buffer), file)) {
        size_t buf_len = strlen(buffer);

        if (line_len + buf_len >= *line_cap) {
            *line_cap *= 2;
            *line = realloc(*line, *line_cap);
        }

        size_t bytes_to_copy = buf_len;
        if (buffer[buf_len - 1] == '\n') {
            bytes_to_copy -= 1;
        }

        memcpy((*line) + line_len, buffer, bytes_to_copy);
        line_len += bytes_to_copy;

        if (buffer[buf_len - 1] == '\n' || line_len > 0) {
            (*line)[line_len] = '\0';
            return line_len;
        }
    }

    return -1;
}

CURLcode download_file(CURL* curl, const char* url) {
    char* filename = get_filename_from_url(url);
    FILE* file = fopen(filename, "wb");

    if (file == NULL) {
        free(file);
        return 0;
    }

    // curl_easy_setopt(curl, CURLOPT_CAINFO, "./cacert.pem");
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10);
    curl_easy_setopt(curl, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NATIVE_CA);

    CURLcode res = curl_easy_perform(curl);

    if (res == CURLE_OK) {
        printf("Downloaded %s -> %s\n", url, filename);
    } else {
        printf("Failed %s: %s\n", url, curl_easy_strerror(res));
    }

    free(filename);
    fclose(file);

    return res;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("You must provide a path to a file containing URLs OR list all the URLs in arguments.\n");
        return 1;
    }

    CURL* curl = curl_easy_init();
    if (curl == NULL) {
        printf("Couldn't initialize curl.\n");
        return 1;
    }

    if (argc == 2) {
        if (strstr(argv[1], "://") != NULL) {
            download_file(curl, argv[1]);
        } else {
            FILE* f = fopen(argv[1], "r");
            if (f == NULL) {
                printf("Couldn't open the file.\n");
                return 1;
            }

            char* line = NULL;
            size_t line_cap = 0;

            while (getline(&line, &line_cap, f) >= 0) {
                download_file(curl, line);
            }

            fclose(f);
        }
    } else {
        for (int i = 1; i < argc; ++i) {
            download_file(curl, argv[i]);
        }
    }

    curl_easy_cleanup(curl);

    return 0;
}