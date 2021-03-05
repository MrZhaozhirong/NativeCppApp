//
// Created by zzr on 2019/3/27.
//

#include "TextureHelper.h"
#include "../common/stb_image.h"
#include "../common/zzr_common.h"
#include <stdlib.h>
#include <cstring>
#include <GLES3/gl32.h>
#include <time.h>
#include <unistd.h>

unsigned    createTexture(int w, int h, const void* data, GLenum type)
{
    unsigned    texId;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //usleep(1000000);
    time_t begin_t  = clock();
    glTexImage2D(GL_TEXTURE_2D, 0, type, w, h, 0, type, GL_UNSIGNED_BYTE, data);
    time_t finish_t = clock();
    LOGI("createTextureFromImage glTexImage2D cost : %f ms.", (double )(finish_t - begin_t)/1000 );

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


GLuint TextureHelper::createTextureFromPBO(const char *fileName)
{
    int     width;
    int     height;
    int     chanel;
    GLuint  texId;
    GLuint  pboId;
    int     dataSize;
    GLenum  type;
    //int     error;
    //while ((error = glGetError()) != GL_NO_ERROR) {
    //    LOGE("EGL error: %x", error);
    //}

    stbi_uc*    pixels  =   stbi_load(fileName, &width, &height, &chanel, 0);
    if (chanel == 3)
    {
        type = GL_RGB;
        dataSize = width*height*3;
    }
    else
    {
        type = GL_RGBA;
        dataSize = width*height*4;
    }

    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, NULL);


    glGenBuffers(1, &pboId);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboId);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, dataSize, NULL, GL_STREAM_DRAW);
    //while ((error = glGetError()) != GL_NO_ERROR) {
    //    LOGE("EGL error: %x", error);
    //}
    GLubyte *bufPtr = static_cast<GLubyte *>(glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, dataSize,
                                                              GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT));
    //while ((error = glGetError()) != GL_NO_ERROR) {
    //    LOGE("EGL error: %x", error);
    //}
    if (bufPtr) {
        memcpy(bufPtr, pixels,  static_cast<size_t>(dataSize));
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    }
    free(pixels);


    //usleep(1000000);  // 1.000s = 1000ms = 1000000us

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texId);
    // glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    // glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, width);
    // glPixelStorei(GL_UNPACK_ROW_LENGTH, height);
    time_t begin_t  = clock();
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, type, GL_UNSIGNED_BYTE, 0);
    time_t finish_t = clock();
    LOGI("createTextureFromPBO glTexSubImage2D cost : %f ms.", (double )(finish_t - begin_t)/1000 );
    return texId;
}