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
