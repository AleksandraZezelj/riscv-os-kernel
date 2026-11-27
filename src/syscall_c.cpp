#include "../h/syscall_c.hpp"
#include "../lib/hw.h"

void* mem_alloc(size_t size) {

    size_t blocks = (size + MEM_BLOCK_SIZE-1)/MEM_BLOCK_SIZE;

    uint64 retval;

    __asm__ volatile(
        "mv a1, %1;"    // premesti blokove u registar a1
        "li a0, 0x01;"  // postavi kod operacije 0x01 u a0
        "ecall;"        // skok u prekidnu rutinu
        "mv %0, a0;"    // pokupi povratnu vrenost iz a0 u retval
        : "=r"(retval)  // izlazni parametar (%0)
        : "r"(blocks)   // ulazni parametar (%1)
        : "a0", "a1"    //napomena kompajleru da radimo sa ovim registrima
        );

    return (void*)retval;

}

int mem_free(void* ptr) {
    if (ptr == 0) {return 0;}

    uint64 retval;
    __asm__ volatile(
         "mv a1, %1;"    // premesti ptr u registar a1
         "li a0, 0x02;"  // postavi kod operacije 0x02 u a0
         "ecall;"        // skok u prekidnu rutinu
         "mv %0, a0;"    // pokupi povratnu vrenost iz a0 u retval
         : "=r"(retval)  // izlazni parametar (%0)
         : "r"(ptr)      // ulazmi parametar (%1)
         : "a0", "a1"     //napomena kompajleru da radimo sa ovim registrima
         );

    return (int)retval;

}

int thread_create(thread_t *handle, void (*start_routine)(void *), void *arg) {
    uint64 retval;
    __asm__ volatile (
        "mv a3, %3;"   // arg ide u a3
        "mv a2, %2;"   // fja ide u a2
        "mv a1, %1;"   //handle ide u a1
        "li a0, 0x11;" // kod ide u a0
        "ecall;"
        "mv %0, a0;"   // uzimamo povratnu vrednost iz a0 i  smestamo je u retval
        : "=r"(retval)
        : "r"(handle), "r"(start_routine), "r"(arg)
        : "a0", "a1", "a2", "a3"
    );

    return (int)retval;
}

int thread_exit() {
    uint64 retval;
    __asm__ volatile (
    "li a0, 0x12;" // kod operacije ide u a0
    "ecall;"
    "mv %0, a0;"   // uzimamo povratnu vrednost iz a0 i  smestamo je u retval
    : "=r"(retval)
    :
    : "a0"
    );

    return (int)retval;
}

void thread_dispatch() {
    __asm__ volatile (
        "li a0, 0x13;"
        "ecall;"
        :
        :
        : "a0"

        );
}

int sem_open(sem_t* handle, unsigned init) {
    uint64 retval;
    __asm__ volatile (
        "mv a1, %1;"            // premesti handle u registar a1
        "mv a2, %2;"             // premesti init u registar a2
        "li a0, 0x21;"          // postavi kod operacije 0x21 u a0
        "ecall;"                 // skok u prekidnu rutinu
        "mv %0, a0;"            // pokupi povratnu vrenost iz a0 u retval
        :"=r"(retval)           // izlazni parametar (%0)
        :"r"(handle), "r"(init) // ulazmi parametari
        : "a0", "a1", "a2"
    );

    return (int)retval;
}

int sem_close(sem_t handle) {
    uint64 retval;
    __asm__ volatile (
        "mv a1, %1;"   // premesti handle u registar a1
        "li a0, 0x22;" // postavi kod operacije 0x22 u a0
        "ecall;"        // skok u prekidnu rutinu
        "mv %0, a0;"   // pokupi povratnu vrenost iz a0 u retval
        :"=r"(retval)  // izlazni parametar (%0)
        :"r"(handle)   // ulazmi parametari
        : "a0", "a1"
    );

    return (int)retval;
}

int sem_wait(sem_t id) {
    uint64 retval;
    __asm__ volatile (
        "mv a1, %1;"   // premesti handle u registar a1
        "li a0, 0x23;" // postavi kod operacije 0x23 u a0
        "ecall;"        // skok u prekidnu rutinu
        "mv %0, a0;"   // pokupi povratnu vrenost iz a0 u retval
        :"=r"(retval)  // izlazni parametar (%0)
        :"r"(id)   // ulazmi parametari
        : "a0", "a1"
    );

    return (int)retval;
}


int sem_signal(sem_t id) {
    uint64 retval;
    __asm__ volatile (
        "mv a1, %1;"   // premesti handle u registar a1
        "li a0, 0x24;" // postavi kod operacije 0x24 u a0
        "ecall;"        // skok u prekidnu rutinu
        "mv %0, a0;"   // pokupi povratnu vrenost iz a0 u retval
        :"=r"(retval)  // izlazni parametar (%0)
        :"r"(id)   // ulazmi parametari
        : "a0", "a1"
    );

    return (int)retval;
}

int sem_wait_n(sem_t id, unsigned n) {
    uint64 retval;
    __asm__ volatile (
        "mv a1, %1;"   // premesti id u registar a1
        "mv a2, %2;"   // premesti n u registar a2
        "li a0, 0x25;" // postavi kod operacije 0x25 u a0
        "ecall;"        // skok u prekidnu rutinu
        "mv %0, a0;"   // pokupi povratnu vrenost iz a0 u retval
        :"=r"(retval)  // izlazni parametar (%0)
        :"r"(id), "r"(n) // ulazni parametri
        : "a0", "a1", "a2"
        );

    return (int)retval;
}

int sem_signal_n(sem_t id, unsigned n) {
    uint64 retval;
    __asm__ volatile (
        "mv a1, %1;"   // premesti handle u registar a1
        "mv a2, %2;"   // premesti handle u registar a2
        "li a0, 0x26;" // postavi kod operacije 0x26 u a0
        "ecall;"        // skok u prekidnu rutinu
        "mv %0, a0;"   // pokupi povratnu vrenost iz a0 u retval
        :"=r"(retval)  // izlazni parametar (%0)
        :"r"(id), "r"(n) // ulazni parametri
        : "a0", "a1", "a2"
        );

    return (int)retval;
}

int time_sleep(time_t time_slice) {
    uint64 retval;
    __asm__ volatile (
        "mv a1, %1;"   // premesti time_slice u registar a1
        "li a0, 0x31;" // postavi kod operacije 0x31 u a0
        "ecall;"        // skok u prekidnu rutinu
        "mv %0, a0;"   // pokupi povratnu vrenost iz a0 u retval
        :"=r"(retval)  // izlazni parametar (%0)
        :"r"(time_slice) // ulazni parametri
        : "a0", "a1"
        );

    return (int)retval;
}

char getc() {
    uint64 retval;

    __asm__ volatile (
        "li a0, 0x41;" // postavi kod operacije 0x41 u a0
        "ecall;"       // skok u prekidnu rutinu
        "mv %0, a0;"
        :"=r"(retval)
        :
        : "a0"

        );

    return (char)retval;
}

void putc(char c) {
    __asm__ volatile (
        "mv a1, %0;"   // premesti c u registar a1
        "li a0, 0x42;" // postavi kod operacije 0x42 u a0
        "ecall;"       // skok u prekidnu rutinu
        :
        :"r"(c)
        : "a0", "a1"

        );
}



