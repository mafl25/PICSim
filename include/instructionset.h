#ifndef INSTRUCTION_SET
#define INSTRUCTION_SET

#include <stdint.h>
#include "vstring.h"

#define AMOUNT_INST 31
#define NOP_POS 23

//-----DATA---------------------------------------------------------------------------------------------

//Types of operands that an instruction can handle.
enum operandType {FILE_D, FILE_REG, FILE_B, K_DIR, K_LIT, EMPTY};
typedef enum operandType OperandType;

//What will be passed on to each insctruction function.
typedef int16_t BitField;

//Struct defining each instruction, showing what its called, 
//a pointer to the function and the operand type
struct Instruction
{
	struct VString *mnemonic;
	OperandType operand;
	void (*instrucFunc)(BitField); //Cambiar parametro
};

extern const struct Instruction *InstructionSet;

//-----FUNCTIONS----------------------------------------------------------------------------------------

BitField prepareBitField(const struct VString* operandString, const struct Instruction *set);
void setInstructionSet(void);

//Funciones de instrucciones
void movlwFun(BitField operandNum);
void addlwFun(BitField operandNum);
void movwfFun(BitField operandNum);
void clrfFun(BitField operand);
void andlwFun(BitField operandNum);
void addwfFun(BitField operandNum);
void andwfFun(BitField operandNum);
void bcfFun(BitField operandNum);
void bsfFun(BitField operandNum);
void comfFun(BitField operandNum);
void decfFun(BitField operandNum);
void incfFun(BitField operandNum);
void iorlwFun(BitField operandNum);
void iorwfFun(BitField operandNum);
void rlfFun(BitField operandNum);
void rrfFun(BitField operandNum);
void sublwFun(BitField operandNum);
void subwfFun(BitField operandNum);
void swapfFun(BitField operandNum);
void xorlwFun(BitField operandNum);
void xorwfFun(BitField operandNum);
void movfFun(BitField operandNum);
void gotoFun(BitField operandNum);
void nopFun(BitField operandNum);
void clrwFun(BitField operandNum);
void decfszFun(BitField operandNum);
void incfszFun(BitField operandNum);
void btfscFun(BitField operandNum);
void btfssFun(BitField operandNum);
void callFun(BitField operandNum);
void returnFun(BitField operandNum);

#endif
