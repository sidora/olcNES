
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

void Demo_olc6502::DrawCpu()
{
	int x = 256 * 2;
	int y = 0;
	int sideWidth = GetDrawTargetWidth() - x;
	float backAreaWidth = sideWidth * 0.9f;
	int backAreaX = x + ((sideWidth - backAreaWidth) * 0.5f);
	


	FillRect(backAreaX, y + 8, backAreaWidth, cpuAreaHeight - 8, olc::Pixel(20, 20, 20));
	FillRect(backAreaX, y + 8, backAreaWidth, cpuAreaHeight *0.15f, olc::Pixel(90, 90, 90));
	DrawString(x+ backAreaWidth*0.5f, y + 4 + cpuAreaHeight * 0.075f, "CPU", olc::Pixel(20, 20, 20));

	olc::Pixel onCol = olc::Pixel(101, 252, 190);
	olc::Pixel ofCol = olc::Pixel(0, 0, 0);


	int Ofst = 80;
	int Spce = 14;
	int newX = x + 30;
	int newY = y + 28;
	
	DrawString(newX, newY, "  FLAGS -", txtCol);
	DrawString(newX + Ofst + (Spce*0),newY, "N", nes.getCpu()->status & olc6502::N ? onCol : ofCol);
	DrawString(newX + Ofst + (Spce*1),newY, "V", nes.getCpu()->status & olc6502::V ? onCol : ofCol);
	DrawString(newX + Ofst + (Spce*2),newY, "B", nes.getCpu()->status & olc6502::B ? onCol : ofCol);
	DrawString(newX + Ofst + (Spce*3),newY, "I", nes.getCpu()->status & olc6502::I ? onCol : ofCol);
	DrawString(newX + Ofst + (Spce*4),newY, "Z", nes.getCpu()->status & olc6502::Z ? onCol : ofCol);
	DrawString(newX + Ofst + (Spce*5),newY, "C", nes.getCpu()->status & olc6502::C ? onCol : ofCol);

	DrawString(newX, newY + 12, "     PC - $" + hex(nes.getCpu()->pc, 4), txtCol);
	DrawString(newX, newY + 24, "      A - $" + hex(nes.getCpu()->a,  2) + "  [" + std::to_string(nes.getCpu()->a) + "]", txtCol);
	DrawString(newX, newY + 36, "      X - $" + hex(nes.getCpu()->x,  2) + "  [" + std::to_string(nes.getCpu()->x) + "]", txtCol);
	DrawString(newX, newY + 48, "      Y - $" + hex(nes.getCpu()->y,  2) + "  [" + std::to_string(nes.getCpu()->y) + "]", txtCol);
	DrawString(newX, newY + 60, "Stack P - $" + hex(nes.getCpu()->stkp, 4), txtCol);
}

void Demo_olc6502::DrawPpu()
{
	// Draw Palettes & Pattern Tables ==============================================
	const int nSwatchSize = 6;
	for (int p = 0; p < 8; p++) // For each palette
		for (int s = 0; s < 4; s++) // For each index
			FillRect(516 + p * (nSwatchSize * 5) + s * nSwatchSize, 340,
				nSwatchSize, nSwatchSize, nes.getPpu()->GetColourFromPaletteRam(p, s));

	// Draw selection reticule around selected palette
	DrawRect(516 + nSelectedPalette * (nSwatchSize * 5) - 1, 339, (nSwatchSize * 4), nSwatchSize, olc::WHITE);

	// Generate Pattern Tables
	
	DrawSprite(516, 348, &nes.getPpu()->GetPatternTable(0, nSelectedPalette));
	DrawSprite(648, 348, &nes.getPpu()->GetPatternTable(1, nSelectedPalette));

	// Draw rendered output ========================================================
	DrawSprite(0, 0, &nes.getPpu()->GetScreen(), 2);
}


void Demo_olc6502::DrawCode(int nLines)
{
	int x = 256 * 2;
	int y = cpuAreaHeight+20;

	int newX = x + 6;
	int spce = 12;

		auto it_a = mapAsm.find(nes.getCpu()->pc);
		int nLineY = (nLines >> 1) * spce + y;
		if (it_a != mapAsm.end())
		{
			DrawString(newX, nLineY, (*it_a).second, olc::Pixel(91, 249, 240));
			while (nLineY < (nLines * spce) + y)
			{
				nLineY += spce;
				if (++it_a != mapAsm.end())
				{
					DrawString(newX, nLineY, (*it_a).second, txtCol);
				}
			}
		}

		it_a = mapAsm.find(nes.getCpu()->pc);
		nLineY = (nLines >> 1) * spce + y;
		if (it_a != mapAsm.end())
		{
			while (nLineY > y)
			{
				nLineY -= spce;
				if (--it_a != mapAsm.end())
				{
					DrawString(newX, nLineY, (*it_a).second, txtCol);
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

	// Sneaky peek of controller input in next video! ;P


	nes.controller[0] = 0x00;
	nes.controller[0] |= GetKey(olc::Key::X).bHeld ? 0x80 : 0x00;
	nes.controller[0] |= GetKey(olc::Key::Z).bHeld ? 0x40 : 0x00;
	nes.controller[0] |= GetKey(olc::Key::A).bHeld ? 0x20 : 0x00;
	nes.controller[0] |= GetKey(olc::Key::S).bHeld ? 0x10 : 0x00;
	nes.controller[0] |= GetKey(olc::Key::UP).bHeld ? 0x08 : 0x00;
	nes.controller[0] |= GetKey(olc::Key::DOWN).bHeld ? 0x04 : 0x00;
	nes.controller[0] |= GetKey(olc::Key::LEFT).bHeld ? 0x02 : 0x00;
	nes.controller[0] |= GetKey(olc::Key::RIGHT).bHeld ? 0x01 : 0x00;

	if (GetKey(olc::Key::SPACE).bPressed) bEmulationRun = !bEmulationRun;
	if (GetKey(olc::Key::R).bPressed) nes.reset();
	if (GetKey(olc::Key::P).bPressed) (++nSelectedPalette) &= 0x07;
	
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



	DrawCpu();
	DrawCode(26);
	DrawPpu();

	return true;

};







