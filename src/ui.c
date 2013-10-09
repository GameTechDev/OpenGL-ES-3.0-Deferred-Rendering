/*! @file ui.c
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#include "ui.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "system.h"
#include "Graphics.h"
#include "gl_include.h"
#include "program.h"

/* Defines
 */
enum {
    kBMFontInfoBlock = 1,
    kBMFontCommonBlock = 2,
    kBMFontPagesBlock = 3,
    kBMFontCharsBlock = 4,
    kBMFontKerningBlock = 5
};

/* Types
 */

#pragma pack(push,1)
typedef struct {
    uint8_t     type;
    uint32_t    size;
} bmfont_block_type_t;

typedef struct {
    int16_t     fontSize;
    uint8_t     bitField; /* bit 0: smooth, bit 1: unicode, bit 2: italic, bit 3: bold, bit 4: fixedHeigth, bits 5-7: reserved */
    uint8_t     charSet;
    uint16_t    stretchH;
    uint8_t     aa;
    uint8_t     paddingUp;
    uint8_t     paddingRight;
    uint8_t     paddingDown;
    uint8_t     paddingLeft;
    uint8_t     spacingHoriz;
    uint8_t     spacingVert;
    uint8_t     outline;
    char        name[128];
} bmfont_info_t;

typedef struct {
    uint16_t    lineHeight;
    uint16_t    base;
    uint16_t    scaleW;
    uint16_t    scaleH;
    uint16_t    pages;
    uint8_t     bitField;
    uint8_t     alphaChnl;
    uint8_t     redChnl;
    uint8_t     greenChnl;
    uint8_t     blueChnl;
} bmfont_common_t;
typedef struct {
    uint8_t id;
    char    pageName[128];
} bmfont_page_t;
typedef struct {
    uint32_t    id;
    uint16_t    x;
    uint16_t    y;
    uint16_t    width;
    uint16_t    height;
    int16_t     xoffset;
    int16_t     yoffset;
    int16_t     xadvance;
    uint8_t     page;
    uint8_t     chnl;
} bmfont_char_t;

typedef struct {
    uint32_t    first;
    uint32_t    second;
    int16_t     amount;
} bmfont_kerning_pairs_t;
#pragma pack(pop)

typedef struct FontData
{
    bmfont_info_t   info;
    bmfont_common_t common;
    bmfont_page_t   pages[16];
    bmfont_char_t   chars[256];
    bmfont_kerning_pairs_t  kerning[256];
} FontData;

typedef struct Font {
    FontData    data;
    GLuint      textures[16];
    GLuint      char_vertices[256];
    GLuint      char_indices;
} Font;

struct UI
{
    Mat4    proj_matrix;
    Graphics*   G;
    int     width;
    int     height;

    GLuint  program;
    GLuint  u_ViewProjection;
    GLuint  u_World;
    GLuint  u_Color;
    GLuint  s_Texture;

    Font    font;
};

/* Constants
 */
static const uint16_t kQuadIndices[] =
{
    0, 1, 2,
    2, 3, 0,
};

/* Variables
 */

/* Internal functions
 */
void* mread(void* dest, size_t size, size_t count, const void* src)
{
    memcpy(dest, src, size*count);
    return ((char*)src) + size*count;
}
FontData _load_font(const char* filename)
{
    FontData    font = {0};
    void*   file_data = NULL;
    void*   read = NULL;
    size_t  file_size = 0;
    char    header[4];

    load_file_data(filename, &file_data, &file_size);
    read = file_data;

    read = mread(header, 1, 4, read);

    if(header[0] != 'B' || header[1] != 'M' || header[2] != 'F' || header[3] != 3) {
        system_log("Error loading font %s: Not proper font format", filename);
        return font;
    }

    do {
        bmfont_block_type_t type;
        read = mread(&type, sizeof(type), 1, read);
        switch(type.type) {
        case kBMFontInfoBlock: {
                read = mread(&font.info, type.size, 1, read);
                break;
            }
        case kBMFontCommonBlock: {
                read = mread(&font.common, type.size, 1, read);
                break;
            }
        case kBMFontPagesBlock: {
                char pagenames[1024] = {0};
                const char* curr_pagename = pagenames;
                int ii=0;
                read = mread(&pagenames, type.size, 1, read);
                while(strlen(curr_pagename))
                {
                    strlcpy(font.pages[ii].pageName, curr_pagename, sizeof(font.pages[ii].pageName));
                    font.pages[ii].id = (uint8_t)ii;
                    while(*curr_pagename != '\0')
                        ++curr_pagename;
                    ++curr_pagename;
                }
                break;
            }
        case kBMFontCharsBlock: {
                int ii;
                int num_chars = type.size/sizeof(bmfont_char_t);
                for(ii=0; ii<num_chars; ++ii) {
                    bmfont_char_t character;
                    read = mread(&character, sizeof(character), 1, read);
                    font.chars[character.id] = character;
                }
                break;
            }
        case kBMFontKerningBlock: {
                int ii;
                int num_chars = type.size/sizeof(bmfont_kerning_pairs_t);
                for(ii=0; ii<num_chars; ++ii) {
                    read = mread(&font.kerning[ii], sizeof(bmfont_kerning_pairs_t), 1, read);
                }
            break;
            }
        }
    } while((char*)read < (char*)file_data + file_size);

    free_file_data(file_data);

    return font;
}

/* External functions
 */
UI* create_ui(Graphics* G)
{
    AttributeSlot slots[] = {
        kPositionSlot,
        kTexCoordSlot,
        kEmptySlot
    };
    int ii = 0;
    UI* U = (UI*)calloc(1, sizeof(UI));

    U->G = G;
    U->font.data = _load_font("inconsolata.fnt");

    /* Load textures */
    for(ii=0;ii<16;++ii) {
        if(U->font.data.pages[ii].pageName[0] == '\0')
            break;
        U->font.textures[ii] = load_texture(U->font.data.pages[ii].pageName);
    }

    /* Create character index buffer */
    ASSERT_GL(glGenBuffers(1, &U->font.char_indices));
    ASSERT_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, U->font.char_indices));
    ASSERT_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kQuadIndices), kQuadIndices, GL_STATIC_DRAW));
    ASSERT_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    /* Create character meshes */
    for(ii=0;ii<256;++ii) {
    int jj;
    Vec2 scale = { U->font.data.common.scaleW, U->font.data.common.scaleW };
        bmfont_char_t c = U->font.data.chars[ii];
        struct {
            Vec3    pos;
            Vec2    tex;
        } quad_vertices[] =
        {
            0.0f,    c.height, 0.0f,     c.x,         c.y,          // TL
            c.width, c.height, 0.0f,     c.x+c.width, c.y,          // TR
            c.width, 0.0f,     0.0f,     c.x+c.width, c.y+c.height, // BR
            0.0f,    0.0f,     0.0f,     c.x,         c.y+c.height, // BL
        };
        if(c.id == 0)
            continue;
        for (jj=0; jj<4; ++jj) {
  quad_vertices[jj].tex = vec2_div(quad_vertices[jj].tex, scale);
}
        ASSERT_GL(glGenBuffers(1, &U->font.char_vertices[ii]));
        ASSERT_GL(glBindBuffer(GL_ARRAY_BUFFER, U->font.char_vertices[ii]));
        ASSERT_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW));
        ASSERT_GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }

    /* Create shader */
    U->program = create_program("shaders/ui/vertex.glsl",
                            "shaders/ui/fragment.glsl",
                             slots);

    ASSERT_GL(U->u_ViewProjection = glGetUniformLocation(U->program, "u_ViewProjection"));
    ASSERT_GL(U->u_World = glGetUniformLocation(U->program, "u_World"));
    ASSERT_GL(U->u_Color = glGetUniformLocation(U->program, "u_Color"));
    ASSERT_GL(U->s_Texture = glGetUniformLocation(U->program, "s_Texture"));

    return U;
}
void destroy_ui(UI* U)
{
    free(U);
}

void resize_ui(UI* U, int width, int height)
{
    U->width = width, U->height = height;
    U->proj_matrix = mat4_ortho(width, height, 0.0f, 1.0f);
}

void draw_string(UI* U, const char* string)
{
    Vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
    float x = -U->width/2.0f;
    float y = 0.0f;
    ASSERT_GL(glDepthMask(GL_FALSE));
    ASSERT_GL(glDepthFunc(GL_ALWAYS));
    ASSERT_GL(glEnable(GL_BLEND));
    ASSERT_GL(glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA));
    ASSERT_GL(glUseProgram(U->program));
    ASSERT_GL(glUniformMatrix4fv(U->u_ViewProjection, 1, GL_FALSE, (float*)&U->proj_matrix));
    ASSERT_GL(glUniform4fv(U->u_Color, 1, (float*)&color));
    ASSERT_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, U->font.char_indices));
    ASSERT_GL(glActiveTexture(GL_TEXTURE0));
    while(string && *string) {
        float* ptr = 0;
        char c = *string;
        bmfont_char_t glyph = U->font.data.chars[c];

        if(c != ' ') {
            Mat4 world = mat4_translatef(x, y, 0.0f);
            ASSERT_GL(glUniformMatrix4fv(U->u_World, 1, GL_FALSE, (float*)&world));
            ASSERT_GL(glBindTexture(GL_TEXTURE_2D, U->font.textures[glyph.page]));
            ASSERT_GL(glBindBuffer(GL_ARRAY_BUFFER, U->font.char_vertices[c]));
            ASSERT_GL(glVertexAttribPointer(kPositionSlot,    3, GL_FLOAT, GL_FALSE, sizeof(float)*5, (void*)(ptr+=0)));
            ASSERT_GL(glVertexAttribPointer(kTexCoordSlot,    2, GL_FLOAT, GL_FALSE, sizeof(float)*5, (void*)(ptr+=3)));
            ASSERT_GL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL));
        }
        x += glyph.xadvance;
        ++string;
    }
    ASSERT_GL(glDepthMask(GL_TRUE));
    ASSERT_GL(glDepthFunc(GL_LESS));
    ASSERT_GL(glDisable(GL_BLEND));
}
