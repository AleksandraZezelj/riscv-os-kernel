#ifndef SLEEPMGR_HPP_
#define SLEEPMGR_HPP_

#include "list.hpp"
#include "tcb.hpp"

class SleepManager {
public:

    static void sleep(time_t timeSlice);

    static void update();


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

private:
    static List<TCB> sleepList;
};


#endif