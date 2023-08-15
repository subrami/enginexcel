#define NOBUILD_IMPLEMENTATION
#include "./libs/nobuild.h"

#define CSV_PATH "payload.csv"

#define CFLAGS "-Wall", "-Wextra", "-Wswitch-enum", "-std=c11", "-pedantic", "-ggdb"

int main (int argc, char **argv) {
    GO_REBUILD_URSELF(argc, argv);
    CMD("cc", CFLAGS, "-o", "excel-engine", "main.c");


    if (argc > 1) {
        if (strcmp(argv[1], "run") == 0) {
            CMD("./excel-engine", CSV_PATH);
        } else {
            PANIC("%s IS AN UNKNOWN SUBCOMMAND.", argv[1]);
            exit(1);
        }
    }
    return 0;
}