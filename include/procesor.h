#ifndef PROCESOR_H
#define PROCESOR_H

#include "instructionset.h"
#include "vstring.h"

#define MAX_PROGRAM_MEMORY 0x800

struct programData {
	int instructionPos;
	BitField operand;
};

bool load_program(const struct VString *program, struct programData *programMemory);
void step_instruction(const struct programData *program);
 
#endif
