# RISC-V Multithreaded Operating System Kernel

A functional, preemptive multithreaded operating system kernel built from scratch in **C++** and **RISC-V Assembly (RV64IMA)** for the **Operating Systems 1** course at the School of Electrical Engineering, University of Belgrade. 

Designed as an embedded-style library kernel running inside the modified **xv6** host environment on QEMU, sharing a single physical address space with the user application.

## Key Features

* **Continuous Memory Allocator:** First-fit / best-fit block-aligned physical memory allocation and deallocation (`mem_alloc`, `mem_free`) managing the heap space between `HEAP_START_ADDR` and `HEAP_END_ADDR`.
* **Multithreading & Context Switching:** Full lifecycle management of lightweight threads (`TCB`), supporting both synchronous (`thread_dispatch`) and asynchronous context switching via custom RISC-V assembly routines.
* **Preemptive Time-Sharing:** Hardware timer interrupt handling (`scause`) enabling time-sliced scheduling (`FIFO` scheduler) and thread sleeping (`time_sleep`).
* **Synchronization Primitives:** Kernel-level counting semaphores supporting standard `wait`/`signal` operations as well as bulk resource acquisition (`sem_wait_n`, `sem_signal_n`) and safe deallocation (`sem_close`).
* **Asynchronous Console I/O:** Interrupt-driven UART serial communication utilizing bounded producer-consumer buffers for non-blocking `getc` and `putc` system calls.
* **Layered Architecture:**
  * **ABI Layer:** Software interrupt (`ecall`) trap handling, privilege mode transitions (User ↔ Supervisor), and register context preservation.
  * **C API:** Procedural system call wrappers (`syscall_c`).
  * **C++ OO API:** Object-oriented abstractions (`Thread`, `Semaphore`, `PeriodicThread`, `Console`) and overloaded global `new`/`delete` operators (`syscall_cpp`).
## Tech Stack & Environment

* **Languages:** C++11, RISC-V Assembly (RV64IMA)
* **Toolchain:** `riscv64-linux-gnu-g++`, GNU Make
* **Emulator:** QEMU (`qemu-system-riscv64`)

## Building & Running

To compile the kernel and run it inside the QEMU emulator:

```bash
make qemu
