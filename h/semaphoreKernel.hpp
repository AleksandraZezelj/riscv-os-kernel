#ifndef SEMAPHORE_KERNEL_HPP_
#define SEMAPHORE_KERNEL_HPP_

#include "list.hpp"
#include "../h/MemoryAllocator.hpp"

class TCB;

class SemaphoreKernel {
public:
    ~SemaphoreKernel(){};

    static SemaphoreKernel* semOpen(unsigned init = 1);

    int semClose();

    int wait();
    int signal();

    int waitN(int n);
    int signalN(int n);


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

    SemaphoreKernel(unsigned init = 1): val((int)init)  {};
    int val;
    List<TCB> blocked;
};




#endif
