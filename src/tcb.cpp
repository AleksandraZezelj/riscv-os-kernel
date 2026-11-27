#include "../h/tcb.hpp"

#include "../h/riscv.hpp"
#include "../h/scheduler.hpp"
#include "../h/SleepManager.hpp"

TCB* TCB::idleThread = nullptr;

TCB* TCB::running = nullptr;

uint64 TCB::timeSlicecCounter = 0;

void TCB::initMainThread() {
    running = new TCB(TIME_SLICE);
}

TCB* TCB::createThread(Body body, void* arg, Thread* myThread) {
    return new TCB(body, arg, myThread, TIME_SLICE);
}

void TCB::yield() {

    // prebacuje nas u prekidnu rutinu gde vrsimo promenu konteksta
    __asm__ volatile ("li a0, 0x13");
    __asm__ volatile ("ecall");

}

void TCB::dispatch() {

    TCB* old = running;

    // Ako je gotova ili blokirana ili je bila Idle ne treba je vracati u scheduler
    if (!old->isFinished() && !old->isBlocked() && !old->isSleeping() && old != idleThread) {
        Scheduler::put(old);
    }

    TCB* next = Scheduler::get();

    // Ako je scheduler bio prazan podmecemo Idle nit
    if (next == nullptr) {
        next = idleThread;


    }


    running = next;

    TCB::contextSwitch(&old->context, &running->context);

}

void TCB::threadWrapper() {

    // ovde smo dosli iz prekidne rutine pa moramo da ukinemo privilegije jer se vracamo u korisnicki
    // skidamo stare dozvole i menjamo mogucnost prekida
    Riscv::popSppSpie();

    // Ako je konstruktorom dat pokazivac na fju
    // izvrsavamo nju i prosledjujemo joj argumente i ignorisemo run()
    if (running->body != nullptr) {
        running->body(running->arg);
    }

    // Kada nit zavrsi sa radom oznacavamo da je zavrsila
    running->setFinished(true);
    TCB::yield();

}

void TCB::idleBody(void* arg) {
    volatile int pom = 1;
    while (pom) {
        yield();
    }
}

void TCB::initIdleThread() {
    idleThread = new TCB(&idleBody, nullptr, nullptr, TIME_SLICE);
}

