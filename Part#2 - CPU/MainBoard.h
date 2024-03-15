#pragma once
#include <memory>


class Bus;
class olc6502;

class MainBoard
{
public:
	MainBoard();
	~MainBoard();

	std::unique_ptr<Bus> bus;
	std::unique_ptr<olc6502> cpu;

};

