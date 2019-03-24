#pragma once
#include <memory>
#include "stdafx.h"

using namespace std;

class CellWorld;

enum BotStatus
{
	BOT_NORMAL = 0,
	BOT_DEAD
};

class CellBot
{
protected:
	unsigned int id; //unique bot ID
	unsigned int x, y;
	BotStatus status;
	
public:
	CellBot() : id(), x(0), y(0), status(BOT_NORMAL) {}
	CellBot(int ID, unsigned int X, unsigned int Y, BotStatus Status = BOT_NORMAL) : id(ID), x(X), y(Y), status(Status) {}
	~CellBot() {}

	unsigned int getX() const { return x; }
	unsigned int getY() const { return y; }
	BotStatus getStatus() const { return status; }
	unsigned int getID() const { return id;  }

	void setX(const unsigned int X) { x = X; }
	void setY(const unsigned int Y) { y = Y; }
	void setStatus(const BotStatus Status) { status = Status; }
	void setID(unsigned int ID) { id = ID; }


	virtual void nextMove(CellWorld *world) { } //return true is the bot is still alive after move
	virtual void updateStatus() { }

	virtual void Render(HDC hdc, int x, int y, int cellSize, int levelOfDetails) const
	{
		HBRUSH hCellBrush = CreateSolidBrush(RGB(0, 255, 0));
		SelectObject(hdc, hCellBrush);
		Rectangle(hdc, x, y, x + cellSize + 1, y + cellSize + 1);
		DeleteObject(hCellBrush);
	}

	virtual void RenderBotInfo(HDC hdc, int x, int y) const {}
};

typedef shared_ptr<CellBot> sharedCellBot;
typedef weak_ptr<CellBot> weakCellBot;