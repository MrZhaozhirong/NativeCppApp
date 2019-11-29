//
// Created by nicky on 2019/4/18.
//

#ifndef BYTE_BUFFER_HPP
#define BYTE_BUFFER_HPP


#include <stdint.h>
#include <malloc.h>
#include <string.h>

class ByteBuffer {

public:
    ByteBuffer()
    {
        pbuffer = (int8_t *) malloc(500 * 500);
        capacity = 500 * 500;
        limit = 0;
        param1 = 0;
        param2 = 0;
        param3 = 0;
    }
    ByteBuffer(size_t size)
    {
        pbuffer = (int8_t *) malloc(size);
        capacity = size;
        limit = 0;
        param1 = 0;
        param2 = 0;
        param3 = 0;
    }
    ~ByteBuffer()
    {
        if(pbuffer!=NULL)
            free(pbuffer);
        capacity = limit = 0;
        param1 = -1;
        param2 = -1;
        param3 = -1;
    }

    int     getCapacity()
    {
        return capacity;
    }
    int     getLimit()
    {
        return limit;
    }
    void    wrap(int8_t * nv21, size_t len)
    {
        if(len > capacity) {
            pbuffer = (int8_t *) realloc(pbuffer, sizeof(int8_t) * len);
            capacity = len;
        }
        memset(pbuffer, 0, capacity);
        memcpy(pbuffer, nv21, sizeof(int8_t)*len);
        limit = len;
    }

    int8_t *    data()
    {
        return pbuffer;
    }

    int     param1;
    int     param2;
    int     param3;
private:
    int8_t * pbuffer;
    size_t capacity;   //总容量
    size_t limit;      //当前数据长度
};


#endif // BYTE_BUFFER_HPP
