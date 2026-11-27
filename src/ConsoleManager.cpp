#include "../h/ConsoleManager.hpp"

#include "../h/scheduler.hpp"


char ConsoleManager::inputBuffer[BUFFER_SIZE];
int ConsoleManager::inputHead = 0;
int ConsoleManager::inputTail = 0;


char ConsoleManager::outputBuffer[BUFFER_SIZE];
int ConsoleManager::outputHead = 0;
int ConsoleManager::outputTail = 0;

SemaphoreKernel* ConsoleManager::fullBafferSemaphore = nullptr;
SemaphoreKernel* ConsoleManager::inputSemaphore = nullptr;

void ConsoleManager::init() {


    fullBafferSemaphore = SemaphoreKernel::semOpen(BUFFER_SIZE);
    inputSemaphore = SemaphoreKernel::semOpen(0);

}

void ConsoleManager::flush() {
    // Dokle god ima podataka u izlaznom baferu prepustamo procesor drugima
    while (outputHead != outputTail) {
        TCB::dispatch();
    }
}

// Citanje iz kruznog bafera
char ConsoleManager::get_c() {
    // ako nema karaktera u baferu nit treba da spava
    inputSemaphore->wait();

    char c = inputBuffer[inputHead];
    inputHead = (inputHead + 1) % BUFFER_SIZE;

    return c;


}


//  Upis u kruzni bafer
void ConsoleManager::put_c(char c) {

    // cekamo da se u baferu oslobodi mesto
    fullBafferSemaphore->wait();

    // kad se nit probudi sigurno ima bar jedno slobodno mesto
    outputBuffer[outputTail] = c;
    outputTail = (outputTail + 1) % BUFFER_SIZE;

    // provera ako moze odmah da se isprazni bafer
    handleConsoleInterrupt();


}

void ConsoleManager::handleConsoleInterrupt() {
    uint8 getReadyMask = 1 << 0;
    uint8 putReadyMask = 1 << 5;

    // Ulazni smer sa tastature
    while (*(volatile uint8*)CONSOLE_STATUS & getReadyMask) {
        char c = *((volatile uint8*)CONSOLE_RX_DATA);
        int nextTail = (inputTail + 1) % BUFFER_SIZE;

        if (nextTail != inputHead) { // Ako bafer nije pun
            inputBuffer[inputTail] = c;
            inputTail = nextTail;
            inputSemaphore->signal(); // budimo nit koja ceka na getc
        }
    }

    // Izlazni smer ka ekranu
    while ( (*(volatile uint8*)CONSOLE_STATUS & putReadyMask) && (outputHead != outputTail) ) {
        char c = outputBuffer[outputHead];
        *(volatile uint8*)CONSOLE_TX_DATA = c;
        outputHead = (outputHead + 1) % BUFFER_SIZE;

        fullBafferSemaphore->signal(); // oslobodili smo mesto u izlaznom baferu

    }
}
