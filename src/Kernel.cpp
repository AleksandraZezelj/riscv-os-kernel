#include "../h/Kernel.hpp"
#include "../h/ConsoleManager.hpp"
#include "../h/riscv.hpp"
#include "../h/tcb.hpp"

void Kernel::initialize() {

    // Postavljamo adresu trap hendler-a
    Riscv::w_stvec((uint64)&Riscv::supervisorTrap);

    // Pakujemo trenutni main u TCB strukturu
    TCB::initMainThread();

    // otvaramo semafore za konzolu
    ConsoleManager::init();

    // Postavljamo Idle nit
    TCB::initIdleThread();


    Riscv::ms_sstatus(Riscv::SSTATUS_SIE); // Palimo globalne prekide u S-modu
    Riscv::ms_sip(Riscv::SIP_SSIP); // Palimo softverske prekide
}

void Kernel::exitQemu() {
    *(volatile uint32*)0x100000 = 0x5555;
}
