#include <iostream>
#include <sstream>

#include "cpu.hpp"

inline bool is_inside_beam(const Tape& tape, int x, int y) {
    std::stringbuf buf;
    std::istream in(&buf);
    std::ostream out(&buf);

    CPU cpu(tape, in, out);
    out << x << std::endl;
    out << y << std::endl;
    cpu.run_program();

    int in_beam;
    in >> in_beam;
    return in_beam == 1;
}

void part1(const Tape& tape) {
    const int width = 50;
    const int height = width;

    size_t n = 0;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (is_inside_beam(tape, x, y))
                n++;
        }
    }

    std::cout << "Part 1: " << n << std::endl;
}

bool is_square_inside_beam(const Tape& tape, int x, int y, int width) {
    return is_inside_beam(tape, x + width, y) && is_inside_beam(tape, x, y + width);
}

int find_min_y(const Tape& tape, int x, int square_width, int search_width) {
    for (int y = 0; y < search_width; y++) {
        if (is_square_inside_beam(tape, x, y, square_width)) {
            return y;
        }
    }
    return -1;
}

void part2(const Tape& tape) {
    std::stringbuf buf;
    std::istream in(&buf);
    std::ostream out(&buf);

    const int square_width = 100 - 1;
    const int search_width = 5000;

    // We'll do a binary search on the x axis, with a linear scan on
    // the y axis for each tentative x value
    int x_min = 0, x_max = search_width;

    while (x_min != x_max) {
        int x_middle = (x_min + x_max) / 2;

        std::cout << "Trying x_middle: " << x_middle << std::endl;

        if (find_min_y(tape, x_middle, square_width, search_width) != -1) {
            x_max = x_middle;
        } else {
            x_min = x_middle + 1;
        }
    }

    int y = find_min_y(tape, x_min, square_width, search_width);

    std::cout << "Part 2: (" << x_min << ", " << y << ") => " << 10000 * x_min + y << std::endl;
}

int main() {
    const Tape tape = read_tape_from_disk("input.txt");

    part1(tape);

    part2(tape);
}
