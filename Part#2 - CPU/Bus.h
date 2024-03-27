#pragma once
#include <cstdint>
#include <array>

class MainBoard;

class Bus
{
public:
	Bus(MainBoard& mBoard);
	~Bus();


public: // Bus Read & Write
	void cpuWrite(uint16_t addr, uint8_t data);
	uint8_t cpuRead(uint16_t addr, bool bReadOnly = false);

private:
	MainBoard& nes;
};

