
#include "olcNes_Video1_6502.h"


Demo_olc6502::Demo_olc6502(MainBoard& mBoard) : nes(mBoard) {
	
	sAppName = "olc6502 Demonstration"; 
	txtCol = olc::Pixel(200, 200, 200);

};

std::string Demo_olc6502::hex(uint32_t n, uint8_t d) {
	std::string s(d, '0');
	for (int i = d - 1; i >= 0; i--, n >>= 4)
		s[i] = "0123456789ABCDEF"[n & 0xF];
	return s;
};

void Demo_olc6502::DrawRam(int x, int y, uint16_t nAddr, int nRows, int nColumns)
{
	int nRamX = x, nRamY = y;
	for (int row = 0; row < nRows; row++)
	{
		std::string sOffset = "$" + hex(nAddr, 4) + ":";
		for (int col = 0; col < nColumns; col++)
		{
			sOffset += " " + hex(nes.cpuRead(nAddr, true), 2);
			nAddr += 1;
		}
		DrawString(nRamX, nRamY, sOffset);
		nRamY += 10;
	}
};

void Demo_olc6502::DrawCpu(int x, int y)
{
	olc::Pixel onCol = olc::GREEN;
	olc::Pixel ofCol = olc::Pixel(0,0,0);

	int Ofst = 80;
	int Spce = 14;
	
	DrawString(x, y, "  FLAGS -", txtCol);
	DrawString(x + Ofst + (Spce*0), y, "N", nes.getCpu()->status & olc6502::N ? onCol : ofCol);
	DrawString(x + Ofst + (Spce*1), y, "V", nes.getCpu()->status & olc6502::V ? onCol : ofCol);
	DrawString(x + Ofst + (Spce*2), y, "B", nes.getCpu()->status & olc6502::B ? onCol : ofCol);
	DrawString(x + Ofst + (Spce*3), y, "I", nes.getCpu()->status & olc6502::I ? onCol : ofCol);
	DrawString(x + Ofst + (Spce*4), y, "Z", nes.getCpu()->status & olc6502::Z ? onCol : ofCol);
	DrawString(x + Ofst + (Spce*5), y, "C", nes.getCpu()->status & olc6502::C ? onCol : ofCol);

	DrawString(x, y + 10, "     PC - $" + hex(nes.getCpu()->pc, 4), txtCol);
	DrawString(x, y + 20, "      A - $" + hex(nes.getCpu()->a, 2) + "  [" + std::to_string(nes.getCpu()->a) + "]", txtCol);
	DrawString(x, y + 30, "      X - $" + hex(nes.getCpu()->x, 2) + "  [" + std::to_string(nes.getCpu()->x) + "]", txtCol);
	DrawString(x, y + 40, "      Y - $" + hex(nes.getCpu()->y, 2) + "  [" + std::to_string(nes.getCpu()->y) + "]", txtCol);
	DrawString(x, y + 50, "Stack P - $" + hex(nes.getCpu()->stkp, 4), txtCol);
}


void Demo_olc6502::DrawCode(int x, int y, int nLines)
{
		auto it_a = mapAsm.find(nes.getCpu()->pc);
		int nLineY = (nLines >> 1) * 10 + y;
		if (it_a != mapAsm.end())
		{
			DrawString(x, nLineY, (*it_a).second, olc::CYAN);
			while (nLineY < (nLines * 10) + y)
			{
				nLineY += 10;
				if (++it_a != mapAsm.end())
				{
					DrawString(x, nLineY, (*it_a).second, txtCol);
				}
			}
		}

		it_a = mapAsm.find(nes.getCpu()->pc);
		nLineY = (nLines >> 1) * 10 + y;
		if (it_a != mapAsm.end())
		{
			while (nLineY > y)
			{
				nLineY -= 10;
				if (--it_a != mapAsm.end())
				{
					DrawString(x, nLineY, (*it_a).second, txtCol);
				}
			}
		}
};


bool Demo_olc6502::OnUserCreate()
{
	// Load the cartridge
	cart = std::make_shared<Cartridge>("../nestest.nes");
	if (!cart->ImageValid())
		return false;

	// Insert into NES
	nes.insertCartridge(cart);

	// Extract dissassembly
	mapAsm = nes.getCpu()->disassemble(0x0000, 0xFFFF);

	// Reset NES
	nes.getCpu()->reset();
	return true;
};

bool Demo_olc6502::OnUserUpdate(float fElapsedTime)
{
	
	Clear(olc::Pixel(40, 40, 40));

	
	
	if (bEmulationRun)
	{
		if (fResidualTime > 0.0f)
			fResidualTime -= fElapsedTime;
		else
		{
			fResidualTime += (1.0f / 59.94f) - fElapsedTime;
			do { nes.clock(); } while (!nes.getPpu()->frame_complete);
			nes.getPpu()->frame_complete = false;
		}
	}
	else
	{
		// Emulate code step-by-step
		if (GetKey(olc::Key::C).bPressed)
		{
			// Clock enough times to execute a whole CPU instruction
			do { nes.clock(); } while (!nes.getCpu()->complete());
			// CPU clock runs slower than system clock, so it may be
			// complete for additional system clock cycles. Drain
			// those out
			do { nes.clock(); } while (nes.getCpu()->complete());
		}

		// Emulate one whole frame
		if (GetKey(olc::Key::F).bPressed)
		{
			// Clock enough times to draw a single frame
			do { nes.clock(); } while (!nes.getPpu()->frame_complete);
			// Use residual clock cycles to complete current instruction
			do { nes.clock(); } while (!nes.getCpu()->complete());
			// Reset frame completion flag
			nes.getPpu()->frame_complete = false;
		}
	}


	if (GetKey(olc::Key::SPACE).bPressed) bEmulationRun = !bEmulationRun;
	if (GetKey(olc::Key::R).bPressed) nes.reset();

	DrawCpu(516, 2);
	DrawCode(516, 72, 26);

	DrawSprite(0, 0, &nes.getPpu()->GetScreen(), 2);
	return true;

};







