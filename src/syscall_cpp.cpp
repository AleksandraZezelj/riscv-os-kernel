#include "../h/syscall_cpp.hpp"



void* operator new (size_t size){
    return mem_alloc(size);
}
void operator delete (void* ptr){
    if (ptr == nullptr){ return;}
    mem_free(ptr);
}

void* operator new[] (size_t size){
    return mem_alloc(size);
}
void operator delete[] (void* ptr){
    if (ptr == nullptr){ return;}
    mem_free(ptr);
}


void threadRun(void* arg) {
    // body e sigurno nullptr
    Thread* myThread = (Thread*)arg;
    if (myThread != nullptr) {
        myThread->run();

    }

}

Thread::Thread(void (*body) (void*), void* arg) {
    this->body = body;
    this->arg = arg;
    this->myHandle = nullptr;
}

Thread::Thread() {
    this->body = nullptr;
    this->arg = nullptr;
    this->myHandle = nullptr;

}

Thread::~Thread() {}

int Thread::start() {
    if (body != nullptr) {
        // Ako je korisnik dao fju kroz konstruktor pokrecemo nju
        return thread_create(&myHandle, body, arg);
    }
    else {
        // Ako je korisnik izveden iz klase Thread i overide-ovao je run()
        return thread_create(&myHandle, threadRun, this);
    }

}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t time) {
    return time_sleep(time);
}

Semaphore::Semaphore(unsigned init) {
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {
    sem_close(myHandle);
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}

PeriodicThread::PeriodicThread(time_t period) : Thread() {
    this->period = period;
}

void PeriodicThread::terminate() {
    // signaliziramo kraj petlje u run()
    this->period = 0;
}

void PeriodicThread::run() {
    while (period > 0) {
        periodicActivation();
        if (period > 0) {
            Thread::sleep(period);
        }
    }
}


char Console::getc() {
    // zovemo fju iz globalnog opsega (ne onu iz ove klase)
    return ::getc();
    //return __getc();
}

void Console::putc(char c) {
    // zovemo fju iz globalnog opsega (ne onu iz ove klase)
    ::putc(c);
    //__putc(c);
}