#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <vector>


const std::vector<int> base_pattern = { 0, 1, 0, -1 };

std::vector<int> read_input(const std::string& filename) {
    std::ifstream file(filename);
    return std::accumulate(
        std::istream_iterator<char>(file),
        std::istream_iterator<char>(),
        std::vector<int>(),
        [](std::vector<int>& out, char c) { out.push_back(c - '0'); return out; }
    );
}

const std::vector<int> generate_pattern(int input_size, int position) {
    std::vector<int> pattern;
    pattern.reserve(input_size);
    while (true)
        for (auto pattern_char : base_pattern)
            for (int i = 0; i < position + 1; i++) {
                if (pattern.size() == input_size) return pattern;
                if (pattern_char == base_pattern[0] && pattern.size() == 0 && i == 0) continue;
                pattern.push_back(pattern_char);
            }
    return pattern;
}

void run_phase1(std::vector<int>& input, std::vector<int>& output) {
    for (int i = 0; i < input.size(); i++) {
        auto pattern = generate_pattern(input.size(), i);
        output[i] = std::abs(
            std::inner_product(input.begin(), input.end(), pattern.begin(), 0) % 10
        );
    }
}

void run_phase2(std::vector<int>& input, std::vector<int>& output) {
    size_t sum  = std::accumulate(input.begin(), input.end(), 0);
    for (int i = 0; i < output.size(); i++) {
        output[i] = sum % 10;
        sum -= input[i];
    }
}

void part1(const std::vector<int>& input, const int phases) {
    std::vector<int> in(input), out(input.size());

    for (int i = 0; i < phases; i++) {
        if (i % 2 == 0) {
            run_phase1(in, out);
        } else {
            run_phase1(out, in);
        }
    }

    std::cout << "Part 1: ";
    for (int i = 0; i < 8; i++) std::cout << in[i];
    std::cout << std::endl;
}

void part2(const std::vector<int>& input, const int phases) {
    const size_t idx = 5971751;
    const size_t total_size = 10000 * input.size();

    std::vector<int> in(total_size - idx), out(total_size - idx);
    for (size_t i = idx; i < total_size; i++) {
        in[i - idx] = input[i % input.size()];
    }

    for (int i = 0; i < phases; i++) {
        if (i % 2 == 0) {
            run_phase2(in, out);
        } else {
            run_phase2(out, in);
        }
    }

    std::cout << "Part 2: ";
    for (int i = 0; i < 8; i++) std::cout << in[i];
    std::cout << std::endl;
}

int main() {
    const int phases = 100;
    const auto input = read_input("input.txt");

    part1(input, phases);
    part2(input, phases);
}