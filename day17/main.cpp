#include <algorithm>
#include <iostream>
#include <map>
#include <numeric>
#include <vector>

#include "cpu.hpp"

using coord_t = std::pair<int,int>;
using grid_t = std::map<coord_t,char>;

coord_t operator+(const coord_t& lhs, const coord_t& rhs) {
    return {lhs.first + rhs.first, lhs.second + rhs.second};
}

std::vector<coord_t> neighbors{
    {-1, 0}, {0, 1}, {1, 0}, {0, -1}
};

grid_t parse_grid(std::istream& in) {
    grid_t grid;
    int i, x = 0, y = 0;
    while (in >> i) {
        if (i == '\n') { y++; x = 0; }
        else { grid[{y,x++}] = i; }
    }

    return grid;
}

inline bool is_grid(char c) {
    return c == '#' || c == '>' || c == '<' || c == '^' || c == 'v';
}

inline bool is_intersection(const grid_t& grid, coord_t pos) {
    return is_grid(grid.at(pos)) && std::all_of(
        neighbors.begin(),
        neighbors.end(),
        [&pos, &grid](const coord_t& neighbor) {
            auto cell = pos + neighbor;
            return grid.count(cell) > 0 ? is_grid(grid.at(cell)) : false;
        }
    );
}

void part1(Tape tape) {
    std::stringbuf buf;
    std::istream in(&buf);
    std::ostream out(&buf);

    CPU cpu(tape, in, out);

    cpu.run_program();

    auto grid = parse_grid(in);

    auto sum = std::accumulate(
        grid.begin(),
        grid.end(),
        0,
        [&grid](int sum, std::pair<coord_t,char> pos) {
            return is_intersection(grid, pos.first) ? sum + pos.first.first * pos.first.second : sum;
        }
    );

    std::cout << "Part 1: " << sum << std::endl;
}

void part2(Tape tape) {
    std::stringbuf buf;
    std::istream in(&buf);
    std::ostream out(&buf);

    tape[0] = 2;
    CPU cpu(tape, in, out);


    // Using attempt #2 from part2-manual-solve.txt
    std::string code(R"(A,B,B,C,A,B,C,A,B,C
L,6,R,12,L,4,L,6
R,6,L,6,R,12
L,6,L,10,L,10,R,6
n
)");

    for (auto c : code) out << static_cast<int>(c) << std::endl;

    cpu.run_program();
    int i;
    while(in >> i) {
        std::cout << static_cast<char>(i);
    }

    std::cout << std::endl << "Part 2: score: " << i << std::endl;
}

int main() {
    const Tape tape = read_tape_from_disk("input.txt");

    part1(tape);
    part2(tape);
}