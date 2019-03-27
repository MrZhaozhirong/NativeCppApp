//
// Created by zzr on 2019/3/27.
//

#ifndef NATIVECPPAPP_TEXTUREHELPER_H
#define NATIVECPPAPP_TEXTUREHELPER_H

#include <GLES2/gl2.h>

class TextureHelper {

public:
    static GLuint createTextureFromImage(const char* fileName);


};


#endif //NATIVECPPAPP_TEXTUREHELPER_H
