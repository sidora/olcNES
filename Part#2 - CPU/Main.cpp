#include <memory>
#include "olcNes_Video1_6502.h"
#include "olcPixelGameEngine.h"
#include "MainBoard.h"
#include "olc6502.h"
#include "olc2C02.h"
#include "Bus.h"


int main()
{
	
	MainBoard nes;
	std::unique_ptr<Bus> bus = std::make_unique<Bus>(nes);
	std::unique_ptr<olc6502> cpu = std::make_unique<olc6502>(nes);
	std::unique_ptr<olc2C02> ppu = std::make_unique<olc2C02>(nes);

	nes.setBus(bus.get());
	nes.setCpu(cpu.get());
	nes.setPpu(ppu.get());

	Demo_olc6502 demo(nes);
	demo.Construct(680, 480, 2, 2);
	demo.Start();
	return 0;
}