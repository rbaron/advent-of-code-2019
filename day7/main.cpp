#include <algorithm>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>

#include "cpu.hpp"

InstrExecStatus run_until_output_is_produced_or_halt(CPU &cpu) {
    while(cpu.peek_current_opcode() != 4) {
        if(cpu.run_one_instruction() == InstrExecStatus::HALT)
            return InstrExecStatus::HALT;
    }
    // Execute the output instruction we just found
    return cpu.run_one_instruction();
}

struct Amplifier {
    Amplifier(const int phase, CPU cpu): phase(phase), cpu(cpu), has_read_phase(false) {}
    const int phase;
    CPU cpu;
    bool has_read_phase;
};

struct Result {
    Result(int output, InstrExecStatus status): output(output), status(status) {}
    int output;
    InstrExecStatus status;
};

std::vector<Amplifier> make_amplifiers(const std::vector<int>& phases, std::function<CPU()> make_cpu) {
    std::vector<Amplifier> amplifiers;
    std::transform(
        phases.begin(),
        phases.end(),
        std::back_inserter(amplifiers),
        [&make_cpu](int phase) { return Amplifier(phase, make_cpu()); }
    );
    return amplifiers;
};

Result run_array_of_amplifiers(
        std::vector<Amplifier>& amplifiers,
        int input,
        std::istream& in,
        std::ostream& out) {

    InstrExecStatus status;
    std::for_each(
        amplifiers.begin(),
        amplifiers.end(),
        [&in, &out, &input, &status](Amplifier& amp) {

            if (!amp.has_read_phase) {
                out << amp.phase << std::endl;
                amp.has_read_phase = true;
            }

            out << input << std::endl;

            status = run_until_output_is_produced_or_halt(amp.cpu);

            // Pick up the current amplifier's output into the `input` variable to be fed
            // to the next amplifier
            in >> input;
        }
    );

    // Returns the output of the last amplifier along with the CPU status
    return Result(input, status);
}

int get_max_output_for_permutation(std::vector<int>& phases, const Tape& tape) {

    // I/O channels for connecting inputs and outputs of the amplifiers
    std::stringbuf buf;
    std::istream in(&buf);
    std::ostream out(&buf);

    auto make_cpu = [&tape, &in, &out]() { return CPU(tape, in, out); };

    int max = std::numeric_limits<int>::min();

    // For each possible permutation of phases (assumed to be lexicographically sorted to begin with)
    do {
            auto amplifiers = make_amplifiers(phases, make_cpu);

            auto result = Result(0, InstrExecStatus::IDLE);

            while (result.status != InstrExecStatus::HALT)
                result = run_array_of_amplifiers(amplifiers, result.output, in, out);

            max = result.output > max ? result.output : max;

    } while (std::next_permutation(phases.begin(), phases.end()));

    return max;
}

int main() {
    const Tape tape = read_tape_from_disk("input.txt");

    // Part 1
    std::vector<int> phases1{0, 1, 2, 3, 4};
    std::cout << "Part 1: " << get_max_output_for_permutation(phases1, tape) << std::endl;

    // Part 2
    std::vector<int> phases2{5, 6, 7, 8, 9};
    std::cout << "Part 2: " << get_max_output_for_permutation(phases2, tape) << std::endl;
}