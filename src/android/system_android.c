/*! @file system_android.c
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
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
size_t load_file_contents(const char* filename, void* buffer, size_t buffer_size)
{
    AAsset* file = AAssetManager_open(_asset_manager, filename, AASSET_MODE_UNKNOWN);
    if(file) {
        off_t file_size = AAsset_getLength(file);
        buffer_size = min(file_size, buffer_size);
        AAsset_read(file, buffer, buffer_size);
        AAsset_close(file);
        system_log("Loaded %d bytes\n", buffer_size);
    } else {
        buffer_size = 0;
    }
    return buffer_size;
}
void system_log(const char* format, ...)
{
    va_list args;
    char message[2048] = {0};
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    __android_log_print(ANDROID_LOG_INFO, "HelloGL", "%s", message);
    va_end(args);
}
