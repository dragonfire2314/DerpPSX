#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

#define printx(text, ...) \
    printf(text, __VA_ARGS__); \
    putchar('\n'); \
    exit(0)

#define printd(text, ...) \
    printf(text, __VA_ARGS__); \
    putchar('\n')

// Data
typedef uint64_t ud; // uns doubleword
typedef uint32_t uw; // uns word
typedef uint16_t uh; // uns halfword
typedef uint8_t  ub; // uns bite

typedef int64_t sd; // doubleword
typedef int32_t sw; // word
typedef int16_t sh; // halfword
typedef int8_t  sb; // bite