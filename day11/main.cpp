#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>

#include "cpu.hpp"

using coord_t = std::pair<int,int>;
using grid_t = std::map<coord_t,int>;

enum class Direction {
    UP, RIGHT, DOWN, LEFT
};

InstrExecStatus run_until_output_is_produced_or_halt(CPU &cpu) {
    while(cpu.peek_current_opcode() % 100 != 4) {
        if(cpu.run_one_instruction() == InstrExecStatus::HALT)
            return InstrExecStatus::HALT;
    }
    // Execute the output instruction we just found
    return cpu.run_one_instruction();
}

InstrExecStatus run_for_two_outputs(CPU &cpu) {
    if (run_until_output_is_produced_or_halt(cpu) == InstrExecStatus::HALT)
        return InstrExecStatus::HALT;
    return run_until_output_is_produced_or_halt(cpu);
}

void update_robot_pos(coord_t& robot_pos, Direction direction) {
    switch (direction) {
        case Direction::UP:    { robot_pos.second--; return; }
        case Direction::RIGHT: { robot_pos.first++;  return;  }
        case Direction::DOWN:  { robot_pos.second++; return; }
        case Direction::LEFT:  { robot_pos.first--;  return;  }
    }
}

Direction update_robot_direction(Direction current_direction, int turn) {
    if (turn == 1) {
        return static_cast<Direction>((static_cast<int>(current_direction) + 1) % 4);
    } else {
        return static_cast<Direction>((static_cast<int>(current_direction) - 1 + 4) % 4);
    }
}

void print_grid(const grid_t& grid, int size) {
    for (int y = -size/2; y <= size/2; y++) {
        for (int x = -size/2; x <= size/2; x++) {
            if (grid.count({x, y}) == 0 || grid.at({x, y}) == 0) std::cout << ".";
            else std::cout << "#";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void run_robot(grid_t& grid, const Tape& tape) {
    std::stringbuf buf;
    std::istream in(&buf);
    std::ostream out(&buf);

    CPU cpu(tape, in, out);

    coord_t robot_pos = {0, 0};
    Direction robot_direction = Direction::UP;

    int color;
    int turn;

    for (auto status = InstrExecStatus::IDLE; status != InstrExecStatus::HALT; ) {
        out << grid[robot_pos] << std::endl;

        status = run_for_two_outputs(cpu);

        in >> color;
        in >> turn;

        grid[robot_pos] = color;

        robot_direction = update_robot_direction(robot_direction, turn);
        update_robot_pos(robot_pos, robot_direction);
    }
}

void part1(const Tape& tape) {
    grid_t grid;

    run_robot(grid, tape);

    std::cout << "Part 1: " << grid.size() << std::endl;
}

void part2(const Tape& tape) {
    // Initialize grid with position (0,0) set to 1
    grid_t grid{{{0,0}, 1}};

    run_robot(grid, tape);

    std::cout << "Part 2: " << std::endl;
    print_grid(grid, 100);
}

int main() {
    Tape tape = read_tape_from_disk("input.txt");

    part1(tape);
    part2(tape);
}