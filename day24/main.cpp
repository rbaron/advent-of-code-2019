#include <algorithm>
#include <bitset>
#include <fstream>
#include <iostream>
#include <numeric>
#include <unordered_set>
#include <unordered_map>

constexpr size_t GridWidth = 5,
                 GridHeight = 5,
                 GridSize = GridWidth * GridHeight;

using grid_t = std::bitset<GridSize>;
using rec_grid_t = std::unordered_map<long int,grid_t>;

grid_t parse_input(const std::string& filename) {
    grid_t grid;

    std::ifstream file(filename);
    std::string line;
    size_t pos = 0;
    while(std::getline(file, line)) {
        for (auto c : line) {
            grid.set(pos, c == '#');
            pos++;
        }
    }
    return grid;
}

inline size_t grid_index(size_t y, size_t x) {
    return y * GridWidth + x;
}

bool evolve_cell(size_t y, size_t x, const grid_t& grid) {
    short int neighbors_alive = 0;
    if (x > 0 && grid.test(grid_index(y, x - 1))) neighbors_alive++;
    if (x < (GridWidth - 1) && grid.test(grid_index(y, x + 1))) neighbors_alive++;
    if (y > 0 && grid.test(grid_index(y - 1, x))) neighbors_alive++;
    if (y < (GridHeight - 1) && grid.test(grid_index(y + 1, x))) neighbors_alive++;

    auto is_bug = grid.test(grid_index(y, x));
    if (is_bug && neighbors_alive != 1) return false;
    else if (!is_bug && (neighbors_alive == 1 || neighbors_alive == 2)) return true;
    else return is_bug;
}

bool evolve_cell_rec(size_t y, size_t x, size_t level, rec_grid_t& rec_grid) {
    short int neighbors_alive = 0;
    const auto& grid = rec_grid[level];


    // Middle cell makes no sense anymore
    if (y == 2 && x == 2) return false;

    // Left neighbor

    // Level above
    if (x == 0 && rec_grid[level-1].test(grid_index(2, 1))) { neighbors_alive++; }
    // Level below
    else if (x == 3 && y == 2) {
        for (int ry = 0; ry < GridHeight; ry++)
            if (rec_grid[level+1].test(grid_index(ry, GridWidth-1))) neighbors_alive++;
    }
    // This level
    else if (x > 0 && grid.test(grid_index(y, x-1))) { neighbors_alive++; }

    // Right neighbor

    // Level above
    if (x == GridWidth - 1 && rec_grid[level-1].test(grid_index(2, 3))) { neighbors_alive++; }
    // Level below
    else if (x == 1 && y == 2) {
        for (int ry = 0; ry < GridHeight; ry++)
            if (rec_grid[level+1].test(grid_index(ry, 0))) neighbors_alive++;
    }
    // This level
    else if (x < (GridWidth - 1) && grid.test(grid_index(y, x+1))) { neighbors_alive++; }

    // Top neighbor

    // Level above
    if (y == 0 && rec_grid[level-1].test(grid_index(1, 2))) { neighbors_alive++; }
    // Level below
    else if (x == 2 && y == 3) {
        for (int rx = 0; rx < GridWidth; rx++)
            if (rec_grid[level+1].test(grid_index(GridHeight-1, rx))) neighbors_alive++;
    }
    // This level
    else { if (y > 0 && grid.test(grid_index(y-1, x))) neighbors_alive++; }

    // Bottom neighbor

    // Level above
    if (y == (GridHeight - 1) && rec_grid[level-1].test(grid_index(3, 2))) { neighbors_alive++; }
    // Level below
    else if (x == 2 && y == 1) {
        for (int rx = 0; rx < GridWidth; rx++)
            if (rec_grid[level+1].test(grid_index(0, rx))) neighbors_alive++;
    }
    // This level
    else if (y < (GridHeight - 1) && grid.test(grid_index(y+1, x))) { neighbors_alive++; }

    auto is_bug = grid.test(grid_index(y, x));
    if (is_bug && neighbors_alive != 1) return false;
    else if (!is_bug && (neighbors_alive == 1 || neighbors_alive == 2)) return true;
    else return is_bug;
}

grid_t evolve(const grid_t& grid) {
    grid_t new_grid;
    for (size_t y = 0; y < GridHeight; y++)
        for (size_t x = 0; x < GridWidth; x++)
            new_grid.set(grid_index(y, x), evolve_cell(y, x, grid));
    return new_grid;
}

rec_grid_t evolve_rec(rec_grid_t& rec_grid) {
    rec_grid_t new_rec_grid;

    std::unordered_set<long int> levels;
    for (const auto& kv : rec_grid) {
        auto level = kv.first;
        auto& grid = kv.second;
        levels.insert(level);
        if (grid.to_ulong() > 0) {
            levels.insert(level - 1);
            levels.insert(level + 1);
        }
    }

    for (auto level : levels) {
        for (size_t y = 0; y < GridHeight; y++)
            for (size_t x = 0; x < GridWidth; x++)
                new_rec_grid[level].set(grid_index(y, x), evolve_cell_rec(y, x, level, rec_grid));
    }
    return new_rec_grid;

}

void print_grid(const grid_t& grid) {
    for (size_t y = 0; y < GridHeight; y++) {
        for (size_t x = 0; x < GridWidth; x++)
            std::cout << (grid.test(grid_index(y, x)) ? '#' : '.');
        std::cout << std::endl;
    }
}

void part1(grid_t grid) {
    std::unordered_set<grid_t> seen;
    while (true) {
        seen.insert(grid);

        grid = evolve(grid);

        if (seen.count(grid) > 0) {
            std::cout << "Part 1: " << grid.to_ulong() << std::endl;
            return;
        }
    }
}

void part2(grid_t grid) {
    rec_grid_t rec_grid;
    rec_grid[0] = grid;

    for (int i = 0; i < 200; i++) {
        rec_grid = evolve_rec(rec_grid);
    }

    auto n_bugs = std::accumulate(
        rec_grid.begin(),
        rec_grid.end(),
        0,
        [](size_t acc, rec_grid_t::value_type& kv) { return acc + kv.second.count(); }
    );

    std::cout << "Part 2: " << n_bugs << std::endl;
}

int main() {
    const auto grid = parse_input("input.txt");

    part1(grid);
    part2(grid);
}