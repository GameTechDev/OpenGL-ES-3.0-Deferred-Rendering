/*! @file system.h
 *  @brief Interfacing with the OS
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#ifndef __system_h__
#define __system_h__

#include <stddef.h>

/** @return Bytes read from file. 0 if failure
 */
size_t load_file_contents(const char* filename, void* buffer, size_t buffer_size);
void system_log(const char* format, ...);

#endif /* include guard */
