#include <algorithm>
#include <bitset>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <queue>
#include <unordered_set>
#include <vector>
#include <set>

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

void print_grid(const grid_t& grid, const int height, const int width) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            std::cout << grid.at({y,x});
        }
            std::cout << std::endl;
    }
}

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

/*
    {
        b: 12,
        c: 25,
        ...
    }
*/
using steps_by_target_t = std::unordered_map<char,size_t>;
steps_by_target_t get_steps_by_target(const grid_t& grid, const coord_t& origin) {
    steps_by_target_t targets;

    std::unordered_set<coord_t,pair_hash> visited;
    std::queue<std::tuple<coord_t,int>> nodes;
    nodes.push({origin, 0});

    while (!nodes.empty()) {
        auto node = nodes.front();
        nodes.pop();

        auto coord = std::get<0>(node);
        auto steps = std::get<1>(node);

        if (visited.count(coord) > 0) continue;

        visited.insert(coord);

        auto c = get_grid_char(grid, coord);

        if (c != '#' && c != '.' && c != grid.at(origin)) {
            targets[c] = steps;
        }

        if (c == '.' || c == grid.at(origin)) {
            for (const auto& cell : neighboring_cells) {
                auto neighbor = coord + cell;
                nodes.push({neighbor, steps + 1});
            }
        }
    }
    return targets;
}

/*
    {
        a: {
            b: 12,
            c: 25,
            ...
        },
        ...
    }
*/
using steps_by_target_by_target_t = std::unordered_map<char,std::unordered_map<char,size_t>>;
steps_by_target_by_target_t make_steps_by_steps_by_target(const grid_t& grid) {
    steps_by_target_by_target_t map;

    for (const auto& kv : grid) {
        auto pos = kv.first;
        auto c = kv.second;

        if (c != '#' && c != '.') {
            map[c] = get_steps_by_target(grid, pos);
        }
    }
    return map;
}

void print_step_map(const steps_by_target_by_target_t& map) {
    for (const auto& kv : map) {
        auto origin = kv.first;
        auto targets = kv.second;

        std::cout << origin << " => ";
        for (const auto& t : targets) {
            auto dest = t.first;
            auto dest_steps = t.second;
            std::cout << dest  << " (" << dest_steps << "), ";
        }
        std::cout << std::endl;
    }
}

struct node_t {
    char label;
    size_t steps;
    std::bitset<26> keys;

    bool has_key(char key) {
        return keys.test(key - 'a');
    }

    void set_key(char key) {
        keys.set(key - 'a');
    }
};

struct node_hash {
    size_t operator () (const node_t& node) const {
        return std::hash<char>{}(node.label) ^ std::hash<std::bitset<26>>{}(node.keys);
    }
};

struct heap_comp {
    unsigned long operator () (const node_t& a, const node_t& b) const {
        // > because we want a min heap
        return a.steps > b.steps;
    }
};

bool operator==(const node_t& lhs, const node_t& rhs) {
    return lhs.label == rhs.label && lhs.keys == rhs.keys;
}

std::ostream& operator<<(std::ostream& out, const node_t& node) {
    out << "Node (steps=" << node.steps << ", label=" << node.label;
    out <<  ", keys=" << node.keys.to_string() << " [" << node.keys.count() << "])";
    return out;
}

void part1(const grid_t& grid, const coord_t& origin) {
    auto step_map = make_steps_by_steps_by_target(grid);

    std::unordered_set<node_t,node_hash> seen;

    heap_comp heap_comp_fn;

    // Heap
    std::vector<node_t> nodes;
    nodes.push_back(node_t{'@', 0, {}});

    int n_keys = std::count_if(
        step_map.begin(),
        step_map.end(),
        [](const std::pair<char,steps_by_target_t>& pair) {
            return std::islower(pair.first);
        }
    );

    while (!nodes.empty()) {
        std::pop_heap(nodes.begin(), nodes.end(), heap_comp_fn);
        auto node = nodes.back();
        nodes.pop_back();

        if (node.keys.count() == n_keys) {
            std::cout << "Part 1: " << node.steps << std::endl;
            return;
        }

        if (seen.count(node) > 0) {
            continue;
        }

        seen.insert(node);

        for (const auto& kv : step_map.at(node.label)) {
            auto dest = kv.first;
            auto dest_steps = kv.second;

            if (std::isupper(dest)) {
                if (node.has_key(std::tolower(dest)) == 0) {
                    continue;
                } else {
                    nodes.push_back(node_t{dest, node.steps + dest_steps, node.keys});
                    std::push_heap(nodes.begin(), nodes.end(), heap_comp_fn);
                }
            } else if (std::islower(dest)) {
                auto new_node(node);
                new_node.label = dest;
                new_node.steps += dest_steps;
                new_node.set_key(dest);
                nodes.push_back(new_node);
                std::push_heap(nodes.begin(), nodes.end(), heap_comp_fn);
            } else if (dest == '@') {
                nodes.push_back({dest, node.steps + dest_steps, node.keys });
                std::push_heap(nodes.begin(), nodes.end(), heap_comp_fn);
            }
        }
    }
}

int main() {
    auto grid_pos = parse_input("input.txt");
    auto grid = grid_pos.first;
    auto origin = grid_pos.second;

    part1(grid, origin);
}