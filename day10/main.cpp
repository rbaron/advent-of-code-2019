#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <vector>

const double PI = std::atan(1) * 4;

template<typename T>
using coord_t = std::pair<T,T>;

template<typename T>
using grid_t = std::set<coord_t<T>>;

template<typename T>
coord_t<T> operator+(const coord_t<T>& c0, const coord_t<T>& c1) {
    return {c0.first + c1.first, c0.second + c1.second};
};

template<typename T>
coord_t<T> operator-(const coord_t<T>& c0, const coord_t<T>& c1) {
    return {c0.first - c1.first, c0.second - c1.second};
};

template<typename T, typename F>
coord_t<T> operator/(const coord_t<T>& c0, F factor) {
    return {c0.first/factor, c0.second/factor};
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const coord_t<T>& coord) {
    os << "(" << coord.first << "," << coord.second << ")";
    return os;
};

template<typename T>
grid_t<T> parse_input(std::string filename) {
    std::ifstream file(filename);

    return std::accumulate(
        std::istream_iterator<std::string>(file),
        std::istream_iterator<std::string>(),
        grid_t<int>(),
        [](grid_t<int>& grid, const std::string& line) {
            static int y = 0;
            for (int x = 0; x < line.size(); x++)
                if (line[x] == '#') grid.emplace(coord_t<int>(x, y));
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

template<typename T>
bool has_direct_line_of_sight(const grid_t<T>& grid, const coord_t<T>& c0, const coord_t<T>& c1) {
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

template<typename T>
coord_t<double> to_polar(const coord_t<T>& coord, const coord_t<T>& origin) {

    // Get angle and map it to [0,2pi)
    auto angle = std::atan2(coord.second - origin.second, coord.first - origin.first);
    angle = angle < 0 ? angle + 2*PI : angle;

    // Add PI/2 so we can sort by angle, with smaller values aligned to the negative Y-axis
    angle = angle + PI/2;
    angle = angle >= 2*PI ? angle - 2*PI : angle;
    return {
        angle,
        std::sqrt(std::pow(coord.first - origin.first, 2) + std::pow(coord.second - origin.second, 2))
    };
}

coord_t<int> to_cartesian(const coord_t<double>& polar_coord, const coord_t<int>& origin) {
    auto angle = polar_coord.first;
    angle = angle - PI/2;
    return {
        std::round(origin.first + polar_coord.second * std::cos(angle)),
        std::round(origin.second + polar_coord.second * std::sin(angle))
    };
}

int main() {
    auto grid = parse_input<int>("input.txt");

    std::map<coord_t<int>,int> counts;
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
        [](const std::pair<coord_t<int>,int>& p1, const std::pair<coord_t<int>,int> p2) {
            return p1.second < p2.second;
        }
    );

    std::cout << "Part 1: " << best_pos.second << std::endl;

    auto origin = best_pos.first;

    /*
     * In order to visit the asteroids clockwise (starting at the noon position), we
     * convert all cartesian coordinates to polar coordianates. Note that the original
     * coordinate system is computer-vision-like, with the Y-axis poiting down.
     * During the conversion to polar coordinates, we add pi/2 to the resulting angle
     * component, since we want the angle 0 to align with the _negative_ y axis from
     * the original cartesian coordinates. The angles are all positives and grow clockwise.
     *
     * We insert the (angle, norm) std::pair into a set, which natually orders the elements
     * regarding the angle, with ties being broken by smaller norms, just like we want.
     * */
    std::set<coord_t<double>> polar_coords;
    std::transform(
        grid.begin(),
        grid.end(),
        std::inserter(polar_coords, polar_coords.begin()),
        [&origin](const coord_t<int>& coord) { return to_polar<int>(coord, origin); }
    );

    // Start removing asteroids clockwise
    auto first = polar_coords.begin();
    double angle = first->first;
    polar_coords.erase(first);

    for (int i = 1; !polar_coords.empty(); ) {
        for (auto it = polar_coords.begin(); it != polar_coords.end();) {
            if (std::abs(it->first - angle) > pow(10, -6)) {
                angle = it->first;
                i++;
                if (i == 200) {
                    auto cartesian_coords = to_cartesian(*it, origin);
                    std::cout << "Part 2: " <<  cartesian_coords.first * 100 + cartesian_coords.second << std::endl;
                }
                it = polar_coords.erase(it);
            } else {
                it++;
            }
        }
    }
}