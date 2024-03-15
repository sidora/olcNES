#pragma once
#include <memory>

class Bus;
class olc6502;

class MainBoard
{
public:
	MainBoard() {};
	~MainBoard() {};
	void setBus(Bus* bus_) { bus = bus_; };
	void setCpu(olc6502* cpu_) { cpu = cpu_; };
	Bus* getBus() { return bus; };
	olc6502* getCpu() { return cpu; };

private:
	Bus* bus;
	olc6502* cpu;

};

