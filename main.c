#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

int errorCode = 0;

void usage(FILE *stream) {
    fprintf(stream, "Usage: ./excelgine <subcommand> [options]\n");
}

char *dump_file(const char *file_path, size_t *file_size) {
    FILE *f = fopen(file_path, "rb");
    if (f == NULL) {
        errorCode = 1;
        goto error;
    }
    (void) file_path;
    (void) file_size;
    assert(0 && "not implemented");
    return 0;

error:
    if (f) {
        fclose(f);
    }

    switch(errorCode) {
        case 1:

            perror("ERROR: Failed to open the file.");
            break;
    }
    return NULL;
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
    char *content = dump_file(input_file_path, content_size);

    if (content == NULL) {
        fprintf(stderr, "ERROR: could not read file %s: %s\n", input_file_path, strerror(errno));
        exit(1);
    }

    return 0;
}



    