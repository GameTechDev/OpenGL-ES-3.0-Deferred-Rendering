#include "utility.h"
#include <string.h>

/* Defines
 */

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
const char* get_line_from_buffer(char* line, size_t line_size, const char* buffer)
{
    const char* line_end = line + line_size;

    if(buffer == NULL || line == NULL || line_size == 0)
        return NULL;

    while(line != line_end) {
        char current_char = *buffer;
        switch(current_char) {
            case '\0':
                *line = '\0';
                return NULL;
            case '\n':
                *line = '\0';
                return buffer+1;
            case '\r':
                *line = '\0';
                buffer++;
                if(*buffer == '\n') {
                    return ++buffer;
                }
                return buffer;
            default:
                *line = current_char;
                ++line;
                ++buffer;
                break;
        }
    }

    return buffer;
}
const char* get_extension_from_filename(const char* filename)
{
    const char* end = filename + strlen(filename);
    while(end > filename) {
        char c = *(end - 1);
        if(c == '.')
            return end;
        end--;
    }
    return NULL;
}
void split_filename(char* path, size_t path_size,
                    char* file, size_t file_size,
                    const char* filename)
{
    const char* end = filename + strlen(filename);
    const char* curr = end;

    if(filename == NULL) {
        strncpy(path, "\0", path_size);
        strncpy(file, "\0", file_size);
        return;
    }

    while(curr > filename) {
        char c = *(curr - 1);
        if(c == '/')
            break;
        curr--;
    }
    if(curr == filename) {
        /* No path found */
        strncpy(path, "\0", path_size);
    } else {
        strncpy(path, filename, path_size);
        path[curr-filename] = '\0';
    }
    strncpy(file, curr, end-curr);
}

