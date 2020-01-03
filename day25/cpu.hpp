#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <unordered_map>


using Tape = std::unordered_map<size_t,long long int>;

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

size_t disassemble(const Tape& tape, size_t start = 0);

class CPU {
public:
    CPU(const Tape& tape):
        tape(tape),
        pc(0),
        relative_addr_base(0) {}

    InstrExecStatus run_program() {
        InstrExecStatus status;
        while((status = run_one_instruction()) == InstrExecStatus::ALL_GOOD);
        return status;
    }

    InstrExecStatus run_until_more_input_is_required() {
        InstrExecStatus status;

        // Run until input is required and `in` queue is empty
        while(peek_current_opcode() % 100 != static_cast<int>(OpCodes::INPUT) || !get_in().empty()) {
            status = run_one_instruction();
            if (status != InstrExecStatus::ALL_GOOD) return status;
        }
        return status;
    }

    int peek_current_opcode() {
        return tape[pc];
    }

    InstrExecStatus run_one_instruction() {
        auto opcode = tape[pc] % 100;
        auto modes  = tape[pc] / 100;

        // disassemble(tape, pc);

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
                auto addrs = eval_operand_addrs(1, modes, pc + 1);
                tape[addrs[0]] = in.front();
                in.pop();
                pc += 2;
                return InstrExecStatus::ALL_GOOD;
            }
            case OpCodes::OUTPUT: {
                auto addrs = eval_operand_addrs(1, modes, pc + 1);
                // std::cout <<  "Put " << static_cast<char>(tape[addrs[0]]) << std::endl;
                out.push(tape[addrs[0]]);
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

    std::queue<long long int>& get_in() { return in; }
    std::queue<long long int>& get_out() { return out; }

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

    std::queue<long long int> in;
    std::queue<long long int> out;

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

void disassemble_addressing(long long instr, int mode) {
    switch (static_cast<AddressingModes>(mode)) {
        case AddressingModes::POSITION: {
            std::cout << "$" << instr << " ";
            return;
        }
        case AddressingModes::IMMEDIATE: {
            std::cout << instr << " ";
            return;
        }
        case AddressingModes::RELATIVE: {
            std::cout << "REL(" << instr << ") ";
            return;
        }
    }
};

size_t disassemble(const Tape& tape, size_t start) {
    size_t addr = start;
    auto opcode = tape.at(addr) % 100;
    auto modes  = tape.at(addr) / 100;

    std::cout << std::setw(6) << addr << " ";

    switch (static_cast<OpCodes>(opcode)) {
        case OpCodes::ADD: {
            std::cout << "ADD ";
            disassemble_addressing(tape.at(addr + 1), modes % 10);
            disassemble_addressing(tape.at(addr + 2), (modes / 10) % 10);
            disassemble_addressing(tape.at(addr + 3), (modes / 100) % 10);
            addr += 4;
            break;
        }
        case OpCodes::MULT: {
            std::cout << "MUL ";
            disassemble_addressing(tape.at(addr + 1), modes % 10);
            disassemble_addressing(tape.at(addr + 2), (modes / 10) % 10);
            disassemble_addressing(tape.at(addr + 3), (modes / 100) % 10);
            addr += 4;
            break;
        }
        case OpCodes::INPUT: {
            std::cout << "INP ";
            disassemble_addressing(tape.at(addr + 1), modes % 10);
            addr += 2;
            break;
        }
        case OpCodes::OUTPUT: {
            std::cout << "OUT ";
            disassemble_addressing(tape.at(addr + 1), modes % 10);
            addr += 2;
            break;
        }
        case OpCodes::JUMP_IF_TRUE: {
            std::cout << "JIT ";
            disassemble_addressing(tape.at(addr + 1), modes % 10);
            disassemble_addressing(tape.at(addr + 2), (modes / 10) % 10);
            addr += 3;
            break;
        }
        case OpCodes::JUMP_IF_FALSE: {
            std::cout << "JIF ";
            disassemble_addressing(tape.at(addr + 1), modes % 10);
            disassemble_addressing(tape.at(addr + 2), (modes / 10) % 10);
            addr += 3;
            break;
        }
        case OpCodes::LESS_THAN: {
            std::cout << "LST ";
            disassemble_addressing(tape.at(addr + 1), modes % 10);
            disassemble_addressing(tape.at(addr + 2), (modes / 10) % 10);
            disassemble_addressing(tape.at(addr + 3), (modes / 100) % 10);
            addr += 4;
            break;
        }
        case OpCodes::EQUALS: {
            std::cout << "EQS ";
            disassemble_addressing(tape.at(addr + 1), modes % 10);
            disassemble_addressing(tape.at(addr + 2), (modes / 10) % 10);
            disassemble_addressing(tape.at(addr + 3), (modes / 100) % 10);
            addr += 4;
            break;
        }
        case OpCodes::SET_REL_OFFSET: {
            std::cout << "SRO ";
            disassemble_addressing(tape.at(addr + 1), modes % 10);
            addr += 2;
            break;
        }
        case OpCodes::HALT: {
            std::cout << "HLT";
            addr += 1;
            break;
        }
        default: throw std::runtime_error("Unknown opcode: " + std::to_string(opcode));
    }

    std::cout << std::setw(10) << " (" << tape.at(start) << ")";
    std::cout << std::endl;
    return addr;

}