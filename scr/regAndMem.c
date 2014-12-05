#include "regAndMem.h"
#include <stdio.h>
#include <stdlib.h>

#define STATUS_REG 0
#define C 0x01
#define DC 0x02
#define Z 0x04
#define LAST_RAM_ADDRESS 0x0A
#define STACK_SIZE 0x8


//Data memory as array of bytes
static uint8_t dataMemory[LAST_RAM_ADDRESS + 1];

//stack for functions
struct stack
{
	uint16_t callStack[STACK_SIZE];
	int top;
};
static struct stack picStack;

//W register as an 8 bit register
static uint8_t W;

//PC as 16 bit long word
static uint16_t PC;
static uint16_t PCTemp;
static bool writtenPC;
static uint8_t incIgnore;

void setDataMemory(uint8_t address, uint8_t value)
{
	dataMemory[address & 0x7F] = value;
}

uint8_t getDataMemory(uint8_t address)
{
	return dataMemory[address & 0x7F];
}

void incrementPC(void)
{

/*	if(writtenPC == ONE)
		writtenBefore = writtenPC;
	else if(writtenPC == ZERO && writtenBefore == ONE)
		writtenBefore = writtenPC;
	else
		PC++;*/


	if(incIgnore == 1 && writtenPC == true){
		PC = PCTemp;
		writtenPC = false;
		incIgnore--;
	}else if(incIgnore == 0)
		PC++;

//	printf("incIgnore: %d\n", incIgnore);
	if(writtenPC == true)
		incIgnore--;

	if(incIgnore < 0){
		printf("incIgnore es menor a 0. Revisar el soource code\n");
		exit(1);
	}
}

void setPC(uint16_t value)
{
/*	if(writtenPC == false)
		writtenPC = ONE;
	else{
		PC = value;
		writtenPC = ZERO;
	}*/
	incIgnore = 2;
	writtenPC = true;
	PCTemp = value;
}

uint16_t getPC(void)
{
	return PC;
}

void setW(uint8_t value)
{
	W = value;
}

uint8_t getW(void)
{
	return W;
}

void setC(bool status)
{
	if(status == true)
		setDataMemory(STATUS_REG, (getDataMemory(STATUS_REG) | C));
	else if (status == false)
		setDataMemory(STATUS_REG, (getDataMemory(STATUS_REG) & ~C));
}

uint8_t getC(void)
{
	return (getDataMemory(STATUS_REG) & C) ? 1 : 0;
}

void setDC(bool status)
{
	if(status == true)
		setDataMemory(STATUS_REG, (getDataMemory(STATUS_REG) | DC));
	else if (status == false)
		setDataMemory(STATUS_REG, (getDataMemory(STATUS_REG) & ~DC));
}

uint8_t getDC(void)
{
	return (getDataMemory(STATUS_REG) & DC) ? 1 : 0;
}

void setZ(bool status)
{
	if(status == true)
		setDataMemory(STATUS_REG, (getDataMemory(STATUS_REG) | Z));
	else if (status == false)
		setDataMemory(STATUS_REG, (getDataMemory(STATUS_REG) & ~Z));
}

uint8_t getZ(void)
{
	return (getDataMemory(STATUS_REG) & Z) ? 1 : 0;
}

void stackPush(uint16_t value)
{
	uint16_t tempValue;
	int j;

	if(!stackIsFull()){
		picStack.callStack[picStack.top] = value;
		picStack.top++;
	}else{
		for (j = 0; j < STACK_SIZE - 1; j++){
			tempValue = picStack.callStack[j];
			picStack.callStack[j] = picStack.callStack[j + 1];
		}
		picStack.callStack[j] = value;
	}
}

uint16_t stackPop()
{
	uint16_t value;

	if(!stackIsEmpty()){
		picStack.top--;
		value = picStack.callStack[picStack.top];
		picStack.callStack[picStack.top] = 0;
	}else
		value = 0;

		return value;
}

uint8_t stackIsFull()
{
	return (picStack.top == (STACK_SIZE)) ? 1 : 0;
}

uint8_t stackIsEmpty()
{
	return (picStack.top) ? 0 : 1;
}

void printStack()
{
	int j;

	for (j = 0; j < STACK_SIZE; j++){
		printf("Stack[%d]: 0x%X\n", j, picStack.callStack[j]);
	}
}
