#include "utilities.h"
#include <stdlib.h>

void bail_with_error(const char* message) {
    fprintf(stderr, "Error: %s\n", message);
    exit(1);
}