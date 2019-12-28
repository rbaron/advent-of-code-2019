#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>

#include "cpu.hpp"

struct Message {
    long long int to, x, y;
};

std::ostream& operator<<(std::ostream &out, const Message& m) {
    out << "Message to=" << m.to << " x=" << m.x << " y=" << m.y;
    return out;
}

Message get_message(std::queue<long long int>& queue) {
    Message m;
    m.to = queue.front();
    queue.pop();
    m.x = queue.front();
    queue.pop();
    m.y = queue.front();
    queue.pop();
    return m;
}

std::vector<Message> get_all_messages(std::queue<long long int>& queue) {
    std::vector<Message> ms;
    while (!queue.empty() && queue.front() == -1) queue.pop();
    while (queue.size() >= 3) {
        auto m = get_message(queue);
        ms.push_back(std::move(m));
    }
    return ms;
}

void part1() {
    const auto tape = read_tape_from_disk("input.txt");

    const int n_cpus = 50;

    std::vector<CPU> cpus;
    for (int i = 0; i < n_cpus; i++) {
        CPU cpu(tape);
        cpu.get_in().push(i);
        cpu.run_until_input_is_required();
        cpu.run_one_instruction();
        cpus.push_back(cpu);
    }

    std::unordered_map<int,std::queue<Message>> mailbox;
    while (true) {
        for (int i = 0; i < n_cpus; i++) {
            auto& cpu = cpus[i];

            if (mailbox[i].size() == 0) {
                cpu.run_until_input_is_required();
                cpu.get_in().push(-1);
                cpu.run_one_instruction();
            }
            while (mailbox[i].size() > 0) {
                const auto& m = mailbox[i].front();
                mailbox[i].pop();
                cpu.get_in().push(m.x);
                cpu.get_in().push(m.y);

                cpu.run_until_input_is_required();
                cpu.run_one_instruction();
                cpu.run_until_input_is_required();
                cpu.run_one_instruction();
            }

            auto messages = get_all_messages(cpu.get_out());
            for (const auto& m : messages) {
                if (m.to == 255) {
                    std::cout << "Part 1: " << m.y << std::endl;
                    goto end;
                }
                mailbox[m.to].push(m);
            }
        }
    }
end:
    return;
}

int main() {
    part1();
}