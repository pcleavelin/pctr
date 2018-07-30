#include <iostream>

#include "pctr.h"

int main(int argc, char** argv) {
    if(argc < 2) {
        std::cerr << "No file specified\n";
        return 1;
    }

    PCTR pctr = PCTR();

    pctr.initialize(argv[0]);

    int return_value = pctr.execute(argv[1], "main");

    pctr.dispose();
    return return_value;
}