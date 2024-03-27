#include "MainBoard.h"
#include "olc6502.h"
#include "olc2C02.h"
#include "Bus.h"

MainBoard::MainBoard()
{
	
}

MainBoard::~MainBoard()
{
}

void MainBoard::insertCartridge(const std::shared_ptr<Cartridge>& cartridge)
{
	// Connects cartridge to both Main Bus and CPU Bus
	this->cart = cartridge;
	ppu->ConnectCartridge(cartridge);
}

void MainBoard::reset()
{
	cpu->reset();
	nSystemClockCounter = 0;
}

void MainBoard::clock()
{
	// Clocking. The heart and soul of an emulator. The running
	// frequency is controlled by whatever calls this function.
	// So here we "divide" the clock as necessary and call
	// the peripheral devices clock() function at the correct
	// times.

	// The fastest clock frequency the digital system cares
	// about is equivalent to the PPU clock. So the PPU is clocked
	// each time this function is called.
	ppu->clock();

	// The CPU runs 3 times slower than the PPU so we only call its
	// clock() function every 3 times this function is called. We
	// have a global counter to keep track of this.
	if (nSystemClockCounter % 3 == 0)
	{
		cpu->clock();
	}

	nSystemClockCounter++;
}
