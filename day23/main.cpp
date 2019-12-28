#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>

#include "cpu.hpp"

struct Message {
    long long int from, to, x, y;
};

std::ostream& operator<<(std::ostream &out, const Message& m) {
    out << "Message to=" << m.to << " x=" << m.x << " y=" << m.y;
    return out;
}

Message get_message(int from, std::queue<long long int>& queue) {
    Message m;
    m.from = from;
    m.to = queue.front();
    queue.pop();
    m.x = queue.front();
    queue.pop();
    m.y = queue.front();
    queue.pop();
    return m;
}

std::vector<Message> get_all_messages(int from, std::queue<long long int>& queue) {
    std::vector<Message> ms;
    while (!queue.empty() && queue.front() == -1) queue.pop();
    while (queue.size() >= 3) {
        auto m = get_message(from, queue);
        ms.push_back(std::move(m));
    }
    return ms;
}

int main() {
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

    Message nat{-1, -1, -1, -1};
    long long int last_from_nat_y = -1;

    while (true) {
        std::vector<bool> sent(50);
        for (int i = 0; i < n_cpus; i++) {
            auto& cpu = cpus[i];

            if (mailbox[i].size() == 0) {
                cpu.run_until_input_is_required();
                cpu.get_in().push(-1);
                cpu.run_one_instruction();
                cpu.run_until_input_is_required();
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
                cpu.run_until_input_is_required();
            }

            auto messages = get_all_messages(i, cpu.get_out());
            for (const auto& m : messages) {
                sent[i] = true;
                if (m.to == 255) {

                    // Just so we print it _only_ on the first message to 255
                    if (nat.from == -1) std::cout << "Part 1: " << m.y << std::endl;
                    nat = m;
                } else {
                    mailbox[m.to].push(m);

                }
            }
        }

        // If network is idle: no messages to be delivered
        if (std::all_of(
                mailbox.begin(),
                mailbox.end(),
                [](const std::pair<int,std::queue<Message>>& pair) { return pair.second.empty(); })
        ) {
            mailbox[0].push(nat);
            if (nat.y == last_from_nat_y) {
                std::cout << "Part 2: " << nat.y << std::endl;
                goto end;
            } else {
                last_from_nat_y = nat.y;
            }
        }

    }
end:
    return 0;
}