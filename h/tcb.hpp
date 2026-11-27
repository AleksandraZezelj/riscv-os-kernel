#ifndef CCB_HPP_
#define CCB_HPP_

#include "riscv.hpp"
#include "scheduler.hpp"
#include "../lib/hw.h"
#include "../h/syscall_cpp.hpp"
#include "../h/MemoryAllocator.hpp"

class Thread;

// Thread Control Block
class TCB {
public:

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

    ~TCB(){ delete[] stack;}

    bool isSleeping() const {return sleeping;}

    void setSleeping(bool value){sleeping = value;}

    int getSleepingCount() const {return sleepingCount;}

    void setSleepingCount(int value){sleepingCount = value;}

    bool isFinished() const {return finished;};

    bool isBlocked() const {return blocked;};

    void setFinished(bool finished){ TCB::finished = finished;};

    void setBlocked(bool blocked) {TCB::blocked = blocked;};

    uint64 getTimeSlice() const {return timeSlice;}

    // fja koja prima void* argument i nema povratnu vrednost
    using Body = void (*)(void*);

    static TCB* createThread(Body body, void* arg, Thread* myThread = nullptr);

    static void initMainThread();

    static void yield();

    static void dispatch();

    void setSemError(bool value) {semError = value;}

    bool getSemError() {return semError;};

    // Fja za pokretanje idle niti
    static void initIdleThread();

    // pokazivac na Idle nit
    static TCB* idleThread;

    // pok na trenutno izvrsavanu korutinu
    static TCB* running;

    int waiting;

private:

    // Konstruktor samo za glavnu (main) nit (specijalna jer vec ima svoj stek na pocetku)
    TCB(uint64 timeSlice) :
    waiting(0),
    body(nullptr),
    arg(nullptr),
    myThread(nullptr),
    stack(nullptr), // svaka nit (osim glavne/ main) ima svoj stek
    timeSlice(timeSlice),
    finished(false),
    blocked(false),
    semError(false),
    sleeping(false),
    sleepingCount(0)
    {
        // Inicijalizacija konteksta
        context.ra = 0;
        context.sp = 0;

        running = this;
    }

    TCB(Body body, void* arg, Thread* myThread, uint64 timeSlice) :
    waiting(0),
    body(body),
    arg(arg),
    myThread(myThread),
    stack(new uint64[STACK_SIZE]), // svaka nit (osim glavne/ main) ima svoj stek
    timeSlice(timeSlice),
    finished(false),
    blocked(false),
    semError(false),
    sleeping(false),
    sleepingCount(0)
    {
        // Inicijalizacija konteksta
        context.ra = (uint64)&threadWrapper;
        context.sp = (uint64)&stack[STACK_SIZE];

    }

    struct Context {
        uint64 ra; //cuvamo pc gde se vracamo
        uint64 sp; //dokle na steku je korutina stavila podatke

    };
    Body body;
    void* arg;
    Thread* myThread; // Pamti roditeljski Thread za potrebe .run()
    uint64* stack;
    Context context;
    uint64 timeSlice;
    volatile bool finished;
    bool blocked;
    bool semError;
    bool sleeping;
    int sleepingCount;

    friend class Riscv;

    // prva se izvrsava za svaku novonapravljenu nit i ono poziva body
    static void threadWrapper();

    static void contextSwitch(Context* oldContext, Context* runningContext);

    static uint64 timeSlicecCounter;

    static void idleBody (void* arg);

    static uint64 constexpr STACK_SIZE = DEFAULT_STACK_SIZE;
    static uint64 constexpr TIME_SLICE = DEFAULT_TIME_SLICE;
};

#endif
