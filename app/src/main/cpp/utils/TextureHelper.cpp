//
// Created by zzr on 2019/3/27.
//

#include "TextureHelper.h"
#include "../common/stb_image.h"
#include <stdlib.h>

unsigned    createTexture(int w, int h, const void* data, GLenum type)
{
    unsigned    texId;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, type, w, h, 0, type, GL_UNSIGNED_BYTE, data);

    return  texId;
}

GLuint TextureHelper::createTextureFromImage(const char *fileName)
{
    int     width;
    int     height;
    int     chanel;
    GLuint  texId;
    stbi_uc*    pixels  =   stbi_load(fileName, &width, &height, &chanel, 0);
    if (chanel == 3)
    {
        texId   =   createTexture(width, height, pixels, GL_RGB);
    }
    else
    {
        texId   =   createTexture(width, height, pixels, GL_RGBA);
    }
    free(pixels);
    return texId;
}
