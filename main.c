#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#define SV_IMPLEMENTATION
#include "./libs/sv.h"

typedef enum {
    CELL_TEXT,
    CELL_NUMBER,
    CELL_FORMULA
} Cell_Type;

typedef union {
    String_View text;
    double number;
} Cell_Union;

typedef enum {
    EXPR_NUMBER = 0,
    EXPR_CELL,
    EXPR_BINARY_OPR,

} Formula_Type;

// typedef formula {
//     Formula_Type type;
// } Formula;

typedef struct {
    Cell_Type type;
} Cell;

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

void calc_table_size(String_View content, size_t *out_rows, size_t *out_cols) {
    // %.*s is a format specifier for a string view.

    size_t cols = 0;
    size_t rows = 0;
    for (; content.count > 0; rows++) {
        
        String_View line = sv_chop_by_delim(&content, '\n');

        size_t col = 0;
        for (; line.count > 0; col++) {
            String_View cell = sv_trim(sv_chop_by_delim(&line, ','));
            // printf("%s: (%zu, %zu) - %.*s\n",
            //  input_file_path,
            //  row, col, SV_Arg(cell));
        }

        if (cols < col) {
            cols = col;
        } 

    }
    if (out_cols) {
        *out_cols = cols;
    }

    if (out_rows) {
        *out_rows = rows;
    }
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

    fwrite(content, 1, content_size, stdout);

    size_t rows, cols;
    calc_table_size(csv, &rows, &cols);
    printf("Table Size: %zux%zu\n", rows, cols);

    Cell *table = malloc(sizeof(Cell) * rows * cols);

    if (table == NULL) {
        fprintf(stderr, "ERROR: memory allocation of table failed.");
    }

    return 0;
}
