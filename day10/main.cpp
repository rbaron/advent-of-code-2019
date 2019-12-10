#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <vector>

using coord_t = std::pair<int,int>;
using grid_t = std::set<coord_t>;

coord_t operator+(const coord_t& c0, const coord_t& c1) {
    return {c0.first + c1.first, c0.second + c1.second};
};

coord_t operator-(const coord_t& c0, const coord_t& c1) {
    return {c0.first - c1.first, c0.second - c1.second};
};

coord_t operator/(const coord_t& c0, int factor) {
    return {c0.first/factor, c0.second/factor};
};

std::ostream& operator<<(std::ostream& os, const coord_t& coord) {
    os << "(" << coord.first << "," << coord.second << ")";
    return os;
}

grid_t parse_input(std::string filename) {
    std::ifstream file(filename);

    return std::accumulate(
        std::istream_iterator<std::string>(file),
        std::istream_iterator<std::string>(),
        grid_t(),
        [](grid_t& grid, const std::string& line) {
            static int y = 0;
            for (int x = 0; x < line.size(); x++)
                if (line[x] == '#') grid.emplace(coord_t(x, y));
            y++;
            return grid;
        }
    );
}

int gcd(int a, int b) {
    return b == 0 ? a : gcd(b, a % b);
}

std::vector<int> all_factors(int a) {
    std::vector<int> factors;
    for (int i = 1; i < std::sqrt(a); i++)
        if (a % i == 0) {
            factors.push_back(i);
            factors.push_back(a / i);
        }
    return factors;
}

std::vector<int> common_factors(int a, int b) {
    return all_factors(std::abs(gcd(a, b)));
}

bool has_direct_line_of_sight(const grid_t& grid, const coord_t& c0, const coord_t& c1) {
    if (c0 == c1) return false;

    auto diff = c1 - c0;

    for (auto factor : common_factors(diff.first, diff.second)) {
        for (auto pos = c0 + diff / factor; pos != c1; pos = pos + diff / factor) {
            if (grid.count(pos) != 0) {
                return false;
            }
        }
    }
    return true;
}

int main() {
    auto grid = parse_input("input.txt");

    std::map<coord_t,int> counts;
    for (auto& c0 : grid) {
        for (auto& c1 : grid) {
            if (has_direct_line_of_sight(grid, c0, c1)) {
                counts[c0]++;
            }
        }
    }

    auto best_pos = *std::max_element(
        counts.begin(),
        counts.end(),
        [](const std::pair<coord_t,int>& p1, const std::pair<coord_t,int> p2) {
            return p1.second < p2.second;
        }
    );

    std::cout << "Part 1: " << best_pos.second << std::endl;
}