#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#define SV_IMPLEMENTATION
#include "./libs/sv.h"


int errorCode = 0;

void usage(FILE *stream) {
    fprintf(stream, "Usage: ./excelgine <subcommand> [options]\n");
}

char *dump_file(const char *file_path, size_t *file_size) {
    char *buffer = NULL;


    FILE *f = fopen(file_path, "rb");

    if (f == NULL) {
        errorCode = 1;
        goto error;
    }
    
    if (fseek(f, 0, SEEK_END) < 0) {
        errorCode = 2;
        goto error;
    }

    if (ftell(f) < 0) {
        errorCode = 2;
        goto error;
    }
    
    long count = ftell(f);

    buffer = malloc(sizeof(char) * count);

    if (buffer == NULL) {
        errorCode = 3;
        goto error;
    }

    // setting pointer back at start of file.
    if (fseek(f, 0, SEEK_SET) < 0) {
        errorCode = 2;
        goto error;
    };

    // ensure that the file is read correctly.
    size_t n = fread(buffer, sizeof(char), count, f);

    if (n != (size_t) count) {
        fprintf(stderr, "ERROR: Fread length not returning expected size. It should be same size as fread.\n");
        exit(1);
    }

    if (n != count) {
        errorCode = 4;
        goto error;
    }

    if (ferror(f)) {
        errorCode = 4;
        goto error;
    }

    if (file_size) {
        *file_size = n;
    }

    fclose(f);

    return buffer;

error:
    if (f) {
        fclose(f);
    }
    if (buffer) {
        free(buffer);
    }

    switch(errorCode) {
        case 1:
            perror("ERROR: Failed to open the file.");
            break;
        case 2:
            perror("ERROR: Failed to seek the file.");
            break;
        case 3:
            perror("ERROR: Buffer returned null.");
            break;
        case 4:
            perror("ERROR: Failed to read the file.");
            break;
    }
    return NULL;
}

int main (int argc, char **argv) {
    printf("Running excel-engine..\n");

    if (argv < 2) {
        usage(stderr);
        fprintf(stderr, "ERROR: No subcommand given.\n");
        exit(1);
    }

    const char *input_file_path = argv[1];

    size_t content_size = 0;
    char *content = dump_file(input_file_path, &content_size);

    if (content == NULL) {
        fprintf(stderr, "ERROR: could not read file %s: %s\n", input_file_path, strerror(errno));
        exit(1);
    }

    // non-owning reference for memory.
    String_View csv = {
        .count = content_size,
        .data = content,
    };

    // %.*s is a format specifier for a string view.
    for (size_t row = 0; csv.count > 0; row++) {
        String_View line = sv_chop_by_delim(&csv, '\n');
        const char *start = line.data;
        for (size_t col = 0; line.count > 0; col++) {
            String_View cell = sv_trim(sv_chop_by_delim(&line, ','));
            printf("%s: (%zu, %zu) - %.*s\n",
             input_file_path,
             row, cell.data - start, SV_Arg(cell));
        }
    }
    fwrite(content, 1, content_size, stdout);
    return 0;
}
