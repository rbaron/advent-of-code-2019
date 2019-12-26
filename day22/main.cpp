#include <algorithm>
#include <bitset>
#include <fstream>
#include <iostream>
#include <numeric>
#include <regex>
#include <vector>
#include <unordered_set>

#include <boost/multiprecision/cpp_int.hpp>

using deck_t = std::vector<uint64_t>;

using big_int = boost::multiprecision::cpp_int;
// using big_int = __int128_t;

// Represents a congruence y = ax + b (mod N)
struct Eq {
    big_int a;
    big_int b;
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
    return Eq{-1, size - 1};
}

Eq inv_cut(size_t n, size_t size) {
    return Eq{1, n + size};
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
    return Eq{(x + size) % size, 0};
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

big_int apply(const Eq& eq, big_int x, size_t size) {
    return (eq.a * x + eq.b + size) % size;
}

Eq map_inv_position(size_t size, const std::string& method) {
    const static std::regex \
        dins("deal into new stack"), \
        c("cut (-?\\d+)"),
        dwi("deal with increment (-?\\d+)");

    const static std::regex pattern1("deal into new stack");
    std::smatch matches;

    if (std::regex_search(method.begin(), method.end(), matches, dins)) {
        return inv_deal_into_new_stack(size);
    } else if (std::regex_search(method.begin(), method.end(), matches, dwi)) {
        return inv_deal_with_increment(std::stoi(matches[1]), size);
    } else if (std::regex_search(method.begin(), method.end(), matches, c)) {
        return inv_cut(std::stoi(matches[1]), size);
    } else {
        throw std::runtime_error("Unknown suffle method: " + method);
    }
}

Eq compose(const Eq& f, const Eq& g, size_t size) {
    // fog(x) = a1(a2x + b2) + b1 = a1a2x + a1b2 + b1 (mod N)
    return {
        (f.a * g.a + size) % size,
        (f.a * g.b + f.b + size) % size
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
    big_int pos = 6850;

    std::fstream file("input.txt");
    std::string line;
    while (std::getline(file, line)) methods.push_back(line);

    Eq eq{1,0};
    for (auto it = methods.rbegin(); it != methods.rend(); it++) {
        eq = compose(map_inv_position(size, *it), eq, size);
    }

    std::cout << "Part 2: " << static_cast<uint64_t>(apply(eq, pos, size)) << std::endl;
}

int main() {
    part1();
    part2();
}