#include <iostream>
#include <queue>
#include <set>
#include <sstream>
#include <vector>
#include <map>

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

std::pair<CPU, int> run_robot(CPU cpu, const path_t& path) {
    int out_code = -1;
    for (auto m : path) {
        cpu.get_ostream() << m << std::endl;
        run_until_output_is_produced(cpu);
        cpu.get_istream() >> out_code;
    }
    return {cpu, out_code};
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

path_t path_to_oxygen_tank(const Tape& tape) {
    std::stringbuf buf;
    std::istream in(&buf);
    std::ostream out(&buf);
    CPU cpu(tape, in, out);

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
        auto cpu_code = run_robot(cpu, path);

        if (cpu_code.second == 2) {
            return path;
        } else if (cpu_code.second == 1) {
            for (auto move : moves) {
                auto new_path(path);
                new_path.push_back(move);
                paths.push(new_path);
            }
        }
    }
    return path_t();
}

int simulate_oxygen_dispersion(const Tape& tape_in, const path_t& initial_path) {
    std::stringbuf buf;
    std::istream in(&buf);
    std::ostream out(&buf);
    Tape tape(tape_in);
    CPU cpu_in(tape, in, out);
    const auto& initial_cpu = run_robot(cpu_in, initial_path).first;

    std::queue<std::tuple<CPU, path_t, int>> paths;
    for (auto move : moves) { paths.push({initial_cpu, path_t{move}, 0}); };

    std::set<std::pair<int,int>> visited;

    int max_steps = 0;
    while (paths.size() > 0) {
        auto cpu_path_step = paths.front();
        paths.pop();

        auto old_cpu = initial_cpu;
        auto path = std::get<1>(cpu_path_step);

        auto pos = get_pos(path);
        if (visited.count(pos) > 0) continue;

        visited.insert(pos);

        CPU cpu(old_cpu);

        auto cpu_code = run_robot(cpu, path);
        const auto& new_cpu = cpu_code.first;
        const auto& code = cpu_code.second;

        if (code != 0) {
            auto steps = path.size();
            max_steps = steps > max_steps ? steps : max_steps;
            for (auto move : moves) {
                auto new_path(path);
                new_path.push_back(move);
                paths.push({new_cpu, new_path, steps});
            }
        }
    }
    return max_steps;
}

int main() {
    const Tape tape = read_tape_from_disk("input.txt");

    // Part 1
    auto path = path_to_oxygen_tank(tape);
    std::cout << "Part 1: " << path.size() << std::endl;

    // Part 2
    auto max_steps = simulate_oxygen_dispersion(tape, path);
    std::cout << "Part 2: " << max_steps << std::endl;
}