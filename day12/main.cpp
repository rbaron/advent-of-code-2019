#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <regex>
#include <vector>

struct pt_3d {
    pt_3d(int x, int y, int z): x(x), y(y), z(z) {}
    bool operator==(const pt_3d& rhs) {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }
    bool operator< (const pt_3d& rhs) const {
        if (x < rhs.x) return true;
        else if (x > rhs.x) return false;
        else if(y < rhs.y) return true;
        else if (y > rhs.y) return false;
        else if(z < rhs.z) return true;
        else if (z > rhs.z) return false;
        return false;
    }
    int x, y, z;
};

struct body {
    body(pt_3d pos, pt_3d vel): pos(std::move(pos)), vel(std::move(vel)) {}
    pt_3d pos;
    pt_3d vel;
    bool operator< (const body& rhs) const {
        if (pos < rhs.pos) return true;
        else if (rhs.pos < pos) return false;
        else if(vel < rhs.vel) return true;
        else if (rhs.vel < vel) return false;
        return false;
    }
};

std::ostream& operator<<(std::ostream& out, const body& b) {
    out << "pos=<x=" << b.pos.x << ", y=" << b.pos.y << ", z=" << b.pos.z << ">, ";
    out << "vel=<x=" << b.vel.x << ", y=" << b.vel.y << ", z=" << b.vel.z << ">";
    return out;
}

const std::vector<std::pair<int,int>> combinations {
    {0,1}, {0,2}, {0,3}, {1,2}, {1,3}, {2,3}
};

void apply_gravity(body& b1, body& b2) {
    auto update_component = [](int pos1, int pos2, int& vel1, int& vel2) {
        if (pos1 < pos2) { vel1++; vel2--; }
        else if (pos1 > pos2) { vel1--; vel2++; }
    };
    update_component(b1.pos.x, b2.pos.x, b1.vel.x, b2.vel.x);
    update_component(b1.pos.y, b2.pos.y, b1.vel.y, b2.vel.y);
    update_component(b1.pos.z, b2.pos.z, b1.vel.z, b2.vel.z);
}

void apply_velocity(body& b) {
    b.pos.x += b.vel.x;
    b.pos.y += b.vel.y;
    b.pos.z += b.vel.z;
}

int energy(const body& b) {
    return (std::abs(b.pos.x) + std::abs(b.pos.y) + std::abs(b.pos.z)) *
           (std::abs(b.vel.x) + std::abs(b.vel.y) + std::abs(b.vel.z));
}

int total_energy(const std::vector<body>& bodies) {
    return std::accumulate(
        bodies.begin(),
        bodies.end(),
        0,
        [](int total_energy, const body& b) { return total_energy + energy(b); }
    );
}

pt_3d parse_line(const std::string& line) {
    std::regex pattern("<x=(-?\\d+), y=(-?\\d+), z=(-?\\d+)>");
    std::smatch matches;
    if (std::regex_search(line, matches, pattern)) {
        return pt_3d{
            std::stoi(matches[1]),
            std::stoi(matches[2]),
            std::stoi(matches[3]),
        };
    } else throw std::runtime_error("Unable to parse line: " + line);
}

std::vector<body> read_input(const std::string& filename) {
    std::vector<body> bodies;
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        bodies.push_back(body{parse_line(line), {0,0,0}});
    }
    return bodies;
}

std::vector<std::vector<int>> description(const std::vector<body>& bodies) {
    std::vector<int> x0, y0, z0;
    for (const auto& body : bodies) {
        x0.push_back(body.pos.x); x0.push_back(body.vel.x);
        y0.push_back(body.pos.y); y0.push_back(body.vel.y);
        z0.push_back(body.pos.z); z0.push_back(body.vel.z);
    }
    return std::vector<std::vector<int>>{x0, y0, z0};
}

long int gcd(long int a, long int b) {
    return b == 0 ? a : gcd(b, a % b);
}

long int lcm(long int a, long int b) {
    return (a * b) / gcd(a, b);
}

int main() {
    auto bodies = read_input("input.txt");

    std::vector<long int> cycles{0, 0, 0};
    const auto desc0 = description(bodies);

    for (int i = 0; ; i++) {
        for (const auto& comb : combinations) apply_gravity(bodies.at(comb.first), bodies.at(comb.second));
        for (auto& body : bodies) apply_velocity(body);

        if (i == 1000 - 1) {
            std::cout << "Part 1: " << total_energy(bodies) << std::endl;
        }

        auto desc = description(bodies);

        for (int j = 0; j < cycles.size(); j++) {
            if (desc[j] == desc0[j]) cycles[j] = i + 1 - cycles[j];
        }

        if (std::all_of(cycles.begin(), cycles.end(), [](int v) { return v > 0; })) break;

    }

    long int next_matching_cycle = lcm(lcm(cycles[0], cycles[1]), cycles[2]);
    std::cout << "Part 2: " << next_matching_cycle << std::endl;
}