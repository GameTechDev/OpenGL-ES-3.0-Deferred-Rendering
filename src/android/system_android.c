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
#include <stdlib.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <stdio.h>

/* Defines
 */
#define min(a,b) ((a) < (b) ? (a) : (b))

/* Types
 */

/* Constants
 */
AAssetManager* _asset_manager = NULL;

/* Variables
 */

/* Internal functions
 */

/* External functions
 */

void free_file_data(void* data)
{
    free(data);
}
int load_file_data(const char* filename, void** data, size_t* data_size)
{
    AAsset* file = AAssetManager_open(_asset_manager, filename, AASSET_MODE_UNKNOWN);
    if(file) {
        off_t file_size = AAsset_getLength(file);
        *data = malloc(file_size);
        *data_size = file_size;
        AAsset_read(file, *data, file_size);
        AAsset_close(file);
    } else {
        return -1;
    }
    return 0;
}
void system_log(const char* format, ...)
{
    va_list args;
    char message[2048] = {0};
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    __android_log_print(ANDROID_LOG_INFO, "DeferredGLES", "%s", message);
    va_end(args);
}
