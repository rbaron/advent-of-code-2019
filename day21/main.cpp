#include <iostream>

#include "cpu.hpp"

void run_program(const Tape& tape, const std::string& code) {
    std::stringbuf buf;
    std::istream in(&buf);
    std::ostream out(&buf);

    CPU cpu(tape, in, out);

    for (auto c : code) out << static_cast<int>(c) << std::endl;

    cpu.run_program();
    int v;
    while (in >> v) {
        if (v <= '~')
            std::cout << static_cast<char>(v);
        else
            std::cout << "Score: " << v << std::endl;
    }
}

void part1(const Tape& tape) {
    // (not(3) || not(2) || not(1)) && 4
    // Jump as soon as possible, if there's ground in 4
    std::string code(
R"(NOT C J
NOT B T
OR T J
NOT A T
OR T J
AND D J
WALK
)");

    run_program(tape, code);
}

void part2(const Tape& tape) {
    // ((not(3) || not(2) || not(1)) && 4) && (5 || 8)
    // Jump as soon as possible, if there's ground in 4 and there's ground at 8 or
    // We can take an additional step at 5 (after jumping)
    std::string code(
R"(NOT C J
NOT B T
OR T J
NOT A T
OR T J
AND D J
NOT E T
NOT T T
OR H T
AND T J
RUN
)");

    run_program(tape, code);
}

int main() {
    const auto tape = read_tape_from_disk("input.txt");

    std::cout << "Part 1: " << std::endl;
    part1(tape);

    std::cout << "Part 2: " << std::endl;
    part2(tape);
}