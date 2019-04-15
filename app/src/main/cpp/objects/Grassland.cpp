//
// Created by nicky on 2019/4/10.
//

#include <memory.h>
#include <GLES2/gl2.h>
#include "Grassland.h"
#include "../common/CELLMath.hpp"

Grassland::Grassland() {
    modelMatrix = new float[16];
    CELL::Matrix::setIdentityM(modelMatrix, 0);

    float   gSizeX = 5;
    float   gSizeZ = 5;
    float   gPos = -1;
    float   rept = 5;

    GrasslandVertex grounds[] =
    {
        {-gSizeX, gPos,-gSizeZ, 0,1,0, 0.0f, 0.0f}, // left far
        { gSizeX, gPos,-gSizeZ, 0,1,0, rept, 0.0f}, // right far
        { gSizeX, gPos, gSizeZ, 0,1,0, rept, rept}, // right near

        {-gSizeX, gPos,-gSizeZ, 0,1,0, 0.0f, 0.0f}, // left far
        { gSizeX, gPos, gSizeZ, 0,1,0, rept, rept}, // right near
        {-gSizeX, gPos, gSizeZ, 0,1,0, 0.0f, rept}, // left near
    };

    VERTEX_DATA = new GrasslandVertex[6];
    memset(VERTEX_DATA, 0, sizeof(VERTEX_DATA));
    memcpy(VERTEX_DATA, grounds, sizeof(grounds));


    matModel.identify();
}

Grassland::~Grassland() {
    delete [] VERTEX_DATA;
    delete [] modelMatrix;
}

void Grassland::bindData(GrasslandProgram *shaderProgram) {
    glVertexAttribPointer(static_cast<GLuint>(shaderProgram->aPositionLocation),
                          3, GL_FLOAT,
                          GL_FALSE, sizeof(GrasslandVertex),
                          &VERTEX_DATA[0].x);
    glEnableVertexAttribArray(static_cast<GLuint>(shaderProgram->aPositionLocation));

    glVertexAttribPointer(static_cast<GLuint>(shaderProgram->aNormalLocation),
                          3, GL_FLOAT,
                          GL_FALSE, sizeof(GrasslandVertex),
                          &VERTEX_DATA[0].nx);
    glEnableVertexAttribArray(static_cast<GLuint>(shaderProgram->aNormalLocation));

    glVertexAttribPointer(static_cast<GLuint>(shaderProgram->aTexUvLocation),
                          2, GL_FLOAT,
                          GL_FALSE, sizeof(GrasslandVertex),
                          &VERTEX_DATA[0].u);
    glEnableVertexAttribArray(static_cast<GLuint>(shaderProgram->aTexUvLocation));

}

void Grassland::unbind(GrasslandProgram *shaderProgram) {
    glDisableVertexAttribArray(static_cast<GLuint>(shaderProgram->aPositionLocation));
    glDisableVertexAttribArray(static_cast<GLuint>(shaderProgram->aNormalLocation));
    glDisableVertexAttribArray(static_cast<GLuint>(shaderProgram->aTexUvLocation));
}

void Grassland::draw() {
    // 1个面，两个三角形，每个三角形三个点
    glDrawArrays(GL_TRIANGLES, 0, 2*3);
}




