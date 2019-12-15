#include <iostream>
#include <queue>
#include <set>
#include <sstream>
#include <vector>

#include "cpu.hpp"

using path_t = std::vector<int>;
const std::vector<int> moves = {1, 2, 3, 4};

InstrExecStatus run_until_output_is_produced(CPU &cpu) {
    InstrExecStatus status;
    while(cpu.peek_current_opcode() % 100 != static_cast<int>(OpCodes::OUTPUT)) {
        status = cpu.run_one_instruction();
        if (status == InstrExecStatus::HALT) return status;
    }
    return cpu.run_one_instruction();
}

int run_robot(Tape tape, const path_t& path) {
    std::stringbuf buf;
    std::istream in(&buf);
    std::ostream out(&buf);

    CPU cpu(tape, in, out);
    int out_code = -1;
    for (auto m : path) {
        out << m << std::endl;
        run_until_output_is_produced(cpu);
        in >> out_code;
    }
    return out_code;
}

std::pair<int,int> get_pos(const path_t& path) {
    int x = 0, y = 0;
    for (auto m : path) {
        switch (m) { case 1: y--; break;
                     case 2: y++; break;
                     case 3: x--; break;
                     case 4: x++; break; }
    }
    return std::pair<int,int>(x, y);
}

int main() {
    const Tape tape = read_tape_from_disk("input.txt");

    std::queue<path_t> paths;
    for (auto move : moves) { paths.push(path_t{move}); };

    std::set<std::pair<int,int>> visited;

    while(paths.size() > 0) {
        auto path = paths.front();
        paths.pop();

        auto pos = get_pos(path);
        if (visited.count(pos) > 0) continue;

        visited.insert(pos);

        // Check if robot found ox tank
        auto code = run_robot(tape, path);

        if (code == 2) {
            std::cout << "Part 1: found oxygen tank in " << path.size() << " moves" << std::endl;
            return 0;
        } else if (code == 1) {
            for (auto move : moves) {
                auto new_path(path);
                new_path.push_back(move);
                paths.push(new_path);
            }
        }
    }
}