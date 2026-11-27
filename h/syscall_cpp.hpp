#ifndef _syscall_cpp
#define _syscall_cpp

#include "../h/syscall_c.hpp"

void* operator new (size_t size);
void operator delete (void* ptr);
void* operator new[] (size_t size);
void operator delete[] (void* ptr);

void threadRun(void* arg);

class Thread {
public:
    Thread(void (*body) (void*), void* arg);
    virtual ~Thread();

    int start();

    static void dispatch();
    static int sleep(time_t time);

protected:
    Thread();
    virtual void run() {};

private:
    thread_t myHandle;
    void (*body)(void *);
    void* arg;

    friend void threadRun(void* arg);

};


class Semaphore {
public:
    Semaphore(unsigned init = 1);
    virtual ~Semaphore();

    int wait();
    int signal();

private:
    sem_t myHandle;

};

class PeriodicThread : public Thread {

public:
    void terminate();

protected:
    PeriodicThread(time_t period);
    virtual void periodicActivation(){};
    virtual void run() override;

private:
    time_t period;

    friend void threadRun(void* arg);
};

class Console {
public:
    static char getc();
    static void putc(char c);
};



#endif