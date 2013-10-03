/*! @file system_ios.m
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#include "../system.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "assert.h"

/* Defines
 */
#define min(a,b) ((a) < (b) ? (a) : (b))

/* Types
 */

/* Constants
 */

/* Variables
 */

/* Internal functions
 */

/* External functions
 */
int load_file_data(const char* filename, void** data, size_t* data_size)
{
    FILE*   file = fopen(filename, "rb");
    assert(file);

    fseek(file, 0, SEEK_END);
    *data_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    *data = malloc(*data_size);
    assert(*data);

    fread(*data, *data_size, 1, file);
    assert(ferror(file) == 0);
    fclose(file);

    return 0;
}
void free_file_data(void* data)
{
    free(data);
}
void system_log(const char* format, ...)
{
    va_list args;
    char message[1024] = {0};
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    printf("%s", message);
    va_end(args);
}

