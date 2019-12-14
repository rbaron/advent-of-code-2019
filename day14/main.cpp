#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <regex>
#include <unordered_map>
#include <queue>


using term_t = std::pair<std::string,int>;
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

int main() {
    auto reactions = parse_input("input.txt");
    std::unordered_map<std::string, Reaction> tree;
    for (auto r : reactions)
        tree[r.out.first] = r;

    std::queue<term_t> nodes;
    nodes.push({"FUEL", 1});

    int used_ore = 0;
    std::unordered_map<std::string, int> excess;

    while (!nodes.empty()) {
        auto node = nodes.front();
        nodes.pop();

        std::string element = node.first;
        int quantity = node.second;

        std::cout << "Making " << quantity << " of " << element << std::endl;;

        if (element == "ORE") {
            used_ore += quantity;
        } else {
            int uses_excess = std::min(excess[element], quantity);
            excess[element] -= uses_excess;
            quantity -= uses_excess;

            if (quantity == 0) continue;

            auto r = tree.at(element);
            int each_produces = r.out.second;

            int times = quantity / each_produces;
            if (quantity % each_produces > 0) times++;

            std::cout << "Will run the reaction producing " << element << " " << times << " times" << std::endl;

            excess[element] = each_produces * times - quantity;

            for (const auto& in : r.ins) {
                nodes.push({in.first, times * in.second});
            }
        }

    }

    std::cout << "Used ORE: " << used_ore << std::endl;
}