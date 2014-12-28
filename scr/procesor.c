#include "procesor.h"
#include "regAndMem.h"
#include "instructionset.h"
#include "asmkeywords.h"
#include "outputbuffer.h"

bool load_program(const struct VString *program, struct programData *programMemory)
{
	bool returnValue = false;
	bool instructionFound = false;
	struct VString *substring = new(VString, NULL, 6);
	uint64_t position = 0;

	int i;
	int j;
	for (j = 0; j < MAX_PROGRAM_MEMORY + 1; ++j) {
		position = get_substring_in_line(program, substring, position);
		instructionFound = false;
	
		if (!position) {
			output_print("ERROR: No Instruction found.", TRUE);
			goto Exit_1;
		}

		for (i = 0; i <  AMOUNT_INST; ++i) {
			if (equal(substring, InstructionSet[i].mnemonic)) {
				instructionFound = true;
				programMemory[j].instructionPos = i;
				position = get_substring_in_line(program, substring, position);

				if (!position) {
					if (InstructionSet[i].operand == EMPTY) {
						programMemory[j].operand = 0;
					} else {
						output_print("Instruction without operand: ", FALSE);
						output_print_string(substring, TRUE);
						goto Exit_1;
					}
				} else {
					programMemory[j].operand = prepareBitField(substring, &InstructionSet[i]);
				}

				if (programMemory[j].operand == -1) 
					goto Exit_1;
				break;
			}
		}

		if (!instructionFound && compare_string(substring, 0, END, utf8_char_number(END, -1))) {
			for (i = j; i < MAX_PROGRAM_MEMORY; ++i) {
				programMemory[i].instructionPos = NOP_POS;
				programMemory[i].operand = 0;
			}
			break;
		}

		if (!instructionFound) {
			output_print("ERROR: Instruction not found: ", FALSE);
			output_print_string(substring, TRUE);
			goto Exit_1;
		}

		if (!get_next_line_pos(program, position, &position)) {
			output_print("ERROR: "END" not found.", TRUE);
			goto Exit_1;
		}
	}

	returnValue = true;

Exit_1:
	delete(substring);

	return returnValue;
}

void step_instruction(const struct programData *program) 
{
	uint16_t currentPC = getPC();

	if (program[currentPC].instructionPos >= AMOUNT_INST) {
		resetPC();
		currentPC = getPC();
	}

	InstructionSet[program[currentPC].instructionPos].instrucFunc(program[currentPC].operand);

	incrementPC();
}










