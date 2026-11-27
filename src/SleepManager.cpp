#include "../h/SleepManager.hpp"


List<TCB> SleepManager::sleepList;

void SleepManager::sleep(time_t timeSlice) {

    TCB* running = TCB::running;
    running->setSleepingCount((int)timeSlice);
    running->setSleeping(true);

    sleepList.addLast(running);

    TCB::dispatch();


}

void SleepManager::update() {

    // pozivamo update iz prekidne rutine tajmera (prekidi su ugaseni)
    int size = sleepList.size();
    if (size == 0) return;


    for (int i = 0; i < size; i++) {
        TCB* thread = sleepList.removeFirst();
        if (!thread) break;


        thread->setSleepingCount(thread->getSleepingCount()-1);

        if (thread->getSleepingCount() <= 0) {

            thread->setSleeping(false);
            // Ako je isteklo vreme vracamo je u scheduler da nastavi rad
            Scheduler::put(thread);
        }
        else {
            //  Ako nije vracamo je na kraj lsite da ceka sledeci otkucaj
            sleepList.addLast(thread);
        }

    }


}
