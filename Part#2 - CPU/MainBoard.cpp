#include "MainBoard.h"
#include "olc6502.h"
#include "Bus.h"

MainBoard::MainBoard()
{
	bus = std::make_unique<Bus>(*this);
	cpu = std::make_unique<olc6502>(*this);
}

MainBoard::~MainBoard()
{
}
