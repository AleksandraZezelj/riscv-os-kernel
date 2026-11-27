#ifndef CONSOLEMANAGER_HPP_
#define CONSOLEMANAGER_HPP_

#include "semaphoreKernel.hpp"
#include "../h/list.hpp"
#include "../h/tcb.hpp"

class ConsoleManager {
public:
    // Velicina bafera
    static int constexpr BUFFER_SIZE = 256;

    // pravi semafore
    static void init();

    // Citanje iz kruznog bafera
    static char get_c();

    //  Upis u kruzni bafer
    static void put_c(char c);

    // Funkcija koja se poziva u prekidnos rutini i obradjuje prekid od konzole
    static void handleConsoleInterrupt();

    // Obezbedjuje nam da dokle god ima podataka u izlaznom baferu prepustamo procesor drugima
    static void flush();


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

    static SemaphoreKernel* fullBafferSemaphore; // broji slobodna mesta za ispis
    static SemaphoreKernel* inputSemaphore; // broji dostupne karaktere sa tastature

    // Ulazni kruzni bafer (tastatura)
    static char inputBuffer[BUFFER_SIZE];
    static int inputHead;
    static int inputTail;

    // izlazni kruzni bafer (ekran)
    static  char outputBuffer[BUFFER_SIZE];
    static int outputHead;
    static int outputTail;


};



#endif
