#include <iostream>
#include "chip8.h"
#include "SDLdrawer.h"

chip8 myChip8;
SDLdrawer myDrawer;

int main(int argc, char* argv[]){
	myChip8.initialize();
	myDrawer.setupScreen();
	std::string name = "E:\\Game\\Chip8\\pong2";
	const char * nameToPass = name.c_str();
	if (myChip8.loadGame(nameToPass))
		cout << "Game loaded " << name;
	myDrawer.drawGraphics();

	// Emulation loop
	for(;;)
	{
		// Emulate one cycle
		myChip8.emulateCycle();
 
		// If the draw flag is set, update the screen
		if(myChip8.drawFlag)
			myDrawer.drawGraphics();
		myChip8.drawFlag = false;

		// Store key press state (Press and Release)
		myChip8.setKeys();	
	}

	return 0;
}