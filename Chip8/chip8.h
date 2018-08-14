#include <stdio.h>
#include <string>
#include <time.h>

#pragma once

using namespace std;

#ifndef CHIP8_H
#define CHIP8_H

class chip8
{
public:
	chip8(void);
	~chip8(void);

	void initialize(void);
	void emulateCycle(void);
	bool loadGame(const char *);
	unsigned static char chip8_fontset[80];
	unsigned static char gfx[64 * 32];
	bool drawFlag;
	char getDelayTimer(void);
	void setKeys(void);
	void setKeys(char);
	char c; //For key input

private:
	unsigned char memory[4096];
	unsigned char V[16];
	unsigned short InsReg;
	unsigned short pc;
	unsigned short opcode;
	unsigned short stack[16];
	unsigned short sp;
	unsigned char delay_timer;
	unsigned char sound_timer;
	unsigned char key[16];
	unsigned short randNr;


};

#endif