#include <iostream>

#include "pctr.h"

int main(int argc, char** argv) {
    PCTR pctr = PCTR();

    pctr.initialize(argv[0]);

    int return_value = pctr.start(argc, argv);

    pctr.dispose();

    std::cout << "returned with " << return_value << "\n";
    return return_value;
}