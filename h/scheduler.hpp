#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include "list.hpp"
#include "MemoryAllocator.hpp"

class TCB;

class Scheduler {
private:
    static List<TCB> readyCoroutineQueue;
public:
    static TCB* get();

    static void put(TCB* ccb);


    void *operator new(uint64 n){
        return MemoryAllocator::malloc(n);
    }

    void *operator new[] (uint64 n){
        return MemoryAllocator::malloc(n);
    }

    void operator delete(void* p) noexcept
    {
        if (p==nullptr) return;
        MemoryAllocator::free(p);
    }

    void operator delete[] (void* p) noexcept
    {
        if (p==nullptr) return;
        MemoryAllocator::free(p);
    }

};



#endif