#include "../h/scheduler.hpp"

List<TCB> Scheduler::readyCoroutineQueue;

TCB* Scheduler::get() {
    // zabranjujemo prekide
    uint64 sstatus = Riscv::r_sstatus();
    Riscv::mc_sstatus(Riscv::SSTATUS_SIE);

    TCB* ret =  readyCoroutineQueue.removeFirst();

    // vracamo na staru vrednost s tim da su mozda bili pre zabranjeni pa i o tome brinemo
    //and-ujemo staru vrdnost sa ovom
    Riscv::ms_sstatus(sstatus & Riscv::SSTATUS_SIE ? Riscv::SSTATUS_SIE : 0);

    return ret;
}

void Scheduler::put(TCB* ccb) {
    // zabranjujemo prekide
    uint64 sstatus = Riscv::r_sstatus();
    Riscv::mc_sstatus(Riscv::SSTATUS_SIE);

    readyCoroutineQueue.addLast(ccb);

    // vracamo na staru vrednost s tim da su mozda bili pre zabranjeni pa i o tome brinemo
    //and-ujemo staru vrdnost sa ovom
    Riscv::ms_sstatus(sstatus & Riscv::SSTATUS_SIE ? Riscv::SSTATUS_SIE : 0);
}


