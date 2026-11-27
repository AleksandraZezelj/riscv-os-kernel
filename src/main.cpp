
#include "../h/Kernel.hpp"

#include "../h/ConsoleManager.hpp"
//#include "../lib/console.h"

void userMain();

int main() {
    Kernel::initialize();

    userMain();



    ConsoleManager::flush();

    Kernel::exitQemu();
    return 0;
}