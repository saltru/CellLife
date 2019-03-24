#include <vector>
#include "stdafx.h"
#include "GameOfLifePhysics.h"
#include "CellWorld.h"

void GameOfLifePhysics::tick(CellWorld * world)
{
	if (nullptr == world)
		return;

	int worldSizeX = world->getSizeX();
	int worldSizeY = world->getSizeY();
	vector<sharedCellBot> botsToAdd;
	int nextID = world->getBotsCount();

	for (int i = 0; i < worldSizeX; ++i)
		for (int j = 0; j < worldSizeY; ++j)
		{			
			if (world->isEmpty(i, j))
			{
				int bots_near = 0;
				for (int x = i - 1; x <= i + 1; ++x)
					for (int y = j - 1; y <= j + 1; ++y)
					{
						if (world->getStatus(x, y) == CELL_BOT)
							++bots_near;
					}

				if (3 == bots_near) //create a new bot in empty cell
				{
					botsToAdd.push_back(make_shared<GameOfLifeBot>(nextID, i, j));					
					nextID++;
				}
			}
		}

	//create bots
	for (auto &bot : botsToAdd)
	{
		world->AddBot(bot->getX(), bot->getY(), bot);
	}
}
