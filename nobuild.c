#define NOBUILD_IMPLEMENTATION

#define CFLAGS "-Wall", "-Wextra"

int main (int argc, char **argv) {
    GO_REBUILD_URSELF(argc, argv);
    CMD("cc", CFLAGS, "-o", "excel-engine", "main.c");


    if (argc > 1) {
        if (strcmp(argv[1], "run") == 0) {
            CMD("./excel-engine", "payload.csv");
        } else {
            PANIC("%s IS AN UNKNOWN SUBCOMMAND.", argv[1]);
        }
    }
    return 0;
}