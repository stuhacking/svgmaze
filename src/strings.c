/** @brief String functions implementation */
#include "strings.h"


u64 strhash(const char *const str) {
    u64 hash = 57;

    for (u32 k = 0; str[k] != '\0'; ++k) {
        hash = 37 * hash + str[k];
    }

    return hash;
}
