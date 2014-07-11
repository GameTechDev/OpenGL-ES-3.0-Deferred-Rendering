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

