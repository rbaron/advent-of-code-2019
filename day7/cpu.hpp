#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>


using Tape = std::vector<int>;

enum class InstrExecStatus {
    IDLE, ALL_GOOD, HALT, UNKOWN_OPCODE
};

class CPU {
public:
    CPU(const Tape& tape, std::istream& in, std::ostream& out): tape(tape), pc(0), in(in), out(out) {}

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

        switch (opcode) {
            case 1: {
                auto operands = eval_operands(2, modes, pc + 1);
                tape[tape[pc+3]] = operands[0] + operands[1];
                pc += 4;
                return InstrExecStatus::ALL_GOOD;
            }
            case 2: {
                auto operands = eval_operands(2, modes, pc + 1);
                tape[tape[pc+3]] = operands[0] * operands[1];
                pc += 4;
                return InstrExecStatus::ALL_GOOD;
            }
            case 3: {
                int n;
                // std::cout << "Input required: ";
                in >> n;
                tape[tape[pc+1]] = n;
                pc += 2;
                return InstrExecStatus::ALL_GOOD;
            }
            case 4: {
                out << tape[tape[pc+1]] << std::endl;
                pc += 2;
                return InstrExecStatus::ALL_GOOD;
            }
            case 5: { // jump-if-true
                auto operands = eval_operands(2, modes, pc + 1);
                pc = operands[0] != 0 ? operands[1] : pc + 3;
                return InstrExecStatus::ALL_GOOD;
            }
            case 6: { // jump-if-false
                auto operands = eval_operands(2, modes, pc + 1);
                pc = operands[0] == 0 ? operands[1] : pc + 3;
                return InstrExecStatus::ALL_GOOD;
            }
            case 7: { // less-than
                auto operands = eval_operands(2, modes, pc + 1);
                tape[tape[pc+3]] = operands[0] < operands[1] ? 1 : 0;
                pc += 4;
                return InstrExecStatus::ALL_GOOD;
            }
            case 8: { // equals
                auto operands = eval_operands(2, modes, pc + 1);
                tape[tape[pc+3]] = operands[0] == operands[1] ? 1 : 0;
                pc += 4;
                return InstrExecStatus::ALL_GOOD;
            }
            case 99: return InstrExecStatus::HALT;
            default: return InstrExecStatus::UNKOWN_OPCODE;
        }
    }

    void mem_dump() const {
        for (int i = 0; i < tape.size(); i++) {
            std::cout << i << ": " << tape[i] << std::endl;
        }
    }
private:
    Tape tape;

    // Program counter
    size_t pc;

    std::istream &in;
    std::ostream &out;

    int eval_operand(int mode, size_t position) {
        return  mode == 0 ? tape[tape[position]] : tape[position];
    }

    std::vector<int> eval_operands(int how_many, int modes, size_t position) {
        std::vector<int> ret;
        for(; how_many > 0; how_many--, modes /= 10, position++) {
            ret.push_back(eval_operand(modes % 10, position));
        }
        return ret;
    }
};

Tape read_tape_from_disk(std::string filename) {
    Tape tape;
    std::ifstream file(filename);

    std::string val;
        while(std::getline(file, val, ',')) {
        tape.push_back(std::stoi(val));
    }
    return tape;
}