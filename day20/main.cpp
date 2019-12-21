#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <set>

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

struct GridContainer {
    Grid grid;
    int width;
    int height;
};

bool is_gate(const std::string& str) {
    return str.length() == 2 && std::all_of(str.begin(), str.end(), [](char c) { return std::isupper(c); });
}

enum class GateType {
    OUTER, INNER,
};

GateType get_gate_type(const GridContainer& gc, const Coord& gate_pos) {
    if (gate_pos.x == 2 || gate_pos.x == gc.width - 3 || gate_pos.y == 2 || gate_pos.y == gc.height - 3) {
        return GateType::OUTER;
    }
    return GateType::INNER;
};

GateType oposite_gate(GateType g) {
    return g == GateType::INNER ? GateType::OUTER : GateType::INNER;
}

GridContainer parse_input(const std::string& filename) {
    std::fstream file(filename);

    Grid grid;
    std::vector<Coord> letters_coords;
    int x = 0, y = 0;
    int width = 0, height;

    std::string line;
    while (std::getline(file, line)) {
        width = std::max(x, width);
        x = 0;
        for (auto c : line) {
            if (c != '#') grid[{x,y}] = c;
            if (std::isupper(c)) letters_coords.push_back({x,y});
            x++;
        }
        y++;
    }

    height = y;

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

    return {std::move(grid), width, height};
}

struct Edge {
    std::string label;
    int steps;
    GateType type;
    GateType source_type;
};

std::ostream& operator<<(std::ostream &out, GateType type) {
    out << (type == GateType::INNER ? "inner" : "outer");
    return out;
}
std::ostream& operator<<(std::ostream &out, const Edge& edge) {
    out << edge.label << "(" << edge.steps << ") " << edge.source_type << " to " << edge.type;
    return out;
}

std::vector<Edge> find_reachables(const GridContainer& gc, const Coord& origin) {
    std::vector<Edge> reachable;
    std::unordered_set<Coord,coord_hash> visited;
    std::queue<std::pair<Coord,int>> nodes;

    auto origin_gate_type = get_gate_type(gc, origin);

    nodes.push({origin, 0});

    while (!nodes.empty()) {
        auto node = nodes.front();
        nodes.pop();

        auto pos = node.first;
        auto steps = node.second;

        if (gc.grid.count(pos) == 0 || visited.count(pos) > 0) continue;
        visited.insert(pos);

        auto label = gc.grid.at(pos);

        if (is_gate(label) && steps > 0) {
            reachable.push_back({label, steps, get_gate_type(gc, pos), origin_gate_type});
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
        posA: [(posB, 10, INNER, OUTER), (posC, 15, OUTER, INNER), ...],
        posB: [(posA, 10, OUTER, INNER), (posD, 29, OUTER, OUTER), ...],
        ...
    }
*/
using Graph = std::unordered_map<std::string,std::vector<Edge>>;

Graph build_graph(const GridContainer& gc) {
    Graph graph;

    for (const auto& kv : gc.grid) {
        if (is_gate(kv.second)) {
            auto reachables = find_reachables(gc, kv.first);
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
    int level;
    GateType type;
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
    heap.push_back({origin, 0, 0});

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
            heap.push_back({neighbor.label, node.steps + neighbor.steps + 1});
            std::push_heap(heap.begin(), heap.end(), heap_comp{});
        }
    }

    return -1;
}

int shortest_path2(const Graph& graph, const std::string& origin, const std::string& dest) {
    std::set<std::tuple<std::string,int,GateType>> visited;
    std::vector<HeapNode> heap;
    heap.push_back({origin, 0, 0, GateType::OUTER});

    while (!heap.empty()) {
        std::pop_heap(heap.begin(), heap.end(), heap_comp{});
        auto node = heap.back();
        heap.pop_back();

        if (node.label == dest) {
            // We subtract 1 since, as this is the actual goal, we
            // didn't really step into the portal
            return node.steps - 1;
        }

        if (visited.count({node.label, node.level, node.type}) > 0) continue;
        visited.insert({node.label, node.level, node.type});

        for (const auto& neighbor : graph.at(node.label)) {
            if (neighbor.source_type != node.type) continue;

            auto teleported_gate_type = oposite_gate(neighbor.type);

            if (node.level == 0) {
                if (neighbor.type == GateType::INNER) {
                    heap.push_back({neighbor.label, node.steps + neighbor.steps + 1, node.level + 1, teleported_gate_type});
                    std::push_heap(heap.begin(), heap.end(), heap_comp{});
                } else if(neighbor.label == dest) {
                    heap.push_back({neighbor.label, node.steps + neighbor.steps + 1, node.level, teleported_gate_type});
                    std::push_heap(heap.begin(), heap.end(), heap_comp{});
                }
            } else {
                // Inner mazes have AA and ZZ closed
                if (neighbor.label == origin || neighbor.label == dest) continue;

                if (neighbor.type == GateType::INNER) {
                    heap.push_back({neighbor.label, node.steps + neighbor.steps + 1, node.level + 1, teleported_gate_type});
                    std::push_heap(heap.begin(), heap.end(), heap_comp{});
                } else {
                    heap.push_back({neighbor.label, node.steps + neighbor.steps + 1, node.level - 1, teleported_gate_type});
                    std::push_heap(heap.begin(), heap.end(), heap_comp{});
                }
            }
        }
    }

    return -1;
}

void print_graph(const Graph& graph) {
    for (const auto& origin_map : graph) {
        auto origin = origin_map.first;
        auto reachables = origin_map.second;
        for (const auto& e : reachables) {
            std::cout << origin << " -> " << e << std::endl;
        }
    }
}

int main() {
    auto grid_container = parse_input("input.txt");
    auto graph = build_graph(grid_container);

    // print_graph(graph);

    // Part 1
    std::cout << "Part 1: " << shortest_path(graph, "AA", "ZZ") << std::endl;

    // Part 2
    std::cout << "Part 2: " << shortest_path2(graph, "AA", "ZZ") << std::endl;
}