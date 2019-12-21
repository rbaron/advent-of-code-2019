#include <iostream>

#include "cpu.hpp"

void part1(const Tape& tape) {
    std::stringbuf buf;
    std::istream in(&buf);
    std::ostream out(&buf);

    CPU cpu(tape, in, out);

    /*
        Jump if there is a hole between 1 and 3 (inclusive), _unless_ there's a hole at 4
    */
    std::string code(
R"(NOT C J
NOT B T
OR T J
NOT A T
OR T J
AND D J
WALK
)");

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

int main() {
    const auto tape = read_tape_from_disk("input.txt");

    part1(tape);
}