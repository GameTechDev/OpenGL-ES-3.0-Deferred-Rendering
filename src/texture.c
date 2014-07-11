#include "texture.h"
#include "system.h"
#include "external/stb_image.h"
#include "gl_include.h"

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
Texture load_texture(const char* filename)
{
    void*   file_data = NULL;
    size_t  file_size = 0;
    uint8_t*    texture_data = NULL;
    int width, height, components;
    GLuint      texture;
    GLenum      format;
    int         result;

    result = load_file_data(filename, &file_data, &file_size);
    if(result != 0)
        system_log("Loading texture failed: %s\n", filename);
    assert(result == 0);

    texture_data = stbi_load_from_memory(file_data, (int)file_size, &width, &height, &components, 0);
    assert(texture_data);

    ASSERT_GL(glGenTextures(1, &texture));
    ASSERT_GL(glBindTexture(GL_TEXTURE_2D, texture));

    ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST));
    ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

    ASSERT_GL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    switch( components ) {
        case 1: {
            // Gray
            format = GL_LUMINANCE;
            break;
        }
        case 2: {
            // Gray and Alpha
            format = GL_LUMINANCE_ALPHA;
            break;
        }
        case 3: {
            // RGB
            format = GL_RGB;
            break;
        }
        case 4: {
            // RGBA
            format = GL_RGBA;
            break;
        }
        default: {
            // Unknown format
            assert(0);
            return 0;
        }
    }

    ASSERT_GL(glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, texture_data));
    ASSERT_GL(glGenerateMipmap(GL_TEXTURE_2D));
    ASSERT_GL(glBindTexture(GL_TEXTURE_2D, 0));

    stbi_image_free(texture_data);
    free_file_data(file_data);

    return texture;
}
void destroy_texture(Texture T)
{
    ASSERT_GL(glDeleteTextures(1, &T));
}
