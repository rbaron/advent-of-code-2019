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

const std::vector<int> run_phase(const std::vector<int>& input) {
    std::vector<int> out(input.size());

    for (int i = 0; i < input.size(); i++) {
        auto pattern = generate_pattern(input.size(), i);
        out[i] = std::abs(
            std::inner_product(input.begin(), input.end(), pattern.begin(), 0) % 10
        );
    }
    return out;
}
int main() {
    const int phases = 100;
    const auto input = read_input("input.txt");
    // for (auto c : input) std::cout << c << std::endl;

    auto out(input);
    for (int i = 0; i < phases; i++) {
        out = run_phase(out);
    }

    // Part 1
    std::cout << "Part 1: ";
    for (int i = 0; i < 8; i++) std::cout << out[i];
    std::cout << std::endl;
}