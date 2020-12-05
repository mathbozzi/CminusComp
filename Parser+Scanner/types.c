
#include "types.h"

static const char *TYPE_STRING[] = {
    "int",
    "void",
    "string",
};

const char* get_text(Type type) {
    return TYPE_STRING[type];
}
