/**
 * @brief String functions implementation
 * SPDX-FileCopyrightText: 2026 Stu Hacking <stuhacking@gmail.com>
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "strings.h"


u64 strhash(const char *const str) {
    u64 hash = 57;

    for (u32 k = 0; str[k] != '\0'; ++k) {
        hash = 37 * hash + str[k];
    }

    return hash;
}
