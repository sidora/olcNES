#pragma once
#include <memory>
#include <cstdint>
#include <array>

class Bus;
class olc6502;
class olc2C02;
class Cartridge;

class MainBoard
{
public:
	MainBoard();
	~MainBoard();

	void setCpu(olc6502* cpu_) { cpu = cpu_; };
	void setPpu(olc2C02* ppu_) { ppu = ppu_; };

	olc6502* getCpu() { return cpu; };
	olc2C02* getPpu() { return ppu; };

	void cpuWrite(uint16_t addr, uint8_t data);
	uint8_t cpuRead(uint16_t addr, bool bReadOnly = false);

	std::array<uint8_t, 2048> cpuRam;
	// Controllers
	uint8_t controller[2];

// System Interface
	// Connects a cartridge object to the internal buses
	void insertCartridge(const std::shared_ptr<Cartridge>& cartridge);
	// Resets the system
	void reset();
	// Clocks the system - a single whole systme tick
	void clock();


private:
	olc6502* cpu;
	olc2C02* ppu;
	std::shared_ptr<Cartridge> cart;



	// A count of how many clocks have passed
	uint32_t nSystemClockCounter = 0;
	// Internal cache of controller state
	uint8_t controller_state[2];

};

