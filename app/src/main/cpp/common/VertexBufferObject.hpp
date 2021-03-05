//
// Created by nicky on 2021/3/3.
//

#ifndef VERTEX_BUFFER_OBJECT_HPP
#define VERTEX_BUFFER_OBJECT_HPP

#include <GLES3/gl3.h>
#include "CELLMath.hpp"

class VertexBufferObject {
private:
    GLuint bufferId = -1;

public:
    void bindData(void* data, size_t size) {
        glGenBuffers(1, &bufferId);
        glBindBuffer(GL_ARRAY_BUFFER, bufferId);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), data, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }

    int getVertexBufferID() {
        return bufferId;
    }

};

#endif // VERTEX_BUFFER_OBJECT_HPP
