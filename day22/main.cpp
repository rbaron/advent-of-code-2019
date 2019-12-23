#include <algorithm>
#include <bitset>
#include <fstream>
#include <iostream>
#include <numeric>
#include <regex>
#include <vector>
#include <unordered_set>

using deck_t = std::vector<uint64_t>;

size_t deal_into_new_stack(size_t position, size_t size) {
    return size - 1 - position;
}

size_t cut(size_t n, size_t position, size_t size) {
    return (position - n + size) % size;
}

size_t deal_with_increment(size_t n, size_t position, size_t size) {
    return (position * n) % size;
}

size_t map_position(size_t position, size_t size, const std::string& method) {
    const static std::regex \
        dins("deal into new stack"), \
        c("cut (-?\\d+)"),
        dwi("deal with increment (-?\\d+)");

    const static std::regex pattern1("deal into new stack");
    std::smatch matches;

    if (std::regex_search(method.begin(), method.end(), matches, dins)) {
        return deal_into_new_stack(position, size);
    } else if (std::regex_search(method.begin(), method.end(), matches, dwi)) {
        return deal_with_increment(std::stoi(matches[1]), position, size);
    } else if (std::regex_search(method.begin(), method.end(), matches, c)) {
        return cut(std::stoi(matches[1]), position, size);
    } else {
        throw std::runtime_error("Unknown suffle method: " + method);
    }
}

void part1() {
    constexpr size_t size = 10007;
    size_t pos = 2019;

    std::fstream file("input.txt");
    std::string line;

    while (std::getline(file, line)) pos = map_position(pos, size, line);

    std::cout << "Part 1: " << pos << std::endl;
}

int main() {
    part1();
}