#include <algorithm>
#include <iostream>
#include <regex>
#include <vector>
#include <fstream>
#include <set>


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
         ((*reg_it)[1].str())[0], // First char of the first regex group
         std::stoi((*reg_it)[2])  // Conversion to int of the second regex group
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

   std::set<Coord> grid;

   // Fill up grip with wire 1 positions
   Coord wire1pos(0,0);
   for (auto maneuver : decode_path(path1)) {
      wire1pos = for_each_visited_coord(
         wire1pos,
         maneuver,
         [&grid](const Coord& current){ grid.insert(current); });
   }

   std::set<Coord> intersections;

   // Follow wire 2 until it visits a grid cell in which wire 1 is present
   Coord wire2pos(0,0);
   for (auto maneuver : decode_path(path2)) {
      wire2pos = for_each_visited_coord(
         wire2pos,
         maneuver,
         [&grid, &intersections](const Coord& current){
            if (grid.find(current) != grid.end()) {
               intersections.insert(current);
            }
         });
   }

   auto closest_intersection = *std::min_element(
      intersections.begin(),
      intersections.end(),
      [](Coord c1, Coord c2){
         return manhattan(c1) < (manhattan(c2));
      }
   );

   std::cout << "Closest intersection: (" << closest_intersection.first << "," << closest_intersection.second << ")\n";
   std::cout << "Distance: " << manhattan(closest_intersection) << std::endl;
}