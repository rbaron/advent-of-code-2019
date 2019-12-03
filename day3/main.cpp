#include <algorithm>
#include <iostream>
#include <regex>
#include <vector>
#include <fstream>
#include <map>


using Coord = std::pair<int,int>;
using Direction = char;
using Steps = int;
using Maneuver = std::pair<Direction,Steps>;

std::vector<Maneuver> decode_path(std::string& path) {
   std::vector<Maneuver> maneuvers;

   auto pattern = std::regex("([URLD])(\\d+)");
   std::regex_iterator<std::string::iterator> reg_it(
      path.begin(),
      path.end(),
      pattern
   );
   std::regex_iterator<std::string::iterator> reg_end;

   for (; reg_it != reg_end; reg_it++) {
      maneuvers.push_back(Maneuver(
         ((*reg_it)[1].str())[0], // First char of the first regex group (one of U, D, L, R)
         std::stoi((*reg_it)[2])  // Conversion to int of the second regex group (number of steps)
      ));
   }
   return maneuvers;
}

inline Coord for_each_visited_coord(Coord current, const Maneuver& m, std::function<void(const Coord&)> fn) {
   switch (m.first) {
      case 'U': {
         for(auto y = 0; y < m.second; y++) {
            current.first += 1;
            fn(current);
         }
         return current;
      }
      case 'D': {
         for(auto y = 0; y < m.second; y++) {
            current.first -= 1;
            fn(current);
         }
         return current;
      }
      case 'L': {
         for(auto x = 0; x < m.second; x++) {
            current.second -= 1;
            fn(current);
         }
         return current;
      }
      case 'R': {
         for(auto x = 0; x < m.second; x++) {
            current.second += 1;
            fn(current);
         }
         return current;
      }
      default: throw std::runtime_error("Unexpected maneuver");
   }
}

inline int manhattan(const Coord& c) {
   return std::abs(c.first) + std::abs(c.second);
}

int main() {
   std::string path1, path2;

   std::fstream file("input.txt");
   file >> path1;
   file >> path2;

   std::map<Coord,Steps> grid;

   // Fill up grid with wire 1 positions
   Steps wire1steps = 0;
   Coord wire1pos(0,0);
   for (auto maneuver : decode_path(path1)) {
      wire1pos = for_each_visited_coord(
         wire1pos,
         maneuver,
         [&grid, &wire1steps](const Coord& current){
            wire1steps++;
            if (grid.find(current) == grid.end()) {
               grid[current] = wire1steps;
            }
         });
   }

   std::map<Coord,Steps> intersections;

   // Follow wire 2 until it visits a grid cell in which wire 1 is present
   Steps wire2steps = 0;
   Coord wire2pos(0,0);
   for (auto maneuver : decode_path(path2)) {
      wire2pos = for_each_visited_coord(
         wire2pos,
         maneuver,
         [&grid, &intersections, &wire2steps](const Coord& current){
            wire2steps++;
            if (grid.find(current) != grid.end()) {
               intersections[current] = grid[current] + wire2steps;
            }
         });
   }

   // Part 1 - min intersection by distance to (0,0)
   auto p1_intersection = *std::min_element(
      intersections.begin(),
      intersections.end(),
      [](std::pair<Coord,Steps> p1, std::pair<Coord,Steps> p2){
         return manhattan(p1.first) < manhattan(p2.first);
      }
   );
   std::cout << "Part 1\n";
   std::cout << "Intersection @ (" << p1_intersection.first.first << "," << p1_intersection.first.second << ")\n";
   std::cout << "Distance: " << manhattan(p1_intersection.first) << std::endl;

   // Part 1 - min intersection sum of steps
   auto p2_intersection = *std::min_element(
      intersections.begin(),
      intersections.end(),
      [](std::pair<Coord,Steps> p1, std::pair<Coord,Steps> p2){
         return p1.second < p2.second;
      }
   );
   std::cout << "Part 2\n";
   std::cout << "Intersection @ (" << p2_intersection.first.first << "," << p2_intersection.first.second << ")\n";
   std::cout << "Sum of steps: " << p2_intersection.second << std::endl;
}