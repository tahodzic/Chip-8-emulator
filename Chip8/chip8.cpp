#include "chip8.h"
#include <windows.h>
#include <conio.h>

chip8::chip8(void)
{

}


chip8::~chip8(void)
{
}

void chip8::initialize(void){
	// Initialize registers and memory once
	pc = 0x200;
	opcode = 0;
	InsReg = 0;
	sp = 0;
	
	// Clear display
	for (int i = 0; i < 2048; ++i)
		gfx[i] = 0;

	// Clear stack
	for (int i = 0; i < 16; ++i)
		stack[i] = 0;

	// Clear registers V0-VF
	for (int i = 0; i < 16; ++i)
		V[i] = 0;

	// Clear memory
	for (int i = 0; i < 4096; ++i)
		memory[i] = 0;

    // Load fontset
	for(int i = 0; i < 80; ++i)
		memory[i] = chip8_fontset[i];	

	//reset timers
	delay_timer = 0;
	sound_timer = 0;

	//set drawFlag to false
	drawFlag = false;

	//do 1 time srand
	srand(time(NULL));
}

void chip8::emulateCycle(void){
	// Fetch Opcode
	Sleep(8);
	opcode = memory[pc] << 8 | memory[pc + 1];
	//printf("pc: 0x%X\topcode:%X\n", pc, opcode);

	switch(opcode & 0xF000){   
		case 0x0000:
			switch(opcode &0x0FFF){
			case 0x0E0: //00E0 	Display 	disp_clear() 	Clears the screen.
			{
				for (int i = 0; i < 2048; ++i)
					gfx[i] = 0;
				drawFlag = true;
				pc += 2;
			}
			break;
		
			case 0x0EE: //00EE 	Flow 	return; 	Returns from a subroutine.
			{
				--sp;
				pc = stack[sp];
				stack[sp] = 0;
				pc += 2;
				
			}
			break;
		
			default: //0NNN
				printf("unknown opcode: 0x%X\n", pc);
			break;
			}
		break;
	
		case 0x1000: //1NNN, jmp to nnn
		{
			pc = opcode & 0x0FFF;
		}
		break;

		case 0x2000: //2NNN, call nnn
		{
			stack[sp] = pc;
			sp++;
			pc = opcode & 0x0FFF;
		}
		break;

		case 0x3000: //3XNN 
		{
			//Skip next instruction if VX == NN
			if(V[(opcode & 0x0F00)>>8] == (opcode & 0x00FF)){
				pc += 4;
			}
			else
				pc += 2;
		}
		break;

		case 0x4000: //4XNN
		{
			//Skip next instruction if VX != NN
			if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)){
				pc += 4;
			}
			else
				pc += 2;
		}
		break;

		case 0x5000: //5XY0
		{
			//Skip next instruction if VX == VY
			if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]){
				pc += 4;
			}
			else 
				pc += 2;
		}
		break;

		case 0x6000: //6XNN, Sets VX to NN.
		{
			V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
			pc += 2;
		}
		break;

		case 0x7000: //7XNN, Adds NN to VX
		{
			V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
			pc += 2;
		}
		break;

		case 0x8000:
			switch(opcode & 0x000F){
				case 0x0000://8XY0 	Assign 	Vx=Vy 	Sets VX to the value of VY.
				{
					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
					pc += 2;
				}
				break;
		
				case 0x0001://8XY1 	BitOp 	Vx=Vx|Vy 	Sets VX to VX or VY. (Bitwise OR operation)
				{
					V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
					pc += 2;
				}
				break;
		
				case 0x0002://8XY2 	BitOp 	Vx=Vx&Vy 	Sets VX to VX and VY. (Bitwise AND operation)
				{
					V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
					pc += 2;
				}
				break;
		
				case 0x0003://8XY3 	BitOp 	Vx=Vx^Vy 	Sets VX to VX xor VY.
				{
					V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
					pc += 2;
				}
				break;
		
				case 0x0004://8XY4 	Math 	Vx += Vy 	Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
				{
					if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
						V[0xF] = 1; //carry
					else
						V[0xF] = 0;
					V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
					pc += 2;
				}
				break;
		
				case 0x0005://8XY5 	Math 	Vx -= Vy 	VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
				{
					if(V[(opcode & 0x0F00) >> 8] < V[(opcode & 0x00F0) >> 4])
						V[0xF] = 1;
					else
						V[0xF] = 0;
					V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
					pc += 2;
				}
				break;
		
				case 0x0006://8XY6 	BitOp 	Vx >> 1 	Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.[2]
				{
					V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] >> 1;
					pc += 2;
				}
				break;
		
				case 0x0007: //8XY7 	Math 	Vx=Vy-Vx 	Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
				{
					if(V[(opcode & 0x00F0) >> 4] < V[(opcode & 0x0F00) >> 8] )
						V[0xF] = 1;
					else
						V[0xF] = 0;
					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
					pc += 2;
				}
				break;
		
				case 0x000E://8XYE 	BitOp 	Vx << 1 	Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.[2]
				{
					V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x8;
					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] << 1;
					pc += 2;
				}
				break;
		
				default:
				break;
			}
		break;

		case 0x9000: //9XY0 	Cond 	if(Vx!=Vy) 	Skips the next instruction if VX doesn't equal VY. (Usually the next instruction is a jump to skip a code block)
		{
			switch(opcode & 0xF00F){
				case 0x9000:
				{
					if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
						pc += 4;
					else
						pc += 2;
				}
				break;
			}
		}
		break;

		case 0xA000: // ANNN: Sets I to the address NNN
		{
			// Execute opcode
			InsReg = opcode & 0x0FFF;
			pc += 2;
		}	
		break;

		case 0xB000: //BNNN 	Flow 	PC=V0+NNN 	Jumps to the address NNN plus V0.
		{
			pc = (V[0x0] + (opcode & 0x0FFF));
		}
		break;

		case 0xC000://CXNN 	Rand 	Vx=rand()&NN 	Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
		{
			randNr = rand() % 255;
			V[(opcode & 0x0F00) >> 8] = (randNr & (opcode & 0x00FF));
			pc += 2;
		}
		break;

		case 0xD000: //DXYN
		{
			unsigned short x = V[(opcode & 0x0F00) >> 8];
			unsigned short y = V[(opcode & 0x00F0) >> 4];
			unsigned short height = opcode & 0x000F;
			unsigned short pixel;
 
			V[0xF] = 0;
			for (int yline = 0; yline < height; yline++)
			{
				pixel = memory[InsReg + yline];
				for(int xline = 0; xline < 8; xline++)
				{
					if((pixel & (0x80 >> xline)) != 0)
					{
					if(gfx[(x + xline + ((y + yline) * 64))] == 1)
						V[0xF] = 1;                                 
					gfx[x + xline + ((y + yline) * 64)] ^= 1;
					}
				}
			}
			//printf("About to draw.\n");
			drawFlag = true;
			pc += 2;
		}
		break;

		case 0xE000:
			switch(opcode & 0x00FF){
				case 0x009E: //EX9E KeyOp 	if(key()==Vx) 
				{
					if (key[V[(opcode & 0x0F00) >> 8]] != 0) {
						pc += 4;
					}
					else
						pc += 2;
				}
				break;
		
				case 0x00A1://EXA1 	KeyOp 	if(key()!=Vx) 	
				{
					if(key[V[(opcode & 0x0F00) >> 8]] == 0)
						pc += 4;
					else
						pc += 2;
					
					/*for(int i = 0; i < 16; ++i){
						key[i] = 0;
					}*/
				}
				break;
		
				default:
				break;
		
			}
		break;

		case 0xF000: 
			switch(opcode & 0x00FF){
				case 0x0007: //FX07 	Timer 	Vx = get_delay() 	Sets VX to the value of the delay timer.
				{
					V[(opcode & 0x0F00) >> 8] = getDelayTimer();
					pc+= 2;
				}
				break;
				
				//TODO: Find a way to not use the overloaded function setKeys(x)
				//TODO: Fix that upon one key press the programm thinks the key is being held, resulting in endless movement
				case 0x000A:  //FX0A 	KeyOp 	Vx = get_key() 	A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)
				{
					char x;
					scanf_s("%c", &x,1);
					V[(opcode & 0x0F00) >> 8] = x;
					setKeys(x);
					pc += 2;
				}
				break;
		
				case 0x0015: //FX15 	Timer 	delay_timer(Vx) 	Sets the delay timer to VX.
				{
					delay_timer = V[(opcode & 0x0F00) >> 8];
					pc += 2;
				}
				break;
		
				case 0x0018: //FX18 	Sound 	sound_timer(Vx) 	Sets the sound timer to VX.
				{
					sound_timer = V[(opcode & 0x0F00) >> 8];
					pc += 2;
				}
				break;
		
				case 0x001E: //FX1E 	MEM 	I +=Vx 	Adds VX to I.[3]
				{
					if ((InsReg + V[(opcode & 0x0F00) >> 8]) > 0xFFF)
						V[0xF] = 1;
					else 
						V[0xF] = 0;
					InsReg += V[(opcode & 0x0F00) >> 8];
					pc += 2;
				}
				break;
		
				case 0x0029: //FX29 	MEM 	I=sprite_addr[Vx] 	Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
				{
					InsReg = 5*V[(opcode & 0x0F00) >> 8];
					pc += 2;
				}
				break;
		
				case 0x0033:
				{
					memory[InsReg]     = V[(opcode & 0x0F00) >> 8] / 100;
					memory[InsReg + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
					memory[InsReg+ 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
					pc += 2;
				}
				break;

				case 0x0055:
				{
					int max = ((opcode & 0x0F00) >> 8);
					for(int count = 0; count <= max; ++count){
						memory[InsReg+count] = V[count];
					}
					pc += 2;
				}
				break;

				case 0x0065:
				{
					int max = ((opcode & 0x0F00) >> 8);
					for(int count = 0; count <= max; ++count){
						 V[count] = memory[InsReg+count];
					}
					pc += 2;
				}
				break;
		
				default:
					printf ("Unknown opcode: 0x%X\n", opcode);
					pc += 2;
				break;
			}
		break;
		default:
			printf ("Unknown opcode: 0x%X\n", opcode);
			pc += 2;
		break;
	}

   //Update timers

  if(delay_timer > 0)
    --delay_timer;
 
  if(sound_timer > 0)
  {
    if(sound_timer == 1)
      printf("BEEP!\n");
    --sound_timer;
  } 
}

bool chip8::loadGame(const char * name){
	FILE * pGame;
	unsigned char buffer[2048];
	//int n;

	errno_t err = fopen_s(&pGame, name, "rb");
	if (pGame){
		fread(buffer, 2048, 1, pGame);
	}
	else {
		printf("Couldn't load game.\n");
		return false;
	}

	for(int i = 0; i < sizeof(buffer); ++i)
		memory[i + 512] = buffer[i];

	return true;
}

char chip8::getDelayTimer(void){
	return delay_timer;
}

void chip8::setKeys(void){
	if (_kbhit()){

		c = _getch();
		printf("%c is being pressed\n", c);
		switch(c){
		case '1':
			{
				key[0x01] = 1;
				break;
			}
		case '2':
			{
				key[0x02] = 1;
				break;
			}
		case '3':
			{
				key[0x03] = 1;
				break;
			}
		case '4': 
			{
				key[0x0c] = 1;
				break;
			}
		case 'q':
			{
				key[0x04] = 1;
				break;
			}
		case 'w': 
			{
				key[0x05] = 1;
				break;
			}
		case 'e':
			{
				key[0x06] = 1;
				break;
			}
		case 'r':
			{
				key[0x0D] = 1;
				break;
			}
		case 'a':
			{
				key[0x07] = 1;
				break;
			}
		case 's':
			{
				key[0x08] = 1;
				break;
			}
		case 'd':
			{
				key[0x09] = 1;
				break;
			}
		case 'f':
			{
				key[0x0E] = 1;
				break;
			}
		case 'y':
			{
				key[0x0A] = 1;
				break;
			}
		case 'x':
			{
				key[0x00] = 1;
				break;
			}
		case 'c':
			{
				key[0x0B] = 1;
				break;
			}
		case 'v':
			{
				key[0x0F] = 1;
				break;
			}
		}
	}
	/*else {
		for(int i = 0; i < 16; ++i){
			key[i] = 0;
		}
	}*/
}

//function is (as of now) still needed, but there should be a better way (see comment at opcode FX0A)
void chip8::setKeys(char x){
		switch(x){
		case '1':
			{
				key[0x01] = 1;
				break;
			}
		case '2':
			{
				key[0x02] = 1;
				break;
			}
		case '3':
			{
				key[0x03] = 1;
				break;
			}
		case '4': 
			{
				key[0x0c] = 1;
				break;
			}
		case 'q':
			{
				key[0x04] = 1;
				break;
			}
		case 'w': 
			{
				key[0x05] = 1;
				break;
			}
		case 'e':
			{
				key[0x06] = 1;
				break;
			}
		case 'r':
			{
				key[0x0D] = 1;
				break;
			}
		case 'a':
			{
				key[0x07] = 1;
				break;
			}
		case 's':
			{
				key[0x08] = 1;
				break;
			}
		case 'd':
			{
				key[0x09] = 1;
				break;
			}
		case 'f':
			{
				key[0x0E] = 1;
				break;
			}
		case 'y':
			{
				key[0x0A] = 1;
				break;
			}
		case 'x':
			{
				key[0x00] = 1;
				break;
			}
		case 'c':
			{
				key[0x0B] = 1;
				break;
			}
		case 'v':
			{
				key[0x0F] = 1;
				break;
			}
		}
}

unsigned char chip8::chip8_fontset[] =
		{ 
		  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		  0x20, 0x60, 0x20, 0x20, 0x70, // 1
		  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
		};

unsigned char  chip8::gfx[64 * 32];