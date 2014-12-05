#include "instructionset.h"
#include "regAndMem.h"
#include "outputbuffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define D_MASK	0x80
#define BIT_MASK 0x07
#define BYTE_MASK 0xFF
#define FILE_MASK 0x7F
#define K_DIR_MASK 0x7FF
#define LOWER_NIBBLE_MASK 0x0F

static struct Instruction instructionSetPIC[AMOUNT_INST];//falta
const struct Instruction *InstructionSet = instructionSetPIC;

BitField prepareBitField(const struct VString *operandString, struct Instruction *set)
{
	BitField result = 0;
	int hexResult = 0;
	struct VString *hexWord = new(VString, NULL, 6);

	char *separator = ",";
	int lenSep = strlen(separator);

	uint64_t position = 0;

	switch (set->operand){
		case FILE_D:
			if (!find_in_vector(operandString, 0, separator, lenSep, &position)) {
				output_print("ERROR: Operand of ", FALSE);
				output_print_string(set->mnemonic, FALSE);
				output_print(" is incorrect:", FALSE);
				output_print_string(operandString, TRUE);
				goto Exit_1;
			}

			insert_vector_at(hexWord, 0, operandString, 0, position);
			hexResult = string_to_hex(hexWord);
			if (hexResult == -1) {
				output_print("ERROR: Operand of ", FALSE);
				output_print_string(set->mnemonic, FALSE);
				output_print(" is not a hexadecimal number:", FALSE);
				output_print_string(operandString, TRUE);
				goto Exit_1;
			}

			if (hexResult > FILE_MASK) {
				output_print("WARNING: Operand of ", FALSE);
				output_print_string(set->mnemonic, FALSE);
				output_print("is out of range, using lower least significant bits: ", FALSE);
				output_print_string(operandString, TRUE);
			}

			result = hexResult & FILE_MASK;

			position++;
			insert_vector_at(hexWord, 0, operandString, position, get_char_number(operandString) - position);
			hexResult = string_to_hex(hexWord);
			if (hexResult == -1) {
				output_print("ERROR: Operand of ", FALSE);
				output_print_string(set->mnemonic, FALSE);
				output_print(" is not a hexadecimal number:", FALSE);
				output_print_string(operandString, TRUE);
				goto Exit_1;
			}
			if (hexResult != 0 && hexResult != 1) {
				output_print("WARNING: Operand of ", FALSE);
				output_print_string(set->mnemonic, FALSE);
				output_print("must be 1 or 0, using lower least significant bits: ", FALSE);
				output_print_string(operandString, TRUE);
			}
			result = (hexResult & 0x01) ? (result | 0x80) : result;

			break;

		case FILE_B:
			if (!find_in_vector(operandString, 0, separator, lenSep, &position)) {
				output_print("ERROR: Operand of ", FALSE);
				output_print_string(set->mnemonic, FALSE);
				output_print(" is incorrect:", FALSE);
				output_print_string(operandString, TRUE);
				goto Exit_1;
			}

			insert_vector_at(hexWord, 0, operandString, 0, position);
			hexResult = string_to_hex(hexWord);
			if (hexResult == -1) {
				output_print("ERROR: Operand of ", FALSE);
				output_print_string(set->mnemonic, FALSE);
				output_print(" is not a hexadecimal number:", FALSE);
				output_print_string(operandString, TRUE);
				goto Exit_1;
			}

			if (hexResult > FILE_MASK) {
				output_print("WARNING: Operand of ", FALSE);
				output_print_string(set->mnemonic, FALSE);
				output_print("is out of range, using lower least significant bits: ", FALSE);
				output_print_string(operandString, TRUE);
			}

			result = hexResult & FILE_MASK;

			position++;
			insert_vector_at(hexWord, 0, operandString, position, get_char_number(operandString) - position);
			hexResult = string_to_hex(hexWord);
			if (hexResult == -1) {
				output_print("ERROR: Operand of ", FALSE);
				output_print_string(set->mnemonic, FALSE);
				output_print(" is not a hexadecimal number:", FALSE);
				output_print_string(operandString, TRUE);
				goto Exit_1;
			}
			if (hexResult > BIT_MASK) {
				output_print("WARNING: Operand of ", FALSE);
				output_print_string(set->mnemonic, FALSE);
				output_print("must be between 0 and 7, using lower least significant bits: ", FALSE);
				output_print_string(operandString, TRUE);
			}
			result = result | ((hexResult & 0x07) << 7);

			break;

		case FILE_REG:
			hexResult = string_to_hex(operandString);
			if (hexResult == -1) {
				output_print("ERROR: Operand of ", FALSE);
				output_print_string(set->mnemonic, FALSE);
				output_print(" is not a hexadecimal number:", FALSE);
				output_print_string(operandString, TRUE);
				goto Exit_1;
			}

			if (hexResult > FILE_MASK) {
				output_print("WARNING: Operand of ", FALSE);
				output_print_string(set->mnemonic, FALSE);
				output_print("is out of range, using lower least significant bits: ", FALSE);
				output_print_string(operandString, TRUE);
			}
			result = hexResult & FILE_MASK;

			break;

		case K_DIR:
			hexResult = string_to_hex(operandString);
			if (hexResult == -1) {
				output_print("ERROR: Operand of ", FALSE);
				output_print_string(set->mnemonic, FALSE);
				output_print(" is not a hexadecimal number:", FALSE);
				output_print_string(operandString, TRUE);
				goto Exit_1;
			}

			if (hexResult > K_DIR_MASK) {
				output_print("WARNING: Operand of ", FALSE);
				output_print_string(set->mnemonic, FALSE);
				output_print("is out of range, using lower least significant bits: ", FALSE);
				output_print_string(operandString, TRUE);
			}
			result = hexResult & K_DIR_MASK;

			break;

		case K_LIT:
			hexResult = string_to_hex(operandString);
			if (hexResult == -1) {
				output_print("ERROR: Operand of ", FALSE);
				output_print_string(set->mnemonic, FALSE);
				output_print(" is not a hexadecimal number:", FALSE);
				output_print_string(operandString, TRUE);
				goto Exit_1;
			}

			if (hexResult > BYTE_MASK) {
				output_print("WARNING: Operand of ", FALSE);
				output_print_string(set->mnemonic, FALSE);
				output_print("is out of range, using lower least significant bits: ", FALSE);
				output_print_string(operandString, TRUE);
			}
			result = hexResult & BYTE_MASK;

			break;

		case EMPTY:
			result = 1;

			break;

		default:
			printf("En preparebitField: valor no permitido asignado a operando");
			exit(1);
	}

Exit_1:
	delete(hexWord);

	return result;
}

void setIntructionSet()
{
	//movlw
	instructionSetPIC[0].mnemonic = new(VString, "movlw", -1);
	instructionSetPIC[0].operand = K_LIT;
	instructionSetPIC[0].instrucFunc = &movlwFun;

	//addlw
	instructionSetPIC[1].mnemonic = new(VString, "addlw", -1);
	instructionSetPIC[1].operand = K_LIT;
	instructionSetPIC[1].instrucFunc = &addlwFun;

	//movwf
	instructionSetPIC[2].mnemonic = new(VString, "movwf", -1);
	instructionSetPIC[2].operand = FILE_REG;
	instructionSetPIC[2].instrucFunc = &movwfFun;
	
	//clrf
	instructionSetPIC[3].mnemonic = new(VString, "clrf", -1);
	instructionSetPIC[3].operand = FILE_REG;
	instructionSetPIC[3].instrucFunc = &clrfFun;

	//andlw
	instructionSetPIC[4].mnemonic = new(VString, "andlw", -1);
	instructionSetPIC[4].operand = K_LIT;
	instructionSetPIC[4].instrucFunc = &andlwFun;
	
	//addwf
	instructionSetPIC[5].mnemonic = new(VString, "addwf", -1);
	instructionSetPIC[5].operand = FILE_D;
	instructionSetPIC[5].instrucFunc = &addwfFun;
	
	//andwf
	instructionSetPIC[6].mnemonic = new(VString, "andwf", -1);
	instructionSetPIC[6].operand = FILE_D;
	instructionSetPIC[6].instrucFunc = &andwfFun;
	
	//bcf
	instructionSetPIC[7].mnemonic = new(VString, "bcf", -1);
	instructionSetPIC[7].operand = FILE_B;
	instructionSetPIC[7].instrucFunc = &bcfFun;
	
	//bsf
	instructionSetPIC[8].mnemonic = new(VString, "bsf", -1);
	instructionSetPIC[8].operand = FILE_B;
	instructionSetPIC[8].instrucFunc = &bsfFun;
	
	//comf
	instructionSetPIC[9].mnemonic = new(VString, "comf", -1);
	instructionSetPIC[9].operand = FILE_D;
	instructionSetPIC[9].instrucFunc = &comfFun;
	
	//decf
	instructionSetPIC[10].mnemonic = new(VString, "decf", -1);
	instructionSetPIC[10].operand = FILE_D;
	instructionSetPIC[10].instrucFunc = &decfFun;
	
	//incf
	instructionSetPIC[11].mnemonic = new(VString, "incf", -1);
	instructionSetPIC[11].operand = FILE_D;
	instructionSetPIC[11].instrucFunc = &incfFun;
	
	//iorlw
	instructionSetPIC[12].mnemonic = new(VString, "iorlw", -1);
	instructionSetPIC[12].operand = K_LIT;
	instructionSetPIC[12].instrucFunc = &iorlwFun;
	
	//iorwf
	instructionSetPIC[13].mnemonic = new(VString, "iorwf", -1);
	instructionSetPIC[13].operand = FILE_D;
	instructionSetPIC[13].instrucFunc = &iorwfFun;
	
	//rlf
	instructionSetPIC[14].mnemonic = new(VString, "rlf", -1);
	instructionSetPIC[14].operand = FILE_D;
	instructionSetPIC[14].instrucFunc = &rlfFun;
	
	//rrf
	instructionSetPIC[15].mnemonic = new(VString, "rrf", -1);
	instructionSetPIC[15].operand = FILE_D;
	instructionSetPIC[15].instrucFunc = &rrfFun;
	
	//sublw
	instructionSetPIC[16].mnemonic = new(VString, "sublw", -1);
	instructionSetPIC[16].operand = K_LIT;
	instructionSetPIC[16].instrucFunc = &sublwFun;
	
	//subwf
	instructionSetPIC[17].mnemonic = new(VString, "subwf", -1);
	instructionSetPIC[17].operand = FILE_D;
	instructionSetPIC[17].instrucFunc = &subwfFun;
	
	//swapf
	instructionSetPIC[18].mnemonic = new(VString, "swapf", -1);
	instructionSetPIC[18].operand = FILE_D;
	instructionSetPIC[18].instrucFunc = &swapfFun;
	
	//xowlw
	instructionSetPIC[19].mnemonic = new(VString, "xowlw", -1);
	instructionSetPIC[19].operand = K_LIT;
	instructionSetPIC[19].instrucFunc = &xorlwFun;
	
	//xorwf
	instructionSetPIC[20].mnemonic = new(VString, "xorwf", -1);
	instructionSetPIC[20].operand = FILE_D;
	instructionSetPIC[20].instrucFunc = &xorwfFun;
	
	//movf
	instructionSetPIC[21].mnemonic = new(VString, "movf", -1);
	instructionSetPIC[21].operand = FILE_D;
	instructionSetPIC[21].instrucFunc = &movfFun;
	
	//goto
	instructionSetPIC[22].mnemonic = new(VString, "goto", -1);
	instructionSetPIC[22].operand = K_DIR;
	instructionSetPIC[22].instrucFunc = &gotoFun;
	
	//nop
	instructionSetPIC[23].mnemonic = new(VString, "nop", -1);
	instructionSetPIC[23].operand = EMPTY;
	instructionSetPIC[23].instrucFunc = &nopFun;
	
	//clrw
	instructionSetPIC[24].mnemonic = new(VString, "clrw", -1);
	instructionSetPIC[24].operand = EMPTY;
	instructionSetPIC[24].instrucFunc = &clrwFun;
	
	//decfsz
	instructionSetPIC[25].mnemonic = new(VString, "decfsz", -1);
	instructionSetPIC[25].operand = FILE_D;
	instructionSetPIC[25].instrucFunc = &decfszFun;
	
	//incfsz
	instructionSetPIC[26].mnemonic = new(VString, "incfsz", -1);
	instructionSetPIC[26].operand = FILE_D;
	instructionSetPIC[26].instrucFunc = &incfszFun;
	
	//btfsc
	instructionSetPIC[27].mnemonic = new(VString, "btfsc", -1);
	instructionSetPIC[27].operand = FILE_B;
	instructionSetPIC[27].instrucFunc = &btfscFun;
	
	//btfss
	instructionSetPIC[28].mnemonic = new(VString, "btfss", -1);
	instructionSetPIC[28].operand = FILE_B;
	instructionSetPIC[28].instrucFunc = &btfssFun;
	
	//call	
	instructionSetPIC[29].mnemonic = new(VString, "call", -1);
	instructionSetPIC[29].operand = K_DIR;
	instructionSetPIC[29].instrucFunc = &callFun;
	
	//return
	instructionSetPIC[30].mnemonic = new(VString, "return", -1);
	instructionSetPIC[30].operand = EMPTY;
	instructionSetPIC[30].instrucFunc = &returnFun;
}

//INSTRUCTIONS------------------------------------------------------------------------------------------
void movlwFun(BitField operandNum)
{
	setW((uint8_t)(operandNum & 0xFF));
}

void addlwFun(BitField operandNum)
{
	unsigned int addResult;
	uint8_t w = getW();
	uint8_t WNibble = (w & LOWER_NIBBLE_MASK) + (uint8_t)(operandNum & LOWER_NIBBLE_MASK);
	(WNibble > LOWER_NIBBLE_MASK) ? setDC(true) : setDC(false);

	addResult = (unsigned int)w + (unsigned int)operandNum;
	(addResult > 0xFF) ? setC(true) : setC(false);
	
	setW((uint8_t)(addResult & 0xFF));
	(getW() == 0) ? setZ(true) : setZ(false);
}

void movwfFun(BitField operandNum)
{
	setDataMemory((uint8_t)(operandNum & 0xFF), getW());
}

void clrfFun(BitField operandNum)
{
	setDataMemory((uint8_t)(operandNum & 0xFF), 0x00);
	setZ(true);
}

void andlwFun(BitField operandNum)
{
	setW(getW() & (uint8_t)(operandNum & 0xFF));
	(getW() == 0) ? setZ(true) : setZ(false);
}

void addwfFun(BitField operandNum)
{
	uint8_t dBit = operandNum & D_MASK;
	uint8_t file = operandNum & FILE_MASK; 
	uint8_t dataFile = getDataMemory(file);
	uint8_t w = getW();
	unsigned int addResult = w + dataFile;
	uint8_t addShort = (uint8_t)(addResult & 0xFF);

	uint8_t WNibble = (w & LOWER_NIBBLE_MASK) + (dataFile & LOWER_NIBBLE_MASK);
	(WNibble > LOWER_NIBBLE_MASK) ? setDC(true) : setDC(false);
	
	(addResult > 0xFF) ? setC(true) : setC(false);
	(addShort == 0) ? setZ(true) : setZ(false);
	(dBit) ? setDataMemory(file, addShort) : setW(addShort);
}

void andwfFun(BitField operandNum)
{
	uint8_t dBit = operandNum & D_MASK;
	uint8_t file = operandNum & FILE_MASK; 
	uint8_t dataFile = getDataMemory(file);
	uint8_t andResult = dataFile & getW();	

	(andResult == 0) ? setZ(true) : setZ(false);
	(dBit) ? setDataMemory(file, andResult) : setW(andResult);
}

void bcfFun(BitField operandNum)
{
	uint8_t bitSelect = (operandNum >> 0x07) & BIT_MASK;
	uint8_t file = operandNum & FILE_MASK; 
	uint8_t dataFile = getDataMemory(file);

	dataFile = dataFile & ~(0x01 << bitSelect);
	setDataMemory(file, dataFile);
}

void bsfFun(BitField operandNum)
{
	uint8_t bitSelect = (operandNum >> 0x07) & BIT_MASK;
	uint8_t file = operandNum & FILE_MASK; 
	uint8_t dataFile = getDataMemory(file);

	dataFile = dataFile | (0x01 << bitSelect);
	setDataMemory(file, dataFile);
}

void comfFun(BitField operandNum)
{
	uint8_t dBit = operandNum & D_MASK;
	uint8_t file = operandNum & FILE_MASK; 
	uint8_t dataFile = getDataMemory(file);

	dataFile = ~dataFile;
	(dataFile == 0) ? setZ(true) : setZ(false);
	(dBit) ? setDataMemory(file, dataFile) : setW(dataFile);
}

void decfFun(BitField operandNum)
{
	uint8_t dBit = operandNum & D_MASK;
	uint8_t file = operandNum & FILE_MASK; 
	uint8_t dataFile = getDataMemory(file);

	dataFile--;
	(dataFile == 0) ? setZ(true) : setZ(false);
	(dBit) ? setDataMemory(file, dataFile) : setW(dataFile);
}

void incfFun(BitField operandNum)
{
	uint8_t dBit = operandNum & D_MASK;
	uint8_t file = operandNum & FILE_MASK; 
	uint8_t dataFile = getDataMemory(file);

	dataFile++;
	(dataFile == 0) ? setZ(true) : setZ(false);
	(dBit) ? setDataMemory(file, dataFile) : setW(dataFile);
}

void iorlwFun(BitField operandNum)
{
	setW(getW() | (uint8_t)(operandNum & 0xFF));
	(getW() == 0) ? setZ(true) : setZ(false);
}

void iorwfFun(BitField operandNum)
{
	uint8_t dBit = operandNum & D_MASK;
	uint8_t file = operandNum & FILE_MASK; 
	uint8_t dataFile = getDataMemory(file);
	uint8_t orResult = dataFile | getW();	

	(orResult == 0) ? setZ(true) : setZ(false);
	(dBit) ? setDataMemory(file, orResult) : setW(orResult);
}

void rlfFun(BitField operandNum)
{
	uint8_t dBit = operandNum & D_MASK;
	uint8_t file = operandNum & FILE_MASK; 
	uint8_t dataFile = getDataMemory(file);
	uint8_t rotateResult = (dataFile << 1) | getC();

	(dataFile & 0x80) ? setC(true) : setC(false);
	(dBit) ? setDataMemory(file, rotateResult) : setW(rotateResult);
}

void rrfFun(BitField operandNum)
{
	uint8_t dBit = operandNum & D_MASK;
	uint8_t file = operandNum & FILE_MASK; 
	uint8_t dataFile = getDataMemory(file);
	uint8_t rotateResult = (dataFile >> 1) | (getC() << 0x07);

	(dataFile & 0x01) ? setC(true) : setC(false);
	(dBit) ? setDataMemory(file, rotateResult) : setW(rotateResult);
}

void sublwFun(BitField operandNum)
{
	uint16_t subResult = getW();
	subResult = ~subResult + 1;
	((getW() & LOWER_NIBBLE_MASK) <= (operandNum & LOWER_NIBBLE_MASK)) ? setDC(true) : setDC(false);

	subResult = (operandNum & 0xFF) + (subResult & 0xFF);
	(getW() <= (uint8_t)(operandNum & 0xFF)) ? setC(true) : setC(false);
	
	setW((uint8_t)(subResult & 0xFF));
	(getW() == 0) ? setZ(true) : setZ(false);
}

void subwfFun(BitField operandNum)
{
	uint8_t dBit = operandNum & D_MASK;
	uint8_t file = operandNum & FILE_MASK; 
	uint16_t dataFile = (uint16_t)getDataMemory(file);
	uint16_t subResult = getW();

	((getW() & LOWER_NIBBLE_MASK) <= (dataFile & LOWER_NIBBLE_MASK)) ? setDC(true) : setDC(false);

	subResult = ~subResult + 1;
	subResult = (dataFile & 0xFF) + (subResult & 0xFF);
	uint8_t subShort = (uint8_t)(subResult & 0xFF);
	(getW() <= (uint8_t)(dataFile & 0xFF)) ? setC(true) : setC(false);
	
	(dBit) ? setDataMemory(file, subShort) : setW(subShort);
	(subShort == 0) ? setZ(true) : setZ(false);
}

void swapfFun(BitField operandNum)
{
	uint8_t dBit = operandNum & D_MASK;
	uint8_t file = operandNum & FILE_MASK; 
	uint8_t dataFile = getDataMemory(file);
	uint8_t swapResult = ((dataFile & 0x0F) << 0x04) | ((dataFile & 0xF0) >> 0x04);	

	(dBit) ? setDataMemory(file, swapResult) : setW(swapResult);
}

void xorlwFun(BitField operandNum)
{
	setW(getW() ^ (uint8_t)(operandNum & 0xFF));
	(getW() == 0) ? setZ(true) : setZ(false);
}

void xorwfFun(BitField operandNum)
{
	uint8_t dBit = operandNum & D_MASK;
	uint8_t file = operandNum & FILE_MASK; 
	uint8_t dataFile = getDataMemory(file);
	uint8_t xorResult = dataFile ^ getW();	

	(xorResult == 0) ? setZ(true) : setZ(false);
	(dBit) ? setDataMemory(file, xorResult) : setW(xorResult);
}

void movfFun(BitField operandNum)
{
	uint8_t dBit = operandNum & D_MASK;
	uint8_t file = operandNum & FILE_MASK; 
	uint8_t dataFile = getDataMemory(file);
	
	(dataFile == 0) ? setZ(true) : setZ(false);
	(dBit) ? setDataMemory(file, dataFile) : setW(dataFile);
}

void gotoFun(BitField operandNum)
{
	static bool called;

	if(called == false){
		setPC(operandNum & 0x7FF);	
		called = true;
	}else
		called = false;
}

void nopFun(BitField operandNum)
{
	operandNum++;
}

void clrwFun(BitField operandNum)
{
	setW(0);
	setZ(true);
}

void decfszFun(BitField operandNum)
{
	uint8_t dBit = operandNum & D_MASK;
	uint8_t file = operandNum & FILE_MASK; 
	uint8_t dataFile = getDataMemory(file);
	static bool called;

	if(called == false){
		dataFile--;
		if(dataFile == 0)
			setPC(getPC() + 2);
		(dBit) ? setDataMemory(file, dataFile) : setW(dataFile);
		called = true;
	}else
		called = false;
}

void incfszFun(BitField operandNum)
{
	uint8_t dBit = operandNum & D_MASK;
	uint8_t file = operandNum & FILE_MASK; 
	uint8_t dataFile = getDataMemory(file);
	static bool called;

	if(called == false){
		dataFile++;
		if(dataFile == 0)
			setPC(getPC() + 2);
		(dBit) ? setDataMemory(file, dataFile) : setW(dataFile);
		called = true;
	}else
		called = false;
}

void btfscFun(BitField operandNum)
{
	uint8_t bitSelect = (operandNum >> 0x07) & BIT_MASK;
	uint8_t file = operandNum & FILE_MASK; 
	uint8_t dataFile = getDataMemory(file);
	static bool called;

	if(called == false){
		if(!(dataFile & (0x01 << bitSelect)))
			setPC(getPC() + 2);
		called = true;
	}else
		called = false;

}

void btfssFun(BitField operandNum)
{
	uint8_t bitSelect = (operandNum >> 0x07) & BIT_MASK;
	uint8_t file = operandNum & FILE_MASK; 
	uint8_t dataFile = getDataMemory(file);
	static bool called;

	if(called == false){
		if(dataFile & (0x01 << bitSelect))
			setPC(getPC() + 2);
		called = true;
	}else
		called = false;

}

void callFun(BitField operandNum)
{
	static bool called;
	uint16_t valuePC = getPC() + 1;

	if(called == false){
		stackPush(valuePC);
		setPC(operandNum & 0x7FF);	
		called = true;
	}else
		called = false;

//	printStack();
}

void returnFun(BitField operandNum)
{
	static bool called;

	if(called == false){
		operandNum = stackPop();
		setPC(operandNum & 0x7FF);	
		called = true;
	}else
		called = false;
	
//	printStack();
}
