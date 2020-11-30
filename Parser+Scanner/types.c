
#include "types.h"

static const char *TYPE_STRING[] = {
    "int",
    "void",
};

const char* get_text(Type type) {
    return TYPE_STRING[type];
}
