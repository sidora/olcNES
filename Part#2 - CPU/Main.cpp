#include "olcNes_Video1_6502.h"
#include "olcPixelGameEngine.h"



int main()
{
	

	Demo_olc6502 demo;
	demo.Construct(680, 480, 2, 2);
	demo.Start();
	return 0;
}