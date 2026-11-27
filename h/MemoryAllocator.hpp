#include "../lib/hw.h"

#ifndef MEMORY_ALLOCATOR_H
#define MEMORY_ALLOCATOR_H

// 3* 8 = 24 Bajta za BlockHeader
struct BlockHeader {
    size_t size;
    BlockHeader* next;
    BlockHeader* prev;
};

class MemoryAllocator{
public:

    static BlockHeader* getFreeMemHead(){return freeMemHead;}

    static void initMemoryAllocator();

    static void* malloc(size_t size);

    static void free(void* ptr);

private:
    MemoryAllocator() {};

    static BlockHeader* freeMemHead;

    void static concatenate(BlockHeader* first, BlockHeader* second);
};

#endif



