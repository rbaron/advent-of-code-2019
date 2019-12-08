#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <unordered_map>
#include <vector>


using layer_t = std::vector<int>;
using frequency_t = std::unordered_map<int,int>;

void print_layer(const layer_t& layer, const int width, const int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            std::cout << (layer[y*width + x] == 1 ? '#' : ' ');
        }
        std::cout << std::endl;
    }
}

std::vector<layer_t> parse_layers(std::istream& in, int width, int height) {
    std::vector<layer_t> layers;
    char c;
    while (in.peek() != EOF) {
        layers.emplace_back();
        layer_t &layer = layers.back();
        layer.reserve(width * height);
        for (int i = 0; i < width * height; i++) {
            in >> c;
            layer.push_back(c - '0');
        }
    }
    return layers;
}

frequency_t layer_frequency(const layer_t& layer, int width, int height) {
    frequency_t freq;
    for (auto digit : layer) {
        freq[digit]++;
    }
    return freq;
}

void part1(const std::vector<layer_t>& layers, const int width, const int height) {
    std::vector<frequency_t> freqs;
    std::transform(
        layers.begin(),
        layers.end(),
        std::back_inserter(freqs),
        [width, height](const layer_t& layer) { return layer_frequency(layer, width, height); }
    );

    auto layer = *std::min_element(
        freqs.begin(),
        freqs.end(),
        [](frequency_t& f1, frequency_t& f2) {
            return f1[0] < f2[0];
        }
    );
    std::cout << "Part 1: " << layer[1] * layer[2] << std::endl;

}

inline int draw_pixel(const int old, const int curr) {
    return curr == 2 ? old : curr;
}

void part2(const std::vector<layer_t>& layers, const int width, const int height) {
    auto canvas = std::accumulate(
        layers.rbegin(),
        layers.rend(),
        layer_t(width * height, 0),
        [width, height](layer_t& acc, const layer_t& current) {
            for (int i = 0; i < width * height; i++) {
                acc[i] = draw_pixel(acc[i], current[i]);
            }
            return acc;
        }
    );

    std::cout << "Part 2:\n";
    print_layer(canvas, width, height);
}

int main() {
    const int width = 25;
    const int height = 6;

    std::ifstream file("input.txt");

    std::vector<layer_t> layers = parse_layers(file, width, height);

    part1(layers, width, height);
    part2(layers, width, height);
}