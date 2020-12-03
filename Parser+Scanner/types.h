
#ifndef TYPES_H
#define TYPES_H

typedef enum {
    INT_TYPE,
    VOID_TYPE,
    NO_TYPE,
} Type;

const char* get_text(Type type);

#endif // TYPES_H

