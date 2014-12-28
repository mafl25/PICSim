#ifndef REG_AND_MEM
#define REG_AND_MEM

#include <stdint.h>
#include <stdbool.h>

#define LAST_RAM_ADDRESS 0x0A

void setDataMemory(uint8_t address, uint8_t value);
uint8_t getDataMemory(uint8_t address);

void resetDataMemory(void);

void incrementPC(void);
void setPC(uint16_t value);
void resetPC(void);//this is mainly to be used by other parts of the program, not the 
							 //emulator itself, it basically overrides setPC and the mechanisms asociated
							 //with it.
uint16_t getPC(void);

void stackPush(uint16_t);
uint16_t stackPop();
uint8_t stackIsEmpty();
uint8_t stackIsFull();
void printStack();

void setW(uint8_t value);
uint8_t getW(void);

void setC(bool status);
uint8_t getC(void);

void setDC(bool status);
uint8_t getDC(void);

void setZ(bool status);
uint8_t getZ(void);

#endif
