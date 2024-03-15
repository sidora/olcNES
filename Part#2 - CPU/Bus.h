#pragma once
#include <cstdint>
#include <array>
#include <memory>

class MainBoard;

class Bus
{
public:
	Bus(MainBoard& mBoard);
	~Bus();

public: // Devices on bus
	

	// Fake RAM for this part of the series
	std::array<uint8_t, 64 * 1024> ram;


public: // Bus Read & Write
	void write(uint16_t addr, uint8_t data);
	uint8_t read(uint16_t addr, bool bReadOnly = false);

private:
	MainBoard& nes;
};

