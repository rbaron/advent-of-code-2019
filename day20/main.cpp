#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct Coord {
    int x;
    int y;

    bool operator==(const Coord& lhs) const {
        return x == lhs.x && y == lhs.y;
    }
};

std::ostream& operator<<(std::ostream &out, const Coord& coord) {
    out << "(" << coord.x << ", " << coord.y << ")";
    return out;
}

Coord operator+(const Coord& lhs, const Coord& rhs) {
    return {rhs.x + lhs.x, rhs.y + lhs.y};
}

const std::vector<Coord> neighboring_cells{
    {-1,0}, {0,1}, {1,0}, {0,-1}
};

struct coord_hash {
    std::size_t operator() (const Coord& c) const {
        return std::hash<int>{}(c.x) ^ std::hash<int>{}(c.y);
    }
};

using Grid = std::unordered_map<Coord,std::string,coord_hash>;

bool is_gate(const std::string& str) {
    return str.length() == 2 && std::all_of(str.begin(), str.end(), [](char c) { return std::isupper(c); });
}

Grid parse_input(const std::string& filename) {
    std::fstream file(filename);

    Grid grid;
    std::vector<Coord> letters_coords;
    int x = 0, y = 0;

    std::string line;
    while (std::getline(file, line)) {
        x = 0;
        for (auto c : line) {
            if (c != '#') grid[{x,y}] = c;
            if (std::isupper(c)) letters_coords.push_back({x,y});
            x++;
        }
        y++;
    }

    // Adjust the gates labels
    for (const auto& c : letters_coords) {
        if (grid[{c.x, c.y-1}] == ".") {
            grid[{c.x, c.y-1}] = grid[c] + grid[{c.x,c.y+1}];
        } else if (grid[{c.x, c.y+1}] == ".") {
            grid[{c.x, c.y+1}] = grid[{c.x,c.y-1}] + grid[c];
        } else if (grid[{c.x-1, c.y}] == ".") {
            grid[{c.x-1, c.y}] = grid[c] + grid[{c.x+1,c.y}];
        } else if (grid[{c.x+1, c.y}] == ".") {
            grid[{c.x+1, c.y}] = grid[{c.x-1,c.y}] + grid[c];
        }
    }

    return grid;
}

std::vector<std::pair<std::string,int>> find_reachables(const Grid& grid, const Coord& origin) {
    std::vector<std::pair<std::string,int>> reachable;
    std::unordered_set<Coord,coord_hash> visited;
    std::queue<std::pair<Coord,int>> nodes;

    nodes.push({origin, 0});

    while (!nodes.empty()) {
        auto node = nodes.front();
        nodes.pop();

        auto pos = node.first;
        auto steps = node.second;

        if (grid.count(pos) == 0 || visited.count(pos) > 0) continue;
        visited.insert(pos);

        auto label = grid.at(pos);

        if (is_gate(label) && steps > 0) {
            reachable.push_back({label, steps});
        } else if (label == "." || steps == 0) {
            for (const auto& n : neighboring_cells) {
                nodes.push({pos + n, steps + 1});
            }
        }
    }

    return reachable;
}

/*
    `Graph` is an adjacency list like this:
    {
        posA: [(posB, 10), (posC, 15), ...],
        posB: [(posA, 10), (posD, 29), ...],
        ...
    }
*/
using Graph = std::unordered_map<std::string,std::vector<std::pair<std::string,int>>>;

Graph build_graph(const Grid& grid) {
    Graph graph;

    for (const auto& kv : grid) {
        if (is_gate(kv.second)) {
            auto reachables = find_reachables(grid, kv.first);
            if (graph.count(kv.second) > 0) {
                std::copy(reachables.begin(), reachables.end(), std::back_inserter(graph.at(kv.second)));
            } else {
                graph[kv.second] = std::move(reachables);
            }
        }
    }
    return graph;
}

struct HeapNode {
    std::string label;
    int steps;
};

struct heap_comp {
    unsigned long operator () (const HeapNode& a, const HeapNode& b) const {
        // > because we want a min heap
        return a.steps > b.steps;
    }
};

int shortest_path(const Graph& graph, const std::string& origin, const std::string& dest) {
    std::unordered_set<std::string> visited;
    std::vector<HeapNode> heap;
    heap.push_back({origin, 0});

    while (!heap.empty()) {
        std::pop_heap(heap.begin(), heap.end(), heap_comp{});
        auto node = heap.back();
        heap.pop_back();

        if (node.label == dest) {
            // We subtract 1 since, as this is the actual goal, we
            // didn't really step into the portal
            return node.steps - 1;
        }

        if (visited.count(node.label) > 0) continue;
        visited.insert(node.label);

        for (const auto& neighbor : graph.at(node.label)) {
            // + 1 is the cost of stepping into a portal
            heap.push_back({neighbor.first, node.steps + neighbor.second + 1});
            std::push_heap(heap.begin(), heap.end(), heap_comp{});
        }
    }

    return -1;
}

void print_graph(const Graph& graph) {
    for (const auto& origin_map : graph) {
        auto origin = origin_map.first;
        auto reachables = origin_map.second;
        for (const auto& p : reachables) {
            std::cout << origin << " -> " << p.first << "(" << p.second << ")" << std::endl;
        }
    }
}

int main() {
    auto grid = parse_input("input.txt");
    auto graph = build_graph(grid);

    // Part 1
    auto steps = shortest_path(graph, "AA", "ZZ");
    std::cout << "Part 1: " << steps << std::endl;
}