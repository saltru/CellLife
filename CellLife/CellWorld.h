#pragma once
#include <vector>
#include <memory>
#include <string>
#include <random>
#include <ctime>
#include "stdafx.h"
#include "CellBot.h"
#include "CellWorldPhysics.h"
#include "TheMatrixPhysics.h"

using namespace std;

enum CellStatus
{
	CELL_EMPTY = 0,
	CELL_WALL,
	CELL_DEAD_BODY,	

	CELL_BOT
};

enum WorldError 
{
	WORLD_OK = 0,
	WORLD_CELL_IS_NOT_EMPTY,
	WORLD_OUT_OF_RANGE,

	WORLD_OTHER
};

class CellBot;

class CellWorld
{
private:
	vector<vector<unsigned int>> world;
	unsigned int worldSizeX;
	unsigned int worldSizeY;

	mt19937 rng;

	sharedTheMatrixPhysics worldPhysics;
	vector<shared_ptr<CellBot>> bots;

public:
	CellWorld(unsigned int width, unsigned int height, shared_ptr<TheMatrixPhysics> WorldPhysics) : worldSizeX(width), worldSizeY(height)
	{
		world.clear();
		world.resize(worldSizeX, vector<unsigned int>(worldSizeY, 0));
		bots.clear();
		worldPhysics = WorldPhysics;
		
		rng.seed(time((unsigned int)0));
	}

	~CellWorld() {}

	unsigned int getSizeX() const { return worldSizeX; }
	unsigned int getSizeY() const { return worldSizeY; }

	void ResetWorld()
	{	
		world.clear();
		world.resize(worldSizeX, vector<unsigned int>(worldSizeY, 0));
		bots.clear();
		worldPhysics.reset();

		rng.seed(time((unsigned int)0));
	}

	bool outOfRange(unsigned int x, unsigned int y) const
	{
		return !( (x < worldSizeX) && (y < worldSizeY) );		
	}

	bool isEmpty(unsigned int x, unsigned int y) const
	{
		return ((int)CELL_EMPTY == world[x][y]);
	}

	sharedTheMatrixPhysics getWorldPhysics() { return worldPhysics; }
	 
	int getBotsCount() const { return bots.size(); }


	CellStatus getStatus(unsigned int x, unsigned int y) const;
	//unsigned int getBotID(unsigned int x, unsigned int y) const;
	unsigned int getBotIndex(unsigned int x, unsigned int y) const;

	WorldError fillCell(unsigned int x, unsigned int y, CellStatus status);

	WorldError AddBot(unsigned int x, unsigned int y, shared_ptr<CellBot> bot);

	WorldError DeleteBot(sharedCellBot bot, CellStatus newCellStatus);
	WorldError DeleteBot(unsigned int id, CellStatus newCellStatus);

	unsigned int getRandomNumber() { return rng(); }

	void tick(); //next epoch

	void RenderAll(HDC hdc, int x, int y, int cellSize, bool borders, int levelOfDetails) const;
};

typedef shared_ptr<class CellWorld> sharedCellWorld;
typedef weak_ptr<class CellWorld> weakCellWorld;