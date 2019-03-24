#include <string>
#include "stdafx.h"
#include "CellBot.h"
#include "CellWorld.h"

using namespace std;

CellStatus CellWorld::getStatus(unsigned int x, unsigned int y) const
{
	if (outOfRange(x, y))
		return CELL_WALL;

	switch (world[x][y])
	{
	case CELL_EMPTY:
	case CELL_WALL:
	case CELL_DEAD_BODY:
	case CELL_BOT:
		return (CellStatus) world[x][y];
		break;
	}

	return CELL_WALL;
}

WorldError CellWorld::fillCell(unsigned int x, unsigned int y, CellStatus status)
{
	if (outOfRange(x, y))
		return WORLD_OUT_OF_RANGE;

	switch (status)
	{
	case CELL_EMPTY:
	case CELL_WALL:
	case CELL_DEAD_BODY:
		world[x][y] = status;
		break;

	case CELL_BOT:
	default:
		return WORLD_OTHER;
		break;
	}

	return WORLD_OK;
}


WorldError CellWorld::AddBot(unsigned int x, unsigned int y, shared_ptr<CellBot> bot)
{
	if (outOfRange(x, y))
		return WORLD_OUT_OF_RANGE;

	if (!isEmpty(x, y))
		return WORLD_CELL_IS_NOT_EMPTY;

	bot->setID(bots.size());
	bot->setX(x);
	bot->setY(y);

	bots.push_back(bot);

	world[x][y] = CELL_BOT;

	return WORLD_OK;
}

//unsigned int CellWorld::getBotID(const unsigned int x, const unsigned int y) const
//{
//	for (unsigned int i = 0; i < bots.size(); ++i)
//	{
//		if ((bots[i]->getX() == x) && (bots[i]->getY() == y))
//		{
//			return bots[i]->getID();
//		}
//	}
//}

unsigned int CellWorld::getBotIndex(const unsigned int x, const unsigned int y) const
{	
	for (unsigned int i = 0; i < bots.size(); ++i) {
		if ((bots[i]->getX() == x) && (bots[i]->getY() == y))
			return i;
	}

	return UINT_MAX; //Inconsistency
}

WorldError CellWorld::DeleteBot(sharedCellBot bot, CellStatus newCellStatus = CELL_EMPTY)
{
	unsigned int id = (bot) ? bot->getID() : MAXUINT;
	return DeleteBot(id, newCellStatus);
}

WorldError CellWorld::DeleteBot(unsigned int id, CellStatus newCellStatus = CELL_EMPTY)
{
	for (unsigned int i = 0; i < bots.size(); ++i)
	{
		if (bots[i]->getID() == id)
		{
			unsigned int x = bots[i]->getX();
			unsigned int y = bots[i]->getY();

			bots.erase(bots.begin() + i);
			fillCell(x, y, newCellStatus);

			return WORLD_OK;
		}
	}

	return WORLD_OTHER; //id not found
}

void CellWorld::tick() //next epoch
{	
	unsigned int i = 0;
	//bots doing their moves
	unsigned int bots_count = bots.size();
	for (i = 0; i < bots_count; ++i)
		bots[i]->nextMove(this);
	
	//world's move (like createing new bots in GameOfLife)
	worldPhysics->tick(this);

	//dead bots cleanup
	i = 0;
	while (i < bots.size())
	{
		if (bots[i]->getStatus() == BOT_DEAD)  //kill all dead bots at once to prevent influence to each other
		{
			unsigned int x = bots[i]->getX();
			unsigned int y = bots[i]->getY();

			DeleteBot(bots[i]);
			fillCell(x, y, CELL_DEAD_BODY);
		}
		else
			++i;
	}

	for (i = 0; i < worldSizeX; ++i)
		for (unsigned int j = worldSizeY - 1; j == 0; --j) //Gravity: dead bodies (ornagic) are falling down
			if ((getStatus(i, j) == CELL_DEAD_BODY) &&
				(getStatus(i, j + 1) == CELL_EMPTY))
			{
				fillCell(i, j + 1, CELL_DEAD_BODY);
				fillCell(i, j, CELL_EMPTY);
			}

}

void CellWorld::RenderAll(HDC hdc, int x, int y, int cellSize, bool borders, int levelOfDetails = 0) const
{	
	wstring txt(L"Generation: " + to_wstring(worldPhysics->getGenCount()) + 
		        L"\nTime: " + to_wstring(worldPhysics->getDaytime()) +
		        L"\nDay To Night Delta: " + to_wstring(worldPhysics->getDayToNightDelta()) +
		        L"\nDate of the year: " + to_wstring(worldPhysics->getDayOfTheYear()) +
		        L"\nWinter To Summer Delta: " + to_wstring(worldPhysics->getWinterToSummerDelta()) +
		        L"\n\nEnergyOnTop: " + to_wstring(worldPhysics->getSunPower(0))
				);
	RECT rc = { x, y, x + 250, y + 130 };
	DrawText(hdc, txt.c_str(), txt.length(), &rc, DT_TOP | DT_LEFT);

	int matrixX = 0;
	int matrixY = 130;

	HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
	HBRUSH hBrushOld = (HBRUSH)SelectObject(hdc, hBrush);
	
	HPEN hBorderPen = CreatePen((borders) ? PS_SOLID : PS_NULL, 1, RGB(0, 0, 0));
	HPEN hPenOld = (HPEN)SelectObject(hdc, hBorderPen);

	for (unsigned int i = 0; i < worldSizeX; ++i)
	{
		for (unsigned int j = 0; j < worldSizeY; ++j)
		{
			HBRUSH hCellBrush = nullptr;
			CellStatus status = getStatus(i, j);
			if (status == CELL_BOT)
			{
				int bot_ind = getBotIndex(i, j);
				if (bot_ind == UINT_MAX)
					throw std::out_of_range("Cell marked as CELL_BOT, but there is no bot (getBotIndex failed)");
				bots[bot_ind]->Render(hdc, matrixX + x + i * cellSize, matrixY + y + j * cellSize, cellSize, levelOfDetails);
			}
			else
			{
				switch (status)
				{
				case CELL_EMPTY:
					hCellBrush = CreateSolidBrush(RGB(255, 255, 255));
					break;
				case CELL_WALL:
					hCellBrush = CreateSolidBrush(RGB(70, 70, 70));
					break;
				case CELL_DEAD_BODY:
					hCellBrush = CreateSolidBrush(RGB(150, 150, 150));
					break;
				}
				SelectObject(hdc, hCellBrush);
				Rectangle(hdc, matrixX + x + i * cellSize,
					matrixY + y + j * cellSize,
					matrixX + x + (i + 1) * cellSize + 1,
					matrixY + y + (j + 1) * cellSize + 1);

				DeleteObject(hCellBrush);
			}
		}
	}

	SelectObject(hdc, hBrushOld);
	SelectObject(hdc, hPenOld);

	DeleteObject(hBrush);
	DeleteObject(hBorderPen);
	   
}