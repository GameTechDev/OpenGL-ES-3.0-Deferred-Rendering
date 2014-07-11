#ifndef __system_h__
#define __system_h__

#include <stddef.h>

/** @return 0 on success, -1 on failure
 */
int load_file_data(const char* filename, void** data, size_t* data_size);
void free_file_data(void* data);
/** Prints a message to the systems log
 */
void system_log(const char* format, ...);

#endif /* include guard */
