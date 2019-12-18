#include <fstream>
#include <iostream>
#include <unordered_map>
#include <queue>
#include <unordered_set>
#include <vector>

// So we can use std::pair<int,int>s as unordered_{map,set} keys
struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1,T2> &p) const {
        return std::hash<T1>{}(p.first) ^ std::hash<T2>{}(p.second);
    }
};

using coord_t = std::pair<int,int>;
using grid_t = std::unordered_map<coord_t,char,pair_hash>;

coord_t operator+(const coord_t& lhs, const coord_t& rhs) {
    return {lhs.first + rhs.first, lhs.second + rhs.second};
}

std::ostream& operator<<(std::ostream &out, const coord_t& coord) {
    out << "(" << coord.first << ", " << coord.second << ")";
    return out;
}

const std::vector<coord_t> neighboring_cells = {
    {-1,0}, {0,1}, {1,0}, {0,-1}
};

std::pair<grid_t,coord_t> parse_input(const std::string& filename) {
    std::ifstream file(filename);
    grid_t grid;
    coord_t initial_pos;
    std::string line;
    int x = 0, y = 0;
    while (std::getline(file, line)) {
        for (auto c : line) {
            if (c == '@') {
                initial_pos = {y,x};
                c = '.';
            }
            grid[{y,x++}] = c;
        }
        y++;
        x = 0;
    }
    return {grid, initial_pos};
}

inline char get_grid_char(const grid_t& grid, const coord_t& coord) {
    return grid.count(coord) > 0 ? grid.at(coord) : '#';
}

std::vector<std::pair<coord_t,int>> get_possible_targets(const grid_t& grid, const coord_t& origin, const std::unordered_set<char>& keys) {
    std::vector<std::pair<coord_t,int>> targets;

    std::unordered_set<coord_t,pair_hash> visited;
    std::queue<std::pair<coord_t,int>> nodes;
    nodes.push({origin, 0});

    while (!nodes.empty()) {
        auto node = nodes.front();
        nodes.pop();

        auto coord = node.first;
        auto steps = node.second;

        if (visited.count(coord) > 0) continue;

        visited.insert(coord);

        for (const auto& cell : neighboring_cells) {
            auto neighbor = coord + cell;
            // std::cout << "node: " << node << std::endl;
            // std::cout << "cell: " << cell << std::endl;
            // std::cout << "neigh: " << neighbor << std::endl;
            auto c = get_grid_char(grid, neighbor);

            if (c == '#') {
                continue;
            } else if (c == '.') {
                nodes.push({neighbor, steps + 1});
            } else if (std::islower(c)) {
                // If we already visited this, we treat it like '.'
                if (keys.count(std::tolower(c)) > 0) {
                    nodes.push({neighbor, steps + 1});

                // Otherwise it's a valid target
                } else {
                    // std::cout << "Found nei " << c << " @ " << neighbor << std::endl;
                    targets.push_back({ neighbor, steps + 1});
                }

            } else if (std::isupper(c)) {
                // If we have the key to this gate
                if (keys.count(std::tolower(c)) > 0) {
                    nodes.push({neighbor, steps + 1});
                }
            }
        }
    }
    return targets;
}

void part1(const grid_t& grid, const coord_t& origin) {
    std::queue<std::tuple<coord_t,std::unordered_set<char>,size_t,std::string>> nodes;

    nodes.push({origin,{},0,"@"});

    while (!nodes.empty()) {
        auto node = nodes.front();
        nodes.pop();

        auto pos = std::get<0>(node);
        auto keys = std::get<1>(node);
        auto steps = std::get<2>(node);
        auto path = std::get<3>(node);

        // std::cout << "Node: " << grid.at(pos) << std::endl;
        // std::cout << "Steps: " << steps << std::endl;
        auto targets = get_possible_targets(grid, pos, keys);
        if (targets.empty()) std::cout << "Exhausted with steps: " << steps << std::endl;
        else {
            // std::cout << grid.at(pos) << " => " << path << " steps: " << steps << std::endl;
            for (const auto& target : get_possible_targets(grid, pos, keys)) {
                // std::cout << grid.at(target) << " ,";
                auto new_keys(keys);
                new_keys.insert(grid.at(target.first));
                nodes.push({target.first, new_keys, steps + target.second, path + grid.at(target.first)});
            }

            // std::cout << " keys: ";
            // for (auto& k : keys) std::cout << k << ", ";
            // std::cout << " steps: " << steps;
            // std::cout << std::endl;
        }
    }
}

void print_grid(const grid_t& grid, const int height, const int width) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            std::cout << grid.at({y,x});
        }
            std::cout << std::endl;
    }
}

int main() {
    auto grid_pos = parse_input("input.txt");
    auto grid = grid_pos.first;
    auto origin = grid_pos.second;

    part1(grid, origin);
}