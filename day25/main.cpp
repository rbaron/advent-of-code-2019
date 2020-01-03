#include <algorithm>
#include <iostream>
#include <numeric>
#include <unordered_map>
#include <vector>

#include "cpu.hpp"

enum Step {
    N, E, S, W
};

struct StepHash {
    template <typename T>
    std::size_t operator()(T t) const {
        return static_cast<std::size_t>(t);
    }
};

const std::unordered_map<Step,Step,StepHash> inv_step{
    {N,S}, {S,N}, {E,W}, {W,E}
};

const std::unordered_map<Step,std::string,StepHash> step_label{
    {N,"north"}, {S,"south"}, {E,"east"}, {W,"west"}
};

using steps_t = std::vector<Step>;

const std::unordered_map<std::string,steps_t> items{
    {"bowl of rice", {E,N,W}},
    {"fuel cell", {E,N,N,W,S,S}},
    {"ornament", {E,N,N,W}},
    {"planetoid", {E,N,N}},
    {"cake", {E,N,N,E}},
    {"astrolabe", {E,N,N,E,S,W,N}},
    {"shell", {E,E,E}},
    {"monolith", {E,E,S}},
    // {"infinite loop", {E,N,N,W,S}},
    // {"molten lava", {E,N,N,W,W}},
    // {"escape pod", {E,N,N,E,W}},
    // {"photons", {E,N,N,E,S,W}},
    // {"giant electromagnet", {E,N,N,E,S,W,W}},
};

const steps_t checkpoint_path{E,N,N,E,S,W,N,W};

void push_word(CPU& cpu, const std::string& word) {
    for (auto c : word) {
        cpu.get_in().push(c);
    }
    cpu.get_in().push('\n');
}

void pick_item(CPU& cpu, const std::string& item, const steps_t& steps) {
    for (auto& w : steps) {
        push_word(cpu, step_label.at(w));
    }
    push_word(cpu, "take " + item);
    for (auto it = steps.rbegin(); it != steps.rend(); it++) {
        push_word(cpu, step_label.at(inv_step.at(*it)));
    }
}


template <typename T>
std::vector<std::vector<T>> all_combinations(std::vector<T> elems) {
    if (elems.empty()) return {{}};

    std::vector<std::vector<T>> res;

    auto head = elems.back();
    elems.pop_back();

    for (auto sub : all_combinations(elems)) {
        res.push_back(sub);
        sub.push_back(head);
        res.push_back(sub);
    }
    return res;
}

void part1() {
    const auto tape = read_tape_from_disk("input.txt");

    // disassemble(tape, 1424);

    CPU cpu(tape);

    auto all_items = std::accumulate(
        items.begin(),
        items.end(),
        std::vector<std::string>(),
        [](std::vector<std::string> acc, const std::pair<std::string,steps_t>& kv) { acc.push_back(kv.first); return acc; }
    );

    // Pick all items and move to checkpoint
    for (const auto& item : all_items) {
        pick_item(cpu, item, items.at(item));
    }

    for (auto& w : checkpoint_path) {
        push_word(cpu, step_label.at(w));
    }

    cpu.run_until_more_input_is_required();
    while (!cpu.get_out().empty()) {
        cpu.get_out().pop();
    }

    // Try all possible combinations of items to drop
    for (const auto& comb : all_combinations(all_items)) {
        CPU cpu_copy(cpu);

        for (const auto& item : comb) {
            push_word(cpu_copy, "drop " + item);
        }
        push_word(cpu_copy, "north");

        cpu_copy.run_until_more_input_is_required();

        // One of these outputs will not say
        std::string output;
        while (!cpu_copy.get_out().empty()) {
            output.push_back(cpu_copy.get_out().front());
            cpu_copy.get_out().pop();
        }

        // If the current output does not contain the string "ejected", it hopefully means
        // that we found the right combination of items
        if (output.find("ejected") == std::string::npos) {
            std::cout << output << std::endl;
            break;
        }
    }
}

int main() {
    part1();
}