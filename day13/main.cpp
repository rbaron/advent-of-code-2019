#include <algorithm>
#include <iostream>
#include <sstream>
#include <map>
#include <numeric>

#include "cpu.hpp"


enum class GameObj {
    EMPTY, WALL, BLOCK, H_PADDLE, BALL,
};

using coord_t = std::pair<int,int>;
using grid_t = std::map<coord_t,GameObj>;

grid_t to_grid(grid_t& grid, std::istream& in) {
    int x, y, obj;
    while (in) {
        in >> x;
        in >> y;
        in >> obj;

        if (x == -1) std::cout << "SCORE: " << obj << std::endl;
        else grid[{x,y}] = static_cast<GameObj>(obj);
    }
    return grid;
}

int count_blocks(const grid_t& grid) {
    return std::count_if(
        grid.begin(),
        grid.end(),
        [](std::pair<coord_t,GameObj>kv) { return kv.second == GameObj::BLOCK; }
    );
}

char to_char(GameObj obj) {
    switch (obj) {
        case GameObj::EMPTY: return '.';
        case GameObj::WALL: return 'W';
        case GameObj::BLOCK: return '#';
        case GameObj::H_PADDLE: return '-';
        case GameObj::BALL: return 'o';
        default: return '?';
    }
}
void draw_grid(const grid_t& grid) {
    for(int y = 0; y < 30; y++) {
        for(int x = 0; x < 60; x++) {
            if (grid.count({x, y}) > 0) std::cout << to_char(grid.at({x, y}));
            else std::cout << "+";
        }
        std::cout << std::endl;
    }
}

InstrExecStatus run_until_input_is_required(CPU &cpu) {
    InstrExecStatus status;
    while(cpu.peek_current_opcode() % 100 != static_cast<int>(OpCodes::INPUT)) {
        status = cpu.run_one_instruction();
        if (status == InstrExecStatus::HALT) return status;
    }
    return status;
}

coord_t get_pos(const grid_t& grid, GameObj obj) {
    return std::find_if(
        grid.begin(),
        grid.end(),
        [obj](const std::pair<coord_t, GameObj>& kv) { return kv.second == obj; }
    )->first;
}

bool exists(const grid_t& grid, GameObj obj) {
    return std::count_if(
        grid.begin(),
        grid.end(),
        [obj](const std::pair<coord_t, GameObj>& kv) { return kv.second == obj; }
    ) > 0;
}

int sign(int n) {
    return n == 0 ? 0 : n / std::abs(n);
}

void part1(const Tape& tape_in, std::istream& in, std::ostream& out) {
    grid_t grid;
    Tape tape = tape_in;
    auto cpu = CPU(tape, in, out);
    cpu.run_program();
    to_grid(grid, in);
    std::cout << "Part 1: " << count_blocks(grid) << std::endl;
}

void part2(const Tape& tape_in, std::istream& in, std::ostream& out) {
    grid_t grid;

    Tape tape = tape_in;

    // Insert coin
    tape[0] = 2;

    auto cpu = CPU(tape, in, out);
    run_until_input_is_required(cpu);
    to_grid(grid, in);

    auto last_ball_pos = get_pos(grid, GameObj::BALL);
    while (count_blocks(grid) > 0) {
        auto ball_pos = get_pos(grid, GameObj::BALL);
        auto paddle_pos = get_pos(grid, GameObj::H_PADDLE);
        grid[ball_pos] = GameObj::EMPTY;
        grid[paddle_pos] = GameObj::EMPTY;

        coord_t ball_vel = {ball_pos.first - last_ball_pos.first, ball_pos.second - last_ball_pos.second};
        last_ball_pos = ball_pos;

        int ball_height = -(ball_pos.second - paddle_pos.second);
        int prediction_x = ball_pos.first + ball_vel.first * ball_height;

        std::cout << "Ball: " << ball_pos.first << ", " << ball_pos.second << std::endl;
        std::cout << "Ball vel: " << ball_vel.first << ", " << ball_vel.second << std::endl;
        std::cout << "Paddle: " << paddle_pos.first << ", " << paddle_pos.second << std::endl;
        std::cout << "Prediction: " << prediction_x << std::endl;

        // Write joystick input
        out << sign(prediction_x - paddle_pos.first) << std::endl;

        // Step over the INPUT instruction
        cpu.run_one_instruction();

        run_until_input_is_required(cpu);

        to_grid(grid, in);

        if (!exists(grid, GameObj::BALL)) grid[ball_pos] = GameObj::BALL;
        if (!exists(grid, GameObj::H_PADDLE)) grid[paddle_pos] = GameObj::H_PADDLE;

        in.clear();

        draw_grid(grid);
    }
}

int main() {
    std::stringbuf out_buf;
    std::istream in(&out_buf);
    std::ostream out(&out_buf);

    auto tape = read_tape_from_disk("input.txt");

    part1(tape, in, out);

    // Reset istream's EOF bit
    in.clear();

    part2(tape, in, out);
}