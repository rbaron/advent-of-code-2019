#include <algorithm>
#include <iostream>
#include <fstream>

#include "cpu.hpp"


int main() {
    Tape tape = read_tape_from_disk("input.txt");
    CPU cpu = CPU(tape, std::cin, std::cout);
    cpu.run_program();
}