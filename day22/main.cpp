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
big_int egcd(big_int a, big_int b, big_int& x, big_int& y) {
    if (a == 0) {
        x = 0;
        y = 1;
        return b;
    }
    big_int x1, y1;
    auto d = egcd(b % a, a, x1, y1);
    x = y1 - (b / a) * x1;
    y = x1;
    return d;
}

big_int inv(big_int n, big_int size) {
    big_int x, y;
    egcd(n, size, x, y);
    return (x + size) % size;
}

Eq inv_deal_with_increment(size_t n, size_t size) {
    return Eq{inv(n, size), 0};
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

// From https://en.wikipedia.org/wiki/Exponentiation_by_squaring#Basic_method
big_int exp(big_int x, big_int n, size_t size) {
    if (n == 0) return  1;
    else if (n == 1) return x;
    // n is even
    else if (n % 2 == 0) return exp(x * x % size, n / 2, size);
    // n is odd
    else return x * exp(x * x % size, (n - 1) / 2, size);
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

// See ./notes.txt for the rationale behind this
void part2() {
    std::vector<std::string> methods;

    constexpr size_t size = 119315717514047;
    constexpr size_t times = 101741582076661;
    big_int pos = 2020;

    std::fstream file("input.txt");
    std::string line;
    while (std::getline(file, line)) methods.push_back(line);

    Eq eq{1,0};
    for (auto it = methods.rbegin(); it != methods.rend(); it++) {
        eq = compose(map_inv_position(size, *it), eq, size);
    }

    auto term_1 = exp(eq.a, times, size);
    auto term_2 = inv(eq.a - 1, size);

    Eq e2{term_1, eq.b * (term_1 - 1) * term_2};
    std::cout << "Part 2: " << apply(e2, pos, size) % size << std::endl;
}

int main() {
    part1();
    part2();
}