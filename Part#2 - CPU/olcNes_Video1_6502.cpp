
#include "olcNes_Video1_6502.h"


Demo_olc6502::Demo_olc6502(MainBoard& mBoard) : nes(mBoard) {
	
	sAppName = "olc6502 Demonstration"; 

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
			sOffset += " " + hex(nes.getBus()->cpuRead(nAddr, true), 2);
			nAddr += 1;
		}
		DrawString(nRamX, nRamY, sOffset);
		nRamY += 10;
	}
};

void Demo_olc6502::DrawCpu(int x, int y)
{
		std::string status = "STATUS: ";
		DrawString(x, y, "STATUS:", olc::WHITE);
		DrawString(x +    64, y, "N", nes.getCpu()->status & olc6502::N ? olc::GREEN : olc::RED);
		DrawString(x +    80, y, "V", nes.getCpu()->status & olc6502::V ? olc::GREEN : olc::RED);
		DrawString(x +    96, y, "-", nes.getCpu()->status & olc6502::U ? olc::GREEN : olc::RED);
		DrawString(x +   112, y, "B", nes.getCpu()->status & olc6502::B ? olc::GREEN : olc::RED);
		DrawString(x +   128, y, "D", nes.getCpu()->status & olc6502::D ? olc::GREEN : olc::RED);
		DrawString(x +   144, y, "I", nes.getCpu()->status & olc6502::I ? olc::GREEN : olc::RED);
		DrawString(x +   160, y, "Z", nes.getCpu()->status & olc6502::Z ? olc::GREEN : olc::RED);
		DrawString(x +   178, y, "C", nes.getCpu()->status & olc6502::C ? olc::GREEN : olc::RED);
		DrawString(x, y + 10, "PC: $"      + hex(nes.getCpu()->pc, 4));
		DrawString(x, y + 20, "A: $"       + hex(nes.getCpu()->a, 2) + "  [" + std::to_string(nes.getCpu()->a) + "]");
		DrawString(x, y + 30, "X: $"       + hex(nes.getCpu()->x, 2) + "  [" + std::to_string(nes.getCpu()->x) + "]");
		DrawString(x, y + 40, "Y: $"       + hex(nes.getCpu()->y, 2) + "  [" + std::to_string(nes.getCpu()->y) + "]");
		DrawString(x, y + 50, "Stack P: $" + hex(nes.getCpu()->stkp, 4));
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
					DrawString(x, nLineY, (*it_a).second);
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
					DrawString(x, nLineY, (*it_a).second);
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
	Clear(olc::DARK_BLUE);

	
	
	if (bEmulationRun)
	{
		if (fResidualTime > 0.0f)
			fResidualTime -= fElapsedTime;
		else
		{
			fResidualTime += (1.0f / 60.0f) - fElapsedTime;
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







