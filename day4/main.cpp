#include <cmath>
#include <iostream>

// Part 1
inline bool is_passwd_valid1(int passwd) {
    if (std::ceil(std::log10(passwd)) != 6) return false;

    bool has_adjacent_same_digit = false;
    unsigned short curr_last_digit, old_last_digit = passwd % 10;
    passwd /= 10;

    while (passwd > 0) {
        curr_last_digit = passwd % 10;
        if (curr_last_digit > old_last_digit) return false;
        if (curr_last_digit == old_last_digit) has_adjacent_same_digit = true;
        old_last_digit = curr_last_digit;
        passwd /= 10;
    }
    return has_adjacent_same_digit;
}

// Part 2
inline bool is_passwd_valid2(int passwd) {
    if (std::ceil(std::log10(passwd)) != 6) return false;

    bool has_adjacent_double_digits = false;
    unsigned short curr_last_digit;
    unsigned short old_last_digit = passwd % 10;
    unsigned short current_group_size = 1;
    passwd /= 10;

    while (passwd > 0) {
        curr_last_digit = passwd % 10;
        if (curr_last_digit > old_last_digit) return false;
        if (curr_last_digit == old_last_digit) {
            current_group_size++;
        } else {
            if (current_group_size == 2) has_adjacent_double_digits = true;
            current_group_size = 1;
        }
        old_last_digit = curr_last_digit;
        passwd /= 10;
    }

    if (current_group_size == 2) has_adjacent_double_digits = true;
    return has_adjacent_double_digits;
}

int main() {
    int n_part1 = 0, n_part2 = 0;
    int lo = 125730;
    int hi = 579381;
    for (auto i=lo; i<=hi; i++) {
        if (is_passwd_valid1(i)) n_part1++;
        if (is_passwd_valid2(i)) n_part2++;
    }
    std::cout << n_part1 << std::endl;
    std::cout << n_part2 << std::endl;
}