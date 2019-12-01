#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>

int calc_fuel_rec(int mass) {
    int fuel = mass / 3 - 2;
    return fuel <= 0 ? 0 : fuel + calc_fuel_rec(fuel);
}

int main() {
    std::ifstream file("input.txt");

    // Part 1
    auto fuel_part1 = std::accumulate(
        std::istream_iterator<int>(file),
        std::istream_iterator<int>(),
        0,
        [](int acc, int mass){ return acc + mass / 3 - 2; }
    );
    std::cout << fuel_part1 << std::endl;

    // Part 2
    file.clear();
    file.seekg(0, std::ios::beg);
    auto fuel_part2 = std::accumulate(
        std::istream_iterator<int>(file),
        std::istream_iterator<int>(),
        0,
        [](int acc, int mass) {
            return acc + calc_fuel_rec(mass);
        }
    );

    std::cout << fuel_part2 << std::endl;
}