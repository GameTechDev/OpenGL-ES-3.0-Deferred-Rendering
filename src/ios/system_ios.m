/*! @file system_ios.m
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#include "system.h"
#import <Foundation/Foundation.h>
#include <stdlib.h>

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
size_t load_file_contents(const char* filename, void* buffer, size_t buffer_size)
{
    NSError* error;
    NSString* filename_string = [[[NSString alloc] initWithUTF8String:filename] stringByDeletingPathExtension];
    NSString* extension_string = [[[NSString alloc] initWithUTF8String:filename] pathExtension];
    NSString* full_path = [[NSBundle mainBundle] pathForResource:filename_string
                                                          ofType:extension_string];
    NSString* file_contents = [NSString stringWithContentsOfFile:full_path
                                                        encoding:NSUTF8StringEncoding
                                                           error:&error];
    size_t total_bytes = 0;

    if(file_contents == nil) {
        NSLog(@"Error reading file %@.%@: %@",
                filename_string,
                extension_string,
                [error localizedDescription]);
        return 0;
    }

    total_bytes = min([file_contents length], buffer_size);
    memcpy(buffer, [file_contents UTF8String], total_bytes);
    return total_bytes;
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
