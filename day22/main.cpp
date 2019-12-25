#include <algorithm>
#include <bitset>
#include <fstream>
#include <iostream>
#include <numeric>
#include <regex>
#include <vector>
#include <unordered_set>

using deck_t = std::vector<uint64_t>;

// Represents a congruence y = ax + b (mod N)
struct Eq {
    size_t a;
    size_t b;
    int a_sign;
};

size_t deal_into_new_stack(size_t position, size_t size) {
    return size - 1 - position;
}

size_t cut(size_t n, size_t position, size_t size) {
    return (position - n + size) % size;
}

size_t deal_with_increment(size_t n, size_t position, size_t size) {
    return (position * n) % size;
}

Eq inv_deal_into_new_stack(size_t size) {
    return Eq{1, size - 1, -1};
}

Eq inv_cut(size_t n, size_t size) {
    return Eq{1, + n + size, 1};
}

// From https://cp-algorithms.com/algebra/extended-euclid-algorithm.html
int egcd(int a, int b, int & x, int & y) {
    if (a == 0) {
        x = 0;
        y = 1;
        return b;
    }
    int x1, y1;
    int d = egcd(b % a, a, x1, y1);
    x = y1 - (b / a) * x1;
    y = x1;
    return d;
}

Eq inv_deal_with_increment(size_t n, size_t size) {
    int x, y;
    egcd(n, size, x, y);
    return Eq{(x + size) % size, 0, 1};
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

size_t apply(const Eq& eq, size_t x, size_t size) {
    return (eq.a_sign * eq.a * x + eq.b) % size;
}

size_t map_inv_position(size_t position, size_t size, const std::string& method) {
    const static std::regex \
        dins("deal into new stack"), \
        c("cut (-?\\d+)"),
        dwi("deal with increment (-?\\d+)");

    const static std::regex pattern1("deal into new stack");
    std::smatch matches;

    if (std::regex_search(method.begin(), method.end(), matches, dins)) {
        return apply(inv_deal_into_new_stack(size), position, size);
    } else if (std::regex_search(method.begin(), method.end(), matches, dwi)) {
        return apply(inv_deal_with_increment(std::stoi(matches[1]), size), position, size);
    } else if (std::regex_search(method.begin(), method.end(), matches, c)) {
        return apply(inv_cut(std::stoi(matches[1]), size), position, size);
    } else {
        throw std::runtime_error("Unknown suffle method: " + method);
    }
}

Eq compose(const Eq& f, const Eq& g, size_t size) {
    // fog(x) = a1(a2x + b2) + b1 = a1a2x + a1b2 + b1 (mod N)
    return {
        f.a_sign * f.a * g.a_sign * g.a,
        f.a_sign * f.a * g.b + f.b,
        f.a_sign * g.a_sign
    };
}

void part1() {
    constexpr size_t size = 10007;
    size_t pos = 2019;

    std::fstream file("input.txt");
    std::string line;

    while (std::getline(file, line)) {
        pos = map_position(pos, size, line);
    }

    std::cout << "Part 1: " << pos << std::endl;
}

void part2() {
    std::vector<std::string> methods;

    constexpr size_t size = 10007;
    size_t pos = 6850;

    std::fstream file("input.txt");
    std::string line;
    while (std::getline(file, line)) methods.push_back(line);

    for (auto it = methods.rbegin(); it != methods.rend(); it++) {
        pos = map_inv_position(pos, size, *it);
    }

    std::cout << "Part 2: " << pos << std::endl;
}

int main() {
    part1();
    part2();
}