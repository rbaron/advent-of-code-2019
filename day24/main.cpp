#include <bitset>
#include <fstream>
#include <iostream>
#include <unordered_set>

constexpr size_t GridWidth = 5,
                 GridHeight = 5,
                 GridSize = GridWidth * GridHeight;

using grid_t = std::bitset<GridSize>;

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

grid_t evolve(const grid_t& grid) {
    grid_t new_grid;
    for (size_t y = 0; y < GridHeight; y++)
        for (size_t x = 0; x < GridWidth; x++)
            new_grid.set(grid_index(y, x), evolve_cell(y, x, grid));
    return new_grid;

}

void print_grid(const grid_t& grid) {
    for (size_t y = 0; y < GridHeight; y++) {
        for (size_t x = 0; x < GridWidth; x++)
            std::cout << (grid.test(grid_index(y, x)) ? '#' : '.');
        std::cout << std::endl;
    }
}

int main() {
    auto grid = parse_input("input.txt");
    std::unordered_set<grid_t> seen;
    while (true) {
        seen.insert(grid);

        grid = evolve(grid);

        if (seen.count(grid) > 0) {
            std::cout << "Part 1: " << grid.to_ulong() << std::endl;
            return 0;
        }
    }
}