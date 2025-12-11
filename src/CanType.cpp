#include "CanType.h"

#include <iostream>
#include <iomanip>

void printFrame(const CanFrame &f, const std::string &prefix) {
    std::cout << prefix
              << "ID=0x" << std::hex << std::uppercase << f.id
              << " DLC=" << std::dec << (int)f.dlc
              << " DATA=";
    for (int i = 0; i < f.dlc; ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << (int)f.data[i] << ' ';
    }
    std::cout << std::dec << std::nouppercase << '\n';
}
