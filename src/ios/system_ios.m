/* Copyright (c) <2013>, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  - Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  - Neither the name of Intel Corporation nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "system.h"
#import <Foundation/Foundation.h>
#include <stdlib.h>
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
    FILE*   file = NULL;
    NSString* full_path = nil;
    NSString* adjusted_relative_path = [@"/assets/" stringByAppendingString:[NSString stringWithUTF8String:filename]];
    full_path = [[NSBundle mainBundle] pathForResource:adjusted_relative_path ofType:nil];

    file = fopen([full_path UTF8String], "rb");
    assert(file);

    fseek(file, 0, SEEK_END);
    *data_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    *data = calloc(1,*data_size);
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

    NSLog(@"%s", message);
    va_end(args);
}

