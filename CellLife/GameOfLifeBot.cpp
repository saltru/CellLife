#include "stdafx.h"
#include "GameOfLifeBot.h"
#include "CellWorld.h"

//Deprecated: need Render function

//int GameOfLifeBot::getColorR() const { return (newCell) ? 0 : 0; }
//int GameOfLifeBot::getColorG() const { return (newCell) ? 0 : 200; } //older cells is green
//int GameOfLifeBot::getColorB() const { return (newCell) ? 150 : 0; } //new cell is blue

void GameOfLifeBot::nextMove(CellWorld *world)
{
	int bots_near = 0;

	if (world)
	{
		setStatus(BOT_DEAD); //let's assume that bot is dead and check if its alive
		for (unsigned int i = x - 1; i <= x + 1; ++i)
			for (unsigned int j = y - 1; j <= y + 1; ++j)
			{
				if ((x == i) && (y == j))
					continue;
				if (world->getStatus(i, j) == CELL_BOT)
					++bots_near;
			}

		if ((2 == bots_near) || (3 == bots_near))
		{
			newCell = false; //bot still alive and gets older
			setStatus(BOT_NORMAL);
		}
	}
}