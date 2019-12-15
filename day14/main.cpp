#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <regex>
#include <unordered_map>
#include <queue>


using term_t = std::pair<std::string,long long int>;
struct Reaction {
    term_t out;
    std::vector<term_t> ins;
};

Reaction parse_line(const std::string& line) {
    std::vector<term_t> terms;
    std::regex pattern("(\\d+) (\\w+)");
    std::smatch matches;
    std::string::const_iterator start = line.begin();

    while (std::regex_search(start, line.end(), matches, pattern)) {
        start = matches.suffix().first;
        terms.push_back({matches[2], std::stoi(matches[1])});
    }
    std::vector<term_t> ins;
    std::copy_n(terms.begin(), terms.size() - 1, std::back_inserter(ins));
    Reaction r;
    r.out = terms.back();
    r.ins = std::move(ins);
    return r;
}

std::vector<Reaction> parse_input(const std::string& filename) {
    std::vector<Reaction> reactions;
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        reactions.push_back(parse_line(line));
    }
    return reactions;
}

long long int ore_cost(const std::unordered_map<std::string, Reaction>& tree, long long int fuel) {
    std::queue<term_t> nodes;
    nodes.push({"FUEL", fuel});

    long long int used_ore = 0;
    std::unordered_map<std::string, long long int> excess;

    while (!nodes.empty()) {
        auto node = nodes.front();
        nodes.pop();

        std::string element = node.first;
        auto quantity = node.second;

        // std::cout << "Making " << quantity << " of " << element << std::endl;;

        if (element == "ORE") {
            used_ore += quantity;
        } else {
            auto uses_excess = std::min(excess[element], quantity);
            excess[element] -= uses_excess;
            quantity -= uses_excess;

            if (quantity == 0) continue;

            auto r = tree.at(element);
            auto each_produces = r.out.second;

            auto times = quantity / each_produces;
            if (quantity % each_produces > 0) times++;

            // std::cout << "Will run the reaction producing " << element << " " << times << " times" << std::endl;

            excess[element] = each_produces * times - quantity;

            for (const auto& in : r.ins) {
                nodes.push({in.first, times * in.second});
            }
        }

    }
    return used_ore;
}

int main() {
    auto reactions = parse_input("input.txt");
    std::unordered_map<std::string, Reaction> tree;
    for (auto r : reactions)
        tree[r.out.first] = r;

    // Part 1
    std::cout << "Part 1: " << ore_cost(tree, 1) << std::endl;

    // Part 2
    const long long int available_ore = 1000000000000;

    long long int lo = 0, hi = available_ore;
    while (lo < hi) {
        auto middle = (lo + hi) / 2;
        if (lo == middle) break;

        if (ore_cost(tree, middle) > available_ore) {
            hi = middle;
        } else {
            lo = middle;
        }
    }

    std::cout << "Part 2: " << lo << " ore produces " << ore_cost(tree, lo) << " fuel" << std::endl;
}