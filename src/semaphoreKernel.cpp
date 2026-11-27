#include "../h/semaphoreKernel.hpp"
#include "../h/scheduler.hpp"
#include "../h/tcb.hpp"

SemaphoreKernel* SemaphoreKernel::semOpen(unsigned init) {

    return new SemaphoreKernel(init);
}

int SemaphoreKernel::semClose() {

    TCB* thread = blocked.removeFirst();
    while (thread != nullptr) {
        thread->setSemError(true);
        thread->setBlocked(false);
        Scheduler::put(thread);
        thread = blocked.removeFirst();
    }
    return 0;

}

int SemaphoreKernel::wait() {

    int ret = waitN(1);

    return ret;
}

int SemaphoreKernel::signal() {

    int ret = signalN(1);

    return ret;

}

int SemaphoreKernel::waitN(int n) {

    if (n <= 0) return -1;

    // ako ima dovoljno odmah uzmi i prodji
    if (val >= n) {
        val -= n;
        return 0;
    }

    TCB::running->waiting = n;
    TCB::running->setBlocked(true);
    blocked.addLast(TCB::running);

    TCB::dispatch();

    if (TCB::running->getSemError()) {
        TCB::running->setSemError(false);

        return -2;
    };

    return 0;


}

int SemaphoreKernel::signalN(int n) {

    if (n <=0) return -1;

    val += n;

    // prolazimo kroz blokirane niti redom FIFO
    while (blocked.size() > 0) {

        TCB* thread = blocked.peekFirst();
        if (thread == nullptr) {break;}

        if (thread->waiting<= val){
            val -= thread->waiting;
            blocked.removeFirst();
            thread->waiting = 0;
            thread->setBlocked(false);
            Scheduler::put(thread);

        }else {
            // ako prva nit trazi vise nego sto imamo slobodno
            // po FIFO redosledu prekidamo odblokiranje, ona drzi red
            break;

        }


    }

    return 0;
}


