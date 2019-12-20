//
// Created by nicky on 2019/4/18.
//

#ifndef NV21_BUFFER_POOL_HPP
#define NV21_BUFFER_POOL_HPP

#include <queue>
#include "../../common/ByteBuffer.hpp"


class NV21BufferPool {

public:
    NV21BufferPool() {
        this->pool_capacity = 1;
        current_pos = 0;
    }
    NV21BufferPool(size_t pool_size) {
        this->pool_capacity = pool_size;
        current_pos = 0;
    }

    ~NV21BufferPool() {
        while(!pool.empty()) {
            ByteBuffer* item = pool.front();
            pool.pop();
            delete item;
        }
        pool_capacity = 0;
        current_pos = 0;
    }


    void        put(ByteBuffer* item)
    {
        if(current_pos >= pool_capacity)
        {
            ByteBuffer* p = pool.front();
            pool.pop();
            delete p;
            current_pos --;
        }
        pool.push(item);
        current_pos ++;
    }
    ByteBuffer* get()
    {
        if(current_pos > 0)
        {
            ByteBuffer* item = pool.front();
            pool.pop();
            current_pos--;
            return item;
        }
        return NULL;
    }
private:
    std::queue<ByteBuffer* > pool;
    size_t pool_capacity;
    size_t current_pos;
};

#endif // NV21_BUFFER_POOL_HPP