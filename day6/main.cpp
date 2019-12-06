#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <unordered_map>
#include <vector>


using kv = std::pair<std::string,std::string>;

std::vector<kv> parse_input(std::string filename) {
    std::vector<kv> pairs;

    std::ifstream file(filename);
    std::transform(
        std::istream_iterator<std::string>(file),
        std::istream_iterator<std::string>(),
        std::back_inserter(pairs),
        [](std::string str) {
            auto delim = str.find(')');
            return std::pair<std::string,std::string>(str.substr(0, delim), str.substr(delim + 1));
        }
    );
    return pairs;
}

using map_key = std::string;
using map_value = std::vector<std::string>;
using tree_t = std::unordered_map<map_key,map_value>;
tree_t make_tree(const std::vector<kv>& pairs) {
    tree_t tree;
    for (auto pair : pairs) {
        if (tree.count(pair.first) == 0) {
            tree[pair.first] = map_value{pair.second};
        } else {
            tree[pair.first].push_back(pair.second);
        }
    }
    return tree;
}

size_t checksum(const tree_t& tree, const map_key node, size_t height) {
    // In our tree representation, a missing node just means it has no children
    if (tree.count(node) == 0) return height;

    return std::accumulate(
        tree.at(node).begin(),
        tree.at(node).end(),
        height,
        [&tree,height](size_t accumulator, map_key child) {
            return accumulator + checksum(tree, child, height + 1);
        }
    );
}

std::vector<std::string> path_to(const tree_t& tree, const map_key& curr_node, const map_key& goal_node) {
    if (curr_node == goal_node) return std::vector<std::string>{goal_node};

    // Current node has no children - bail out
    if (tree.count(curr_node) == 0) return std::vector<std::string>{};

    for (auto child : tree.at(curr_node)) {
        auto subpath = path_to(tree, child, goal_node);
        if (subpath.size() > 0) {
            subpath.push_back(curr_node);
            return subpath;
        }
    }

    // Not found on this branch
    return std::vector<std::string>{};
}

int main() {
    auto pairs = parse_input("input.txt");
    auto tree = make_tree(pairs);

    // Part 1
    std::cout << "Part 1: " << checksum(tree, "COM", 0) << std::endl;

    // Part 2
    auto you_path = path_to(tree, "COM", "YOU");
    auto san_path = path_to(tree, "COM", "SAN");

    // Now we traverse both paths until they diverge. At the diversion point (the nearest common ancestor),
    // we can figure out the size of each branch by subtracing distance_from_root to the size of each path
    for (size_t dist_from_root = 0; dist_from_root < std::min(you_path.size(), san_path.size()); dist_from_root++) {
        // This means we started diverging, so we went one hop over the diversion point. To make up for it, we
        // adjust each branch size by adding 1
        if (you_path[you_path.size() - 1 - dist_from_root] != san_path[san_path.size() - 1 - dist_from_root]) {
            size_t you_branch_size = you_path.size() - 1 - dist_from_root;
            size_t san_branch_size = san_path.size() - 1 - dist_from_root;
            std::cout << "Part 2: " << you_branch_size + san_branch_size << std::endl;
            break;
        }
    }

}