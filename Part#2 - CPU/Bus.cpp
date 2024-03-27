#include "Bus.h"
#include "MainBoard.h"
#include "olc2C02.h"

Bus::Bus(MainBoard& mBoard) : nes(mBoard)
{
	// Connect CPU to communication bus
	//mainBoard.cpu.ConnectBus(this);
	
	// Clear RAM contents, just in case :P
	for (auto &i : nes.cpuRam) i = 0x00;
}


Bus::~Bus()
{
}

void Bus::cpuWrite(uint16_t addr, uint8_t data)
{
	if (addr >= 0x0000 && addr <= 0x1FFF) {
		nes.cpuRam[addr & 0x07FF] = data;
	}
	else if (addr >= 0x2000 && addr <= 0x3FFF) {
		nes.getPpu()->cpuWrite(addr & 0x0007, data);
	}
}

uint8_t Bus::cpuRead(uint16_t addr, bool bReadOnly)
{
	uint8_t data = 0x00;

	if (addr >= 0x0000 && addr <= 0x1FFF) {

		data = nes.cpuRam[addr & 0x07FF];
	}
	else if (addr >= 0x2000 && addr <= 0x3FFF) {

		data = nes.getPpu()->cpuRead(addr & 0x0007, bReadOnly);
	}

	return data;
}
