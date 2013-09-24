/** @file gl_helper_ios.m
 *  @copyright  Copyright (c) 2013 Intel. All rights reserved.
 */
#include "gl_helper.h"
#import <CoreGraphics/CGImage.h>
#import <uiKit/uikit.h>
#include <stdlib.h>
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>

/* Constants
 */

/* Variables
 */

/* Internal functions
 */

/* External functions
 */

GLuint load_texture(const char* filename)
{
    int width, height;
    void* tex_data;
    GLuint texture;
    CGContextRef spriteContext;

    CGImageRef spriteImage = [UIImage imageNamed:[NSString stringWithUTF8String:filename]].CGImage;
    if (!spriteImage)  {
        NSLog(@"Failed to load image %s", filename);
        exit(1);
    }
    width = CGImageGetWidth(spriteImage);
    height = CGImageGetHeight(spriteImage);
    tex_data = (GLubyte *) calloc(width*height*4, sizeof(GLubyte));
    spriteContext = CGBitmapContextCreate(tex_data, width, height, 8, width*4,CGImageGetColorSpace(spriteImage), (CGBitmapInfo)kCGImageAlphaPremultipliedLast);
    CGContextDrawImage(spriteContext, CGRectMake(0, 0, width, height), spriteImage);
    CGContextRelease(spriteContext);

    glGenTextures(1, &texture);
    CheckGLError();
    glBindTexture(GL_TEXTURE_2D, texture);
    CheckGLError();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
    CheckGLError();
    glGenerateMipmap(GL_TEXTURE_2D);
    CheckGLError();
    glBindTexture(GL_TEXTURE_2D, 0);
    CheckGLError();

    free(tex_data);

    return texture;
}
