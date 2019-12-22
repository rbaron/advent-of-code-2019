#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <regex>
#include <vector>

using deck_t = std::vector<short int>;

void deal_into_new_stack(deck_t& deck) {
    auto size = deck.size();
    for (int i = 0; i < size / 2; i++)
        std::swap(deck[i], deck[size - 1- i]);
}

void cut(deck_t& deck, int n) {
    auto copy(deck);
    auto size = deck.size();
    auto i = (n + size) % size;
    std::copy_n(copy.begin() + i, size - i, deck.begin());
    std::copy_n(copy.begin(), i, deck.begin() + size - i);
}

void deal_with_increment(deck_t& deck, int n) {
    auto copy(deck);
    auto size = deck.size();
    for (int i = 0; i < size; i++)
        deck[(i * n) % size] = copy[i];
}

void print_deck(const deck_t& deck) {
    for (const auto& card : deck)
        std::cout << card << ", ";
    std::cout << std::endl;
}

void shuffle(deck_t& deck, const std::string& method) {
    const static std::regex \
        dins("deal into new stack"), \
        c("cut (-?\\d+)"),
        dwi("deal with increment (-?\\d+)");

    const static std::regex pattern1("deal into new stack");
    std::smatch matches;

    if (std::regex_search(method.begin(), method.end(), matches, dins)) {
        deal_into_new_stack(deck);
    } else if (std::regex_search(method.begin(), method.end(), matches, dwi)) {
        deal_with_increment(deck, std::stoi(matches[1]));
    } else if (std::regex_search(method.begin(), method.end(), matches, c)) {
        cut(deck, std::stoi(matches[1]));
    } else {
        throw std::runtime_error("Unknown suffle method: " + method);
    }

}

int main() {
    deck_t deck(10007);
    std::iota(deck.begin(), deck.end(), 0);

    std::fstream file("input.txt");
    std::string line;
    while (std::getline(file, line)) shuffle(deck, line);

    auto it = find(deck.begin(), deck.end(), 2019);
    std::cout << "Part 1: " << std::distance(deck.begin(), it) << std::endl;
}