////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License.  You may obtain a copy
// of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
// License for the specific language governing permissions and limitations
// under the License.
////////////////////////////////////////////////////////////////////////////////

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

