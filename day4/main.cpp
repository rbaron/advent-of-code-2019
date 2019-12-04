#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

using us_int =  unsigned short;
using digit_count = std::pair<us_int,us_int>;

inline std::vector<digit_count> run_length_encode(int number) {
    std::vector<digit_count> ret;
    ret.reserve(std::ceil(std::log10(number)));
    us_int curr_digit = number % 10;
    us_int occurrences = 1;
    number /= 10;
    while (number > 0) {
        us_int new_digit = number % 10;
        if (new_digit == curr_digit) occurrences++;
        else {
            ret.push_back(digit_count(curr_digit, occurrences));
            curr_digit = new_digit;
            occurrences = 1;
        }
        number /= 10;
    }
    ret.push_back(digit_count(curr_digit, occurrences));
    return ret;
}

inline bool is_increasing(const std::vector<digit_count> counts) {
    // Reverse iterators since we store the counts from right to left
    return std::is_sorted(counts.rbegin(), counts.rend());
}

int main() {
    int n_part1 = 0, n_part2 = 0;
    int lo = 125730;
    int hi = 579381;
    for (auto pw = lo; pw <= hi; pw++) {
        auto counts = run_length_encode(pw);

        if (!is_increasing(counts)) continue;

        // Part 1
        if (std::any_of(counts.begin(), counts.end(), [](digit_count c){ return c.second >= 2; })) {
            n_part1++;
        }

        // Part 2
        if (std::any_of(counts.begin(), counts.end(), [](digit_count c){ return c.second == 2; })) {
            n_part2++;
        }
    }
    std::cout << "Part 1: " << n_part1 << std::endl;
    std::cout << "Part22: " << n_part2 << std::endl;
}