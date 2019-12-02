#include <fstream>
#include <iostream>
#include <string>
#include <vector>

enum class InstrExecStatus {
    ALL_GOOD, HALT, MAMMA_MIA_THATS_GONNA_BE_AN_ERROR
};

InstrExecStatus exec_instr(int pc, std::vector<int>& tape) {
    switch (tape.at(pc)) {
        case 1:
            tape[tape[pc + 3]] = tape[tape[pc + 1]] + tape[tape[pc + 2]];
            return InstrExecStatus::ALL_GOOD;
        case 2:
            tape[tape[pc + 3]] = tape[tape[pc + 1]] * tape[tape[pc + 2]];
            return InstrExecStatus::ALL_GOOD;
        case 99:
            return InstrExecStatus::HALT;
        default:
            return InstrExecStatus::MAMMA_MIA_THATS_GONNA_BE_AN_ERROR;
    }
    return InstrExecStatus::ALL_GOOD;
}

void run_program(std::vector<int>& tape) {
    // Program Counter
    int pc = 0;
    while (exec_instr(pc, tape) == InstrExecStatus::ALL_GOOD)
        pc += 4;
}

const std::vector<int> read_tape_from_disk(const std::string& filename) {
    std::vector<int> tape;

    std::ifstream file(filename);
    std::string tmp;
    while(std::getline(file, tmp, ',')) tape.emplace_back(std::stoi(tmp));
    return tape;
}

void print_tape(const std::vector<int>& tape) {
    std::for_each(
        tape.begin(),
        tape.end(),
        [](int cell) { std::cout << cell << ","; }
    );
}

int main() {
    // Immutable tape we'll use for restoring the initial state
    auto initial_tape = read_tape_from_disk("input.txt");

    // Part 1

    std::vector<int> tape(initial_tape);
    tape[1] = 12;
    tape[2] = 2;

    run_program(tape);
    std::cout << "Part 1: first tape cell contains: " << tape.at(0) << std::endl;

    // Part 2

    for (int noun = 0; noun <= 99; noun++) {
        for (int verb = 0; verb <= 99; verb++) {
            std::vector<int> tape(initial_tape);
            tape[1] = noun;
            tape[2] = verb;
            run_program(tape);
            if (tape[0] == 19690720) {
                std::cout << "Part 2: found nount and verb such that 100*noun + verb = " << 100*noun + verb << std::endl;
                return 0;
            }
        }
    }
}