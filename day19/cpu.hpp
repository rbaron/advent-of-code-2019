#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>


using Tape = std::unordered_map<size_t,long int>;

enum class InstrExecStatus {
    IDLE, ALL_GOOD, HALT, UNKOWN_OPCODE, NO_INPUT,
};

enum class OpCodes {
    ADD            = 1,
    MULT           = 2,
    INPUT          = 3,
    OUTPUT         = 4,
    JUMP_IF_TRUE   = 5,
    JUMP_IF_FALSE  = 6,
    LESS_THAN      = 7,
    EQUALS         = 8,
    SET_REL_OFFSET = 9,
    HALT           = 99,
};

enum class AddressingModes {
    POSITION  = 0,
    IMMEDIATE = 1,
    RELATIVE  = 2,
};

class CPU {
public:
    CPU(const Tape& tape, std::istream& in, std::ostream& out):
        tape(tape),
        pc(0),
        relative_addr_base(0),
        in(in),
        out(out) {}

    InstrExecStatus run_program() {
        InstrExecStatus status;
        while((status = run_one_instruction()) == InstrExecStatus::ALL_GOOD);
        return status;
    }

    int peek_current_opcode() {
        return tape[pc];
    }

    InstrExecStatus run_one_instruction() {
        auto opcode = tape[pc] % 100;
        auto modes  = tape[pc] / 100;

        switch (static_cast<OpCodes>(opcode)) {
            case OpCodes::ADD: {
                auto addrs = eval_operand_addrs(3, modes, pc + 1);
                tape[addrs[2]] = tape[addrs[0]] + tape[addrs[1]];
                pc += 4;
                return InstrExecStatus::ALL_GOOD;
            }
            case OpCodes::MULT: {
                auto addrs = eval_operand_addrs(3, modes, pc + 1);
                tape[addrs[2]] = tape[addrs[0]] * tape[addrs[1]];
                pc += 4;
                return InstrExecStatus::ALL_GOOD;
            }
            case OpCodes::INPUT: {
                int n;
                auto addrs = eval_operand_addrs(1, modes, pc + 1);
                in >> n;
                // std::cout << "Consumed " << n << std::endl;
                tape[addrs[0]] = n;
                pc += 2;
                return InstrExecStatus::ALL_GOOD;
            }
            case OpCodes::OUTPUT: {
                auto addrs = eval_operand_addrs(1, modes, pc + 1);
                // std::cout << "Will produce " << tape[addrs[0]] << std::endl;
                out << tape[addrs[0]] << std::endl;
                pc += 2;
                return InstrExecStatus::ALL_GOOD;
            }
            case OpCodes::JUMP_IF_TRUE: {
                auto addrs = eval_operand_addrs(2, modes, pc + 1);
                pc = tape[addrs[0]] != 0 ? tape[addrs[1]] : pc + 3;
                return InstrExecStatus::ALL_GOOD;
            }
            case OpCodes::JUMP_IF_FALSE: {
                auto addrs = eval_operand_addrs(2, modes, pc + 1);
                pc = tape[addrs[0]] == 0 ? tape[addrs[1]] : pc + 3;
                return InstrExecStatus::ALL_GOOD;
            }
            case OpCodes::LESS_THAN: {
                auto addrs = eval_operand_addrs(3, modes, pc + 1);
                tape[addrs[2]] = tape[addrs[0]] < tape[addrs[1]] ? 1 : 0;
                pc += 4;
                return InstrExecStatus::ALL_GOOD;
            }
            case OpCodes::EQUALS: {
                auto addrs = eval_operand_addrs(3, modes, pc + 1);
                tape[addrs[2]] = tape[addrs[0]] == tape[addrs[1]] ? 1 : 0;
                pc += 4;
                return InstrExecStatus::ALL_GOOD;
            }
            case OpCodes::SET_REL_OFFSET: {
                auto addrs = eval_operand_addrs(1, modes, pc + 1);
                relative_addr_base += tape[addrs[0]];
                pc += 2;
                return InstrExecStatus::ALL_GOOD;
            }
            case OpCodes::HALT: return InstrExecStatus::HALT;
            default: return InstrExecStatus::UNKOWN_OPCODE;
        }
    }

    std::istream& get_istream() { return in; }
    std::ostream& get_ostream() { return out; }

    void mem_dump(size_t start, size_t end) {
        for (int i = start; i < end; i++) {
            std::cout << i << ": " << tape[i] << std::endl;
        }
    }
private:
    Tape tape;

    // Program counter
    size_t pc;

    // Base pointer for relative addressing
    size_t relative_addr_base;

    std::istream &in;
    std::ostream &out;

    int eval_operand_addr(int mode, size_t position) {
        switch (static_cast<AddressingModes>(mode)) {
            case AddressingModes::POSITION:  return tape[position];
            case AddressingModes::IMMEDIATE: return position;
            case AddressingModes::RELATIVE:  return relative_addr_base + tape[position];
        }
    }

    std::vector<int> eval_operand_addrs(int how_many, int modes, size_t position) {
        std::vector<int> ret;
        for(; how_many > 0; how_many--, modes /= 10, position++) {
            ret.push_back(eval_operand_addr(modes % 10, position));
        }
        return ret;
    }
};

Tape read_tape_from_disk(std::string filename) {
    Tape tape;
    std::ifstream file(filename, 0);

    size_t head_pos = 0;
    std::string val;
    while(std::getline(file, val, ',')) {
        tape[head_pos] = std::stol(val);
        head_pos++;
    }
    return tape;
}