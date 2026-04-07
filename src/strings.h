/**
 * @file
 * @brief String functions
 *
 * SPDX-FileCopyrightText: 2026 Stu Hacking <stuhacking@gmail.com>
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef STRINGS_H
#define STRINGS_H

#include "types.h"

/** Simple hashing function for C string. */
u64 strhash(const char *str);

#endif /* STRINGS_H */
