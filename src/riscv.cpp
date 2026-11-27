#include "../h/riscv.hpp"

#include "../h/ConsoleManager.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/tcb.hpp"
#include "../h/semaphoreKernel.hpp"
#include "../h/SleepManager.hpp"

void Riscv::popSppSpie() {

    // iz statusnog registra popuje spp i spie
    // sepc nam kaze gde se odavde vracamo
    // zato sto ubacujemo ra onda ova fja NE SME biti inline jer da jeste ne bi nam promenila ra
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}

void Riscv::handleSupervisorTrap(uint64* savedRegs) {
    uint64 scause = r_scause();

    uint64 sepc = r_sepc();
    uint64 sstatus = r_sstatus();

    if ( scause == 0x0000000000000008UL) {

        // interrupt: no, cause code: environment call from U-mode (8)
        // ne zelimo da se vratimo na adresu ecall-a vec na adresu instrukcije iza pa preskacemo ecall sa ovim + 4
        sepc = sepc + 4;

        handleSyscall(savedRegs);

    }
    else if ( scause == 0x0000000000000009UL) {

        // interrupt: no, cause code: environment call from S-mode (9)
        // ne zelimo da se vratimo na adresu ecall-a vec na adresu instrukcije iza pa preskacemo ecall sa ovim + 4
        sepc = sepc + 4;

        handleSyscall(savedRegs);

    }
    else if (scause == 0x8000000000000001UL) {

        // interrupt: yes, cause code: supervisor software interrupt (timer)

        // Azuriramo sve uspavane niti
        SleepManager::update();

        // ako je niti istekao vremenski odsecak

        TCB::timeSlicecCounter++;
        if (TCB::timeSlicecCounter >= TCB::running->getTimeSlice()) {

            TCB::timeSlicecCounter = 0;
            TCB::dispatch();


        }
        // gasimo softverske prekide
        mc_sip(SIP_SSIP);

    }
    else if (scause == 0x8000000000000009UL) {
        // interrupt: yes, cause code: supervisor external interrupt (timer)

        //za proveru koji spoljasnji uredjaj je izazvao prekid
        // isto je kod i za ulaz i za izlaz
        int request = plic_claim();

        if (request == 10) {// prekid od konzole )
            ConsoleManager::handleConsoleInterrupt();
        }

         //saljemo obavestenje da je prekid uspesno obradjen
        plic_complete(request);


    } else {

        // unexpected trap cause

        // print(stval) opsisuje dodatno u zavisnosti koji je scause

    }
    w_sstatus(sstatus);
    w_sepc(sepc);
}

void Riscv::handleSyscall(uint64* savedRegs) {
    uint64 a0_code = savedRegs[10]; // pristupa lokaciji sp+80 u memoriji!
    uint64 a1_arg = savedRegs[11]; // pristupa lokaciji sp+88 u memoriji!

    if (a0_code == 0x01) { // mem_alloc code
        size_t blocks = (size_t)a1_arg;
        size_t bytes = blocks * MEM_BLOCK_SIZE;
        void* allocated_mem_adr = MemoryAllocator::malloc(bytes);

        savedRegs[10] = (uint64)allocated_mem_adr;

    }
    else if (a0_code == 0x02) { // mem_free code
        void* mem_adr = (void*)a1_arg;
        MemoryAllocator::free(mem_adr);

        uint64 result = 0;
        savedRegs[10] = (uint64)result;

    }
    else if (a0_code == 0x11) { // thread_create
        // Izvlacimo argumente koje je korisnik stavio u registe pre ecall-a
        TCB** handle = (TCB**)savedRegs[11]; // a1
        TCB::Body start_routine = (TCB::Body)savedRegs[12]; // a2
        void* arg  = (void*)savedRegs[13]; // a3

        // Kreiramo nit
        TCB* newThread = TCB::createThread(start_routine, arg, nullptr);

        if (newThread != nullptr) {
            *handle = newThread;
            Scheduler::put(newThread);
            savedRegs[10] = 0; // vracamo korisniku 0 (preko a0) u slucaju uspeha
        }else {
            savedRegs[10] = -1; // vracamo korisniku -1 u slucaju neuspeha (preko a0)
        }


    }
    else if (a0_code == 0x12) { // thread_exit

        // Obelezavamo nit kao zavrsenu i uklanjamo je sa procesora
        TCB::running->setFinished(true);
        TCB::dispatch();

    }
    else if (a0_code == 0x13) { // thread_dispatch
        TCB::timeSlicecCounter = 0;
        TCB::dispatch();

    }
    else if (a0_code == 0x21) { // sem_open
        SemaphoreKernel** handle = (SemaphoreKernel**)savedRegs[11];
        unsigned init = (unsigned)savedRegs[12];

        // Kreiramo semafor
        SemaphoreKernel* sem = SemaphoreKernel::semOpen(init);

        if (sem != nullptr) {
            *handle = sem;
            savedRegs[10] = 0; // vracamo korisniku 0 (preko a0) u slucaju uspeha
        }else {
            savedRegs[10] = -1; // vracamo korisniku -1 u slucaju neuspeha (preko a0)
        }

    }
    else if (a0_code == 0x22) { // sem_close
        SemaphoreKernel* handle = (SemaphoreKernel*)savedRegs[11];

        if (handle != nullptr) {
            uint64 ret = handle->semClose();
            savedRegs[10] = ret;
        }else {
            savedRegs[10] = -1;
        }

    }
    else if (a0_code == 0x23) { //sem_wait
        SemaphoreKernel* handle = (SemaphoreKernel*)savedRegs[11];

        int ret = handle->wait();

        savedRegs[10] = ret;


    }
    else if (a0_code == 0x24) { //sem_signal
        SemaphoreKernel* handle = (SemaphoreKernel*)savedRegs[11];

        int ret = handle->signal();

        savedRegs[10] = ret;
    }
    else if (a0_code == 0x25) { // sem_wait_n
        SemaphoreKernel* handle = (SemaphoreKernel*)savedRegs[11]; // iz a1 uzimamo rucku
        unsigned n = (unsigned)savedRegs[12]; // iz a2 uzimamo n

        if (handle != nullptr) {
            uint64 ret = handle->waitN(n);
            savedRegs[10] = ret;
        }else {
            savedRegs[10] = -1;
        }

    }
    else if (a0_code == 0x26) { // sem_signal_n
        SemaphoreKernel* handle = (SemaphoreKernel*)savedRegs[11]; // iz a1 uzimamo rucku
        unsigned n = (unsigned)savedRegs[12]; // iz a2 uzimamo n

        if (handle != nullptr) {
            uint64 ret = handle->signalN(n);
            savedRegs[10] = ret;
        }else {
            savedRegs[10] = -1;
        }

    }
    else if (a0_code == 0x31) { //time_sleep
        time_t time_slices = (time_t)savedRegs[11];

        if (time_slices <= 0) {
            savedRegs[10] = -1;
        }
        else {
            savedRegs[10] = 0;

            SleepManager::sleep(time_slices);

        }


    }
    else if (a0_code == 0x41) { //getc
        char c = ConsoleManager::get_c();

        savedRegs[10] = (uint64)c;
    }
    else if (a0_code == 0x42) { //putc

        char c = (char)savedRegs[11]; // citamo iz a1 prosledjeno c

        ConsoleManager::put_c(c);

    }
    else {
        // nesto drugo
    }

}
